# Kairo symbol indexing — status and next architecture

Handoff document. Written 2026-07-28. Everything below was measured on this
machine against stage 1 as of that date, not estimated.

> **Status update, same day — all five work-order items are built**, plus a
> column off-by-one fix. `kbld --get-drivers`, stage 0 `--index-file`, eager
> whole-workspace indexing, `.cache/kairo-index/` persistence, and the debounce
> have all landed. What changed versus the plan below is recorded in §7 —
> including two of the plan's assumptions that turned out to be wrong, and the
> lazy-indexing decision that the first editor session disproved.

---

## 1. What exists and works today

Three Python files in `Lib/bootstrap/lib-helix/scripts/` (copied into
`build/release/arm64-macosx/scripts/` by the `after_build` glob in
`xmake.lua:414`, which is what the VS Code extension actually runs):

| file | role |
|---|---|
| `kairo_index.py` | runs stage 0, parses its AST JSON, builds flat symbol tables |
| `kairo_resolve.py` | answers "what does this symbol refer to", ranked |
| `lsp-server.py` | `textDocument/definition`, `documentSymbol`, `workspace/symbol` |

Measured on stage 1's 144-file import closure:

```
143 files, 10926 decls, 434 types, 8500 bindings, 29340 accesses
index build from a saved AST dump: 2.3s
resolution:                        ~110,000 sites/sec
```

Resolution quality over all 29,340 member-access sites:

| tier | share | single candidate |
|---|---|---|
| `exact`  — receiver type known syntactically | 33.1% | 94.1% |
| `typed`  — type known, member not in its closure | 14.8% | 29.2% |
| `guess`  — receiver type unknown | 52.1% | 30.9% |

Cross-mixin resolution works: `self.parse_decl` in `ASTParse.k` resolves to
`DeclParse::parse_decl` at `DeclParse.k:113` by walking the seven `derives`
mixins. Go-to-definition, outline, and workspace symbol search all function.

**Do not rebuild any of this.** The extraction, the resolver, and the LSP
wiring are correct and tested. Only the *refresh strategy* is wrong.

---

## 2. The blocker

Editor autosave fires every **100 ms**. The current refresh path costs
**~9 seconds per save**, because refreshing one file re-runs stage 0 over that
file's entire import closure:

```
indexed CompilerInstance.k: +1 files in 9.2s
```

Nine seconds of CPU to update one file, triggered ten times a second. This is
not a tuning problem; the design is wrong.

Worth knowing: the pre-existing diagnostics path in `lsp-server.py`
(`parse()`, which runs stage 0 with `--emit-ir` and a 10 s timeout) *also*
fires on every save and *also* times out on large files. At 100 ms autosave
that path is already pathological on its own, independent of indexing.

---

## 3. Target architecture

Four pieces. The key insight is that **cross-file linking happens in the Python
index, not in stage 0** — the resolver looks symbols up by name across all
files. So stage 0 never needs to expand imports for indexing purposes. It only
needs to hand back one file's declarations, fields, bindings, and access sites.

```
kbld --get-drivers ──► entry points + include flags
         │
         ▼
   cold index: one closure parse per driver entry ──► .cache/kairo-index/
         │
         ▼
   warm reindex: single-file parse of the changed file only ──► Index.absorb()
```

### 3.1 `kbld --get-drivers`

Emit the build graph's entry points as JSON so the indexer knows what to index
and with which flags, instead of guessing from `compile_commands.json`.

Everything needed already exists in `kbld/src/main.cc`:

- `struct Target { name, entry, kind, includes, links, libs, deps, defines, ... }` — line ~107
- `Config::targets` — the driver list, already parsed
- `Command::Index` already exists (line 441 in `parse_cli`, dispatched at 1333)
- `execute_index()` at line 1089 regenerates `compile_commands.json`

So this is roughly 30 lines: a new `Command::GetDrivers` (or a `--json` flag on
the existing `index` command) that serialises `cfg.targets`. Suggested shape:

```json
{"drivers":[{"name":"kairo","entry":"Compiler/Driver/Kairo.k","kind":"binary",
             "includes":["Compiler","Lib/bootstrap/lib-helix/core","Extras/KaTAR","Linker"],
             "defines":[]}]}
```

### 3.2 Stage 0 single-file parse mode

New CLI flag — name it something explicit like `--index-file`. Semantics:
lex and parse **this file only**, emit its AST JSON, do not expand imports, do
not run codegen, do not link.

The hook already exists. `ImportProcessor` has:

```cpp
void* override_processable_imports = nullptr;
// if 0xFFF is present then we stop processing imports
```
— `source/parser/preprocessor/include/preprocessor.hh:78`

It is already set to `0xFFF` on internal bail-out paths
(`source/parser/preprocessor/source/import_preprocessor.cc` lines 506, 536,
563, 584, 608, 614). The new mode sets it up front instead of on error.

Wiring points:

| what | where |
|---|---|
| flag declaration | `source/controller/source/cli.cc:67` (next to `args::Flag emit_ast`) |
| flag storage | `source/controller/include/cli/cli.hh:128` (next to `bool emit_ast`) |
| pipeline | `source/controller/source/core/compilation_unit.cc:229-250` |

The pipeline in `compilation_unit.cc` is already the right shape:

```cpp
TokenList tokens = pre_process(parsed_args, enable_logging);   // ← closure expansion lives here
ast = parse_ast(tokens, in_file_path);
if (parsed_args.emit_ast) { Jsonify json_visitor; ast->accept(json_visitor); ... }
```

Single-file mode short-circuits the first line and leaves the rest untouched,
so the emitted JSON keeps exactly the shape `kairo_index.py` already parses.

**Macro risk: retracted — verified void on 2026-07-28.** An earlier draft of
this document flagged macro expansion as a risk to validate before building.
It is not a risk: stage 0 does not expand macros at all.
`source/parser/preprocessor/source/macro_preprocessor.cc` is a stub —
`MacroProcessor` is declared inside that `.cc` and never instantiated anywhere
in the tree, `parse_invoke()` unconditionally returns `false`, and the
`KEYWORD_MACRO` case in `parse()` falls through to `default`. `pre_process()`
never references it. Import closure expansion is the *only* thing the
preprocessor does, so disabling it is exactly and only what the flag name says.

**Target cost:** well under 500 ms for one file. If it lands above ~1 s, the
mode is not doing what it should and something is still walking the closure.

### 3.3 Cache

Persist the extracted tables, not the raw AST — the AST is ~55 MB of JSON for
stage 1, the flat tables are a fraction of that.

- Location: `.cache/kairo-index/` (add to `.gitignore`)
- One entry per source file, keyed by absolute path
- Invalidate on `(mtime, size)`; use a content hash if that proves flaky
- Store a format version so a schema change invalidates everything cleanly

Cold start becomes: load cache → for any file whose stamp changed, single-file
reparse → done. Full closure parse only when the cache is absent or the format
version moved.

### 3.4 Debounce — mandatory regardless of speed

Even at 200 ms per single-file reparse, a 100 ms autosave still means spawning
processes faster than they finish. The refresh must:

- coalesce saves per file on an idle timer (**300–500 ms** of quiet)
- cancel or discard an in-flight reparse when the same file changes again
- never let more than one reparse per file be in flight

This applies to the diagnostics path too, which currently has no debounce at
all and is the more expensive of the two.

---

## 4. Work order

1. `kbld --get-drivers` — small, self-contained, unblocks the rest
2. Stage 0 `--index-file` — the macro risk is retracted (see §3.2); build it
3. Rewire `IndexManager` in `lsp-server.py`: drivers for cold build,
   single-file for warm refresh
4. Debounce (do not skip; the 100 ms autosave defeats everything without it)
5. `.cache/kairo-index/` persistence — last, it is an optimisation on top of a
   design that already works

---

## 5. Things already fixed — do not re-introduce

These were live bugs, found and fixed on 2026-07-28. Each one is a trap that a
rewrite would fall back into:

- **`a.b` and `a->b` are different AST nodes.** Only `.` is a `DotPathExpr`;
  `->` is a `BinaryExpr` with `op.kind == "->"`. Stage 1 has 17,385 of the
  former and 11,955 of the latter. Handling one loses ~40% of accesses.
- **`FuncDecl.name` is a `PathExpr`, not an `IdentExpr`.** Methods can be
  declared out-of-line with a qualified path
  (`fn std::Panic::FrameContext::crash()`), and the qualified prefix must win
  over the lexically enclosing scope for owner attribution.
- **`CompileCommands.commands` is empty until `.load(path)` is called.**
  Reading it cold yields `[]`, so no `-I` flags, so stage 0 resolves no imports
  and only 7 prelude files get indexed.
- **Stage 0 resolves relative `-I` paths against `PWD`, not the process cwd.**
  An editor-hosted server inherits `PWD=/`. Set `env["PWD"]` explicitly.
- **`pygls` inspects the handler signature.** A decorator without
  `functools.wraps` makes every handler fail with "missing 1 required
  positional argument".
- **Never drop before you have the replacement.** Dropping a file's entries and
  then rebuilding leaves a window where its symbols do not exist; readers query
  without a lock and see every jump into that file fail. Stage into a throwaway
  `Index`, then `Index.absorb(staged, path)`. Verified zero misses across 34
  polls during a rebuild.
- **An index that fails to build must never replace the one that works.**
  Building the eager index with a compiler that lacked `--index-file` yielded
  0 decls from 540 files. The old code then marked all 540 `seen`, cached the
  empty results, and installed the empty index -- so go-to-definition returned
  `[]` for everything, the lazy closure fallback was suppressed, and the
  poisoned cache made it survive restarts. Three separate guards now exist:
  an index with zero declarations is discarded rather than installed, only
  files that actually produced an AST are marked seen or cached, and empty
  cache entries are treated as stale on load. A wrong binary should cost
  speed, never correctness.
- **Files that fail a standalone parse must be left unseen.** That is exactly
  the case where `ensure()`'s closure walk can still succeed, because it
  supplies the imports the standalone parse lacked. Marking them seen to avoid
  re-parsing trades a few milliseconds for a permanent capability loss.
- **Stage 0's AST output is NDJSON**, one document per translation unit, with a
  `debug: ` prefix and ANSI colour codes. `json.load()` on the whole stream
  fails with "Extra data: line 2".

---

## 6. Known limits, by design

- **C++ / FFI symbols are invisible.** `Logger` is declared in
  `Compiler/Native/KLog.hh`, so it has no `.k` declaration and go-to-definition
  correctly finds nothing. Jumping into headers would need a separate small
  indexer over `Compiler/Native/*.hh` producing the same `Decl` shape.
- **The 52% `guess` tier is dominated by one missing rule.** 9,311 of 15,288
  guesses (61%) are a named receiver with no binding — i.e. unannotated
  `var x = <expr>`. Inferring the type from the right-hand side (constructor
  call, callee return type, generic argument) is the single highest-value
  addition, and it is the first real sema rule. Deliberately left unwritten.
- The `typed` tier means the receiver type is known but the member is not in
  its `derives` closure — usually an `inline "c++"` member or an FFI type. It
  reports honestly rather than silently widening.

---

## 7. What was actually built — 2026-07-28

Implemented in this order. Two of the plan's assumptions were wrong; both are
corrected here rather than in the sections above, so the original reasoning
stays legible next to what it turned into.

### 7.1 `kbld --get-drivers` — done

`kbld/src/main.cc`. New `Command::GetDrivers`, reachable as either
`kbld drivers` or `kbld --get-drivers`. Emits one JSON document on stdout;
every `_I_log` diagnostic already goes to stderr, so the stream is clean —
none of stage 0's NDJSON/ANSI handling is needed on the consumer side.

Payload carries **`root`** in addition to the fields §3.1 listed. That is not
cosmetic: `includes` are reproduced verbatim from `build.k` and are usually
relative, and stage 0 resolves relative `-I` against `$PWD` rather than the
process cwd (§5, trap 4). Without `root` a consumer cannot set `PWD` correctly.

```json
{"version":1,"root":"/abs/path","compiler":"kairo","mode":"release",
 "drivers":[{"name":"kairo","kind":"binary","entry":"/abs/entry.k",
             "includes":["Compiler","Lib/bootstrap/lib-helix/core"],"defines":[]}]}
```

**Correction to §3.1's premise.** This branch (`archive/beta-helix-0.0.1`) has
no `build.k` — stage 0 builds with `xmake.lua`, and the only `build.k` in the
tree is an aspirational sketch under `pkgs/std/arc/` that imports modules which
do not exist. `--get-drivers` is therefore *not* usable for indexing stage 0's
own sources; `compile_commands.json` remains the source there.

It *is* usable for stage 1, which is the actual indexing target: `main` has a
real `build.k` declaring eight targets via an `llvm_tool()` helper —
`kairo`, `kbld`, `kfmt`, `kld`, `kals`, `kpkg`, `katar`,
`clang-diag-identity` — with include sets like
`["Compiler", "Lib/bootstrap/lib-helix/core", "Extras/KaTAR", "Linker"]`.
That is exactly the shape `--get-drivers` emits.

**Known blocker, pre-existing and unrelated to this change:** running
`kbld --get-drivers` against stage 1's `build.k` fails before reaching the new
code — kbld compiles `build.k` to `build/.kbld/build_script`, that compile
silently produces no binary, and the run dies with exit 127. Needs its own
investigation.

### 7.2 Stage 0 `--index-file` — done

Flag declared in `source/controller/source/cli.cc`, stored in
`cli.hh`, implies `--emit-ast --lsp-mode`. Pipeline branch in
`compilation_unit.cc:pre_process`.

Two things had to be skipped, not one. Disabling the recursive walk is the
obvious half; the forced core import is the expensive half — `force_import`
runs a **full recursive `build_unit` plus `generate_cxir` on `core.k`**, which
dominates the cost for small files.

**The macro risk was the wrong risk.** §3.2 flagged macro expansion; macros are
not implemented at all (see §3.2's retraction). The real blocker was found by
running the thing: **the parser never sees `import` statements**, because
`process()` always deletes their tokens after expanding them. Simply declining
to follow imports leaves the syntax in the token stream and `Program::parse`
aborts with an empty `children`. Files with zero imports parsed fine; every
file with one or more aborted — which is what made the mechanism obvious.

So `--index-file` must *strip* imports, not merely decline to follow them:
`ImportProcessor::strip_imports()` in `import_preprocessor.cc`. It reuses
`process()`'s span logic and covers both spellings — bare `import ...;` and the
`ffi "c++" import "...";` / `ffi "c++" { import ...; }` forms, where the span
has to start at the `ffi` keyword or a dangling `ffi "c++"` is left behind. An
`ffi` block containing no import is real code and is left alone.

Two details worth keeping:
- `TokenList::remove` is **half-open** (`erase(start_it, end_it)`), so the span
  must close one token past the terminator. Leaving the `;` behind yields a
  stray empty `ExprState` per stripped import.
- Stage 0 *requires* terminators, so scanning for `;`/`}` is exact. An earlier
  version bounded the scan on statement-starting keywords to tolerate
  `ffi "c++" import "filesystem"` with no semicolon; that file
  (`__new_tooling.k`) is simply a draft with invalid syntax, and the heuristic
  risked truncating valid multi-line imports. Removed.

**Measured on stage 1** (489 `.k` files on `main`, stage 0 release binary,
`-I Compiler -I Lib/bootstrap/lib-helix/core -I Extras/KaTAR -I Linker` for
the baseline arm):

| file | `--index-file` | baseline | speedup | nodes kept |
|---|---|---|---|---|
| `Compiler/Parser/DeclParse.k` | 336 ms | 2.27 s | 6.8× | 10980 / 11006 |
| `Compiler/Parser/ASTParse.k`  | 151 ms | 3.29 s | 21.8× | 5833 / 5883 |
| `Compiler/Lexer/Lexer.k`      | 154 ms | 1.06 s | 6.9× | 5410 / 5432 |
| `Compiler/Driver/Kairo.k`     | 11 ms | 5.85 s | 532× | 200 / 226 |

Across the whole corpus: median 3 ms, p95 66 ms, max 474 ms per file — the
§3.2 target was "well under 500 ms", met. The missing nodes are the stripped
import statements themselves, which carry nothing the index wants.

Regressions in a 24-file sample including all four harsh files: **zero**.

**Caveat on the corpus, stated plainly.** A full 489-file regression sweep was
*not* run — it was cut short deliberately. Stage 0 cannot parse much of stage
1's newer syntax at all: only 212/489 files yield their own nodes under
`--index-file`, and spot-checking shows the baseline returns zero for those
same files in ~50 ms, i.e. they fail identically either way. That is consistent
with "no regressions" but is not the same as having proved it corpus-wide.

**A methodology trap worth recording**, because it produced two confidently
wrong numbers before being caught. The obvious success check — "does the output
contain `\"children\":[`" — is invalid for the baseline arm. Baseline emits one
NDJSON document *per translation unit* and the core closure parses first, so a
file whose own AST is empty still yields six healthy core documents and scores
as a pass. The only valid measure is counting nodes whose `filename` is the
target file itself. (`hello.k`, five lines, produced 441 KB of baseline output
containing zero references to `hello.k`.) Separately: `timeout(1)` does not
exist on macOS, and a comparison harness that shells out to it silently scores
every baseline run as a failure.

### 7.3 Warm/cold split + debounce — done

`lsp-server.py`:

- `Debouncer` — trailing per-key idle timer, 400 ms. At most one job per key in
  flight; a save landing mid-job re-arms rather than queueing. Unit-tested:
  20 saves at 100 ms collapse to 1 run, max concurrency per key is 1.
- `IndexManager.invalidate` → `_refresh`, the warm path: `ki.run_index_file`,
  staged into a throwaway `Index`, then `absorb`. Never drops before the
  replacement is in hand (§5, trap 6).
- `IndexManager._build` is now cold-path only; its dead `replace=True` branch
  is gone.
- `did_save` debounces **both** halves. The diagnostics run is re-fetched
  inside the timer, since the document has moved on by the time it fires.
- `did_close` cancels queued work for that file.
- Removed the per-save loop that logged the entire environment, one line per
  variable, inside `parse()`.

`kairo_index.py`: added `run_index_file()`; the NDJSON decoding shared with
`run_emit_ast` is factored into `_parse_ast_stream`.

### 7.4 Eager indexing + persistence — done (second pass)

Added after the first editor session, which surfaced two things.

**Lazy indexing was wrong.** `ensure()` only indexed a file's closure when that
file was *opened*, so the first jump into a not-yet-indexed file always missed
and only the second worked. Replaced with `IndexManager.bootstrap()`, fired
from `INITIALIZED`: it indexes the whole workspace on a background thread
before any request needs it.

This is only affordable because of `--index-file`. `build_index_eager()` runs
one single-file parse per `.k` file across a thread pool rather than closure
walks — faster, and strictly more complete, since a closure walk only reaches
files some driver imports and anything unreferenced would stay invisible.

`IndexCache` (`.cache/kairo-index/`, already in `.gitignore`) stores one JSON
entry per source file, keyed by a hash of its absolute path, stamped with
`(mtime_ns, size)` and `CACHE_VERSION`. Only the by-file tables are stored;
`decls_by_name`, `types_by_name` and `aliases` are derived and rebuilt by
replaying `add_decl` on load. Entries are written via write-then-rename so a
reader never sees a half-written file, and `_refresh` writes through on save.

Measured on stage 0's workspace (113 files): cold 0.51 s, cache-warmed 0.03 s.
Round-trip verified exact — `decls_by_file`, `bindings_by_file` and
`accesses_by_file` compare byte-identical between a cold build and a
cache-loaded one, as do `lookup()` and `members_of()`.

Files that parse to nothing are still marked seen, so they are neither
re-parsed on every `ensure()` nor counted differently between a cold and a
warm start.

### 7.5 Column off-by-one — fixed

`_decl_location` subtracted 1 from the column, putting every jump one
character early. Stage 0's two location fields do **not** share a base:
`line_number` is 1-based (so `line - 1` is right for LSP) but `column_number`
is already 0-based, as is LSP's `Position.character`, so the column must pass
through untouched. Verified against `abi.k:15`, where col 8 lands exactly on
the `M` of `Module`, and `Tests/main.k:20`, where col 7 lands on `Fraction`.

The diagnostics path (`_convert_to_diagnostics`) uses `error["col"]` raw and
was **not** changed — that column anchors to the parser's current token rather
than the offending text, so it could not be shown to be off by one from the
available evidence. Worth a look if squiggles land one column right.

### 7.6 Not a bug: `defines` escaping in `--get-drivers`

`--get-drivers` reports defines like
`VERSION=L\\\"kairo-0.1.1+dev\\\"`. The extra backslashes are real and
intentional: `execute_build` assembles the compiler invocation as a single
**shell string**, so `version_macro()` in stage 1's `build.k` pre-escapes the
quotes to survive that hop. The JSON pipeline does not double-escape — a
controlled `.define("VERSION=L\"x\"")` emits correct single escaping.

Consumers that build an argv list instead of a shell string (anything using
`subprocess` without `shell=True`) will need to strip one layer. `--index-file`
takes no flags at all, so the indexer is unaffected.

### 7.7 Indexing progress in the editor — done

Server-side only; no client change. Standard LSP work-done progress, which is
the same mechanism clangd's indexing indicator uses, so `vscode-languageclient`
renders it in the status bar with no extension work.

`IndexManager._bootstrap` creates a token via
`window/workDoneProgress/create`, then emits `$/progress` `begin` → `report`
→ `end`, with the report count fed from `build_index_eager`'s `progress`
callback. Reporting granularity is `max(1, len(todo) // 50)`, so the bar moves
smoothly on a small workspace without spamming a large one.

Guarded on the client actually advertising `window.workDoneProgress` — creating
progress against a client that did not would make the request fail. Every
progress call is wrapped so a failure degrades to no progress bar rather than
taking the index down with it.

Two pygls 2.0.0a2 specifics worth recording, both of which cost a debugging
round:

- The `Progress` object is `server.**work_done_progress**`, not
  `server.progress` — the latter is a plain method that sends a raw
  `$/progress` notification and has no `.create`. `hasattr(server, "progress")`
  returns True and is therefore actively misleading.
- Calling `create`/`begin`/`report`/`end` from a worker thread is safe under
  the stdio transport, despite `_send_data` containing an
  `asyncio.ensure_future` path: `StdoutWriter.write` is synchronous and returns
  `None`, so that branch is never taken, and header+body go out in a single
  `write` so they cannot interleave with the event loop's own writes. This
  would need revisiting for the websocket transport, whose writer *is*
  awaitable.

Verified end to end by driving the real server over stdio with a scripted
initialize handshake: cold start emits `begin`, 57 evenly spaced `report`s and
`end`; a cache-warm start emits `begin` then `end` with no stuck bar.
