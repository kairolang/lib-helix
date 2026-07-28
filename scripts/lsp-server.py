from concurrent.futures import Future
import functools
import json
import logging
import os
import pathlib
import re
import subprocess
import sys
import threading
import time
from contextlib import contextmanager
import traceback
from typing import Any, Callable, Dict, List, Optional
from urllib.parse import unquote, urlparse
from urllib.request import url2pathname
from pathlib import Path
import sys
import traceback
import tempfile

from lsprotocol.types import (
    INITIALIZED,
    TEXT_DOCUMENT_DID_CLOSE,
    TEXT_DOCUMENT_CODE_ACTION,
    TEXT_DOCUMENT_DID_OPEN,
    TEXT_DOCUMENT_DID_SAVE,
    TEXT_DOCUMENT_DEFINITION,
    TEXT_DOCUMENT_DOCUMENT_SYMBOL,
    WORKSPACE_SYMBOL,
    DefinitionParams,
    DocumentSymbolParams,
    Location,
    SymbolInformation,
    SymbolKind,
    WorkspaceSymbolParams,
    Diagnostic,
    DiagnosticSeverity,
    DidChangeTextDocumentParams,
    DidCloseTextDocumentParams,
    DidOpenTextDocumentParams,
    PublishDiagnosticsParams,
    DidSaveTextDocumentParams,
    Position,
    Range,
    CodeActionParams,
    CodeAction,
    ExecuteCommandParams,
    CodeActionKind,
    Command,
    TextDocumentItem,
    WorkDoneProgressBegin,
    WorkDoneProgressEnd,
    WorkDoneProgressReport,
)
from pygls.lsp.server import LanguageServer

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kairo_index as ki  # noqa: E402
import kairo_resolve as kr  # noqa: E402

# ---------------------------------------------------------------------- #
# Logging Configuration
# ---------------------------------------------------------------------- #
LOG_FILE = os.path.join(os.path.dirname(__file__), "lsp.log")
LOG_CLEAR_INTERVAL = 600  # seconds

logging.basicConfig(
    filename=LOG_FILE,
    level=logging.DEBUG,
    format="%(asctime)s - %(levelname)s - %(message)s",
)

logger = logging.getLogger("KairoLSP")
logger.propagate = True

def log_unhandled_exception(exc_type, exc_value, exc_traceback):
    """Global hook for all uncaught exceptions."""
    if issubclass(exc_type, KeyboardInterrupt):
        # Allow graceful shutdown without treating as error
        sys.__excepthook__(exc_type, exc_value, exc_traceback)
        return

    tb_str = ''.join(traceback.format_exception(exc_type, exc_value, exc_traceback))
    logger.critical("Uncaught exception:\n%s", tb_str)

    # Also write directly to an emergency file in case logging fails
    err_path = os.path.join(os.path.dirname(__file__), "error.log")
    with open(err_path, "a", encoding="utf-8") as f:
        f.write(f"\n[UNCAUGHT] {time.strftime('%Y-%m-%d %H:%M:%S')}\n{tb_str}\n")

# Install global exception handler
sys.excepthook = log_unhandled_exception

def safe_handler(fn):
    # functools.wraps is load-bearing, not cosmetic: pygls inspects the
    # handler's signature to decide whether to call it as (server, params) or
    # just (params).  A bare *args wrapper reads as single-argument and the
    # handler dies with "missing 1 required positional argument".
    @functools.wraps(fn)
    def wrapper(*args, **kwargs):
        try:
            return fn(*args, **kwargs)
        except Exception:
            logger.exception(f"Exception in handler {fn.__name__}")
    return wrapper

def clear_log_file() -> None:
    """Clears the log file content."""
    with open(LOG_FILE, "w"):
        pass

clear_log_file()  # empty log on start

@contextmanager
def timer():
    start = time.time()
    yield lambda: time.time() - start


# ---------------------------------------------------------------------- #
# Helper Path Comparison
# ---------------------------------------------------------------------- #
def compare_paths(path1, path2, case_sensitive=True):
    try:
        p1 = Path(path1).resolve()
        p2 = Path(path2).resolve()
    except (FileNotFoundError, OSError):
        p1 = Path(path1).absolute()
        p2 = Path(path2).absolute()

    if not case_sensitive:
        return str(p1).lower() == str(p2).lower()
    return p1 == p2


# ---------------------------------------------------------------------- #
# Optimized CompileCommands
# ---------------------------------------------------------------------- #
class CompileCommands:
    """Optimized class to handle compile_commands.json with caching and fast lookups."""

    def __init__(self, lsp) -> None:
        workspace = lsp.workspace.folders
        self.commands: list[str] = []
        self._cache: dict[str, list[str]] = {}
        self._lock = threading.Lock()

        if not workspace:
            logger.error("No workspace folder found.")
            self.path = None
            return

        uri = next(iter(workspace.values())).uri
        parsed = urlparse(uri)
        win_path = url2pathname(unquote(parsed.path))
        p = pathlib.Path(win_path).absolute()
        if p.drive:
            p = pathlib.Path(p.drive.upper() + str(p)[len(p.drive):])

        self.directory = str(p)
        self.path = str(p / "compile_commands.json")
        self._last_mtime: float = 0.0
        self._commands_map: dict[str, list[str]] = {}

    # ------------------------------------------------------------------ #
    def _reload_if_needed(self) -> None:
        """Reloads compile_commands.json only if modified."""
        if not self.path:
            return
        try:
            mtime = os.path.getmtime(self.path)
        except FileNotFoundError:
            return

        if mtime <= self._last_mtime:
            return

        with self._lock:
            try:
                with open(self.path, "r") as f:
                    data = json.load(f)
            except Exception as e:
                logger.error("Failed to load compile_commands.json: %s", e)
                return

            new_map: dict[str, list[str]] = {}

            for cmd in data:
                file_path = cmd.get("file")
                if not file_path:
                    continue
                abs_path = os.path.abspath(file_path)
                args = cmd.get("arguments")
                if not args:
                    continue

                match args:
                    case str() as arg_str:
                        new_map[os.path.normpath(abs_path).lower()] = arg_str.split()
                    case list() as arg_list if all(isinstance(a, str) for a in arg_list):
                        new_map[os.path.normpath(abs_path).lower()] = arg_list
                    case _:
                        logger.warning("Invalid compile command entry for: %s", file_path)

            self._commands_map = new_map
            self._last_mtime = mtime
            logger.debug("Reloaded compile_commands.json with %d entries", len(new_map))

    # ------------------------------------------------------------------ #
    def load(self, for_file: str) -> None:
        """Loads compile commands for a specific file, using cache when possible."""
        self.commands.clear()
        if not self.path:
            return

        self._reload_if_needed()
        key = os.path.normpath(os.path.abspath(for_file)).lower()

        # Cache hit
        if cached := self._cache.get(key):
            self.commands.extend(cached)
            logger.debug("Cache hit for %s", for_file)
            return

        if args := self._commands_map.get(key):
            self._cache[key] = args
            self.commands.extend(args)
            logger.debug("Loaded compile command for %s: %s", for_file, args)
        else:
            logger.debug("No compile command found for %s", for_file)


def extract_cpp_from_ir(kairo_path: str, compile_db: 'CompileCommands', file: str, line_range: str | None = None) -> str:
    """
    Run the Kairo compiler to emit IR for the given file, trim boilerplate,
    extract the mapped C++ for the specified line range, and return formatted output.
    Respects compile_commands.json from the running LSP server.
    """
    logger = logging.getLogger("KairoLSP")

    if not os.path.exists(kairo_path):
        raise FileNotFoundError(f"Kairo binary not found: {kairo_path}")

    compile_db.load(file)
    cmd = [kairo_path, file, "--emit-ir", "--verbose"]

    if compile_db.commands:
        cmd.extend(compile_db.commands)

    logger.info(f"Running Kairo IR emission for {file}")
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
    output = proc.stdout

    if not output.strip():
        raise RuntimeError("No output received from Kairo compiler")

    # --- Trim preamble ---
    hdr_pat = re.compile(r"#define __KAIRO_CORE_CXX__.*?#endif", re.DOTALL)
    m = hdr_pat.search(output)
    if m:
        output = output[m.end():]

    # Trim everything after the last #endif (Kairo emits multiple files)
    last_endif = output.rfind("#endif")
    if last_endif != -1:
        output = output[:last_endif + len("#endif")]

    # --- Remove ANSI color codes ---
    ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
    output = ansi_escape.sub('', output)

    # --- Build mapping: file → [lines] ---
    file_lines: dict[str, list[str]] = {}
    current_file = None
    current_macro = None
    current_line = None
    in_guard_section = False

    for raw in output.splitlines():
        line = raw.strip()
        if not line:
            continue

        # detect new file section
        if line.startswith("#define") and len(line.split()) == 3:
            _, macro, path_token = line.split(maxsplit=2)
            current_file = path_token.strip('"')
            current_macro = macro
            current_line = None
            in_guard_section = False
            file_lines.setdefault(current_file, [])
            continue

        if line.startswith("#line") and current_file:
            parts = line.split()
            try:
                ln = int(parts[1])
            except Exception:
                continue

            # second #line 1 marks the start of real code
            if len(parts) == 3 and parts[2] == current_macro:
                if in_guard_section:
                    file_lines[current_file] = []
                in_guard_section = True
                current_line = ln - 1
            elif len(parts) == 2:
                current_line = ln - 1
            continue

        if line.startswith("#"):
            continue

        if current_file is None or current_line is None:
            continue

        lines = file_lines[current_file]
        while len(lines) <= current_line:
            lines.append("")
        if lines[current_line]:
            lines[current_line] += "\n" + raw
        else:
            lines[current_line] = raw
        current_line += 1

    # --- Match file ---
    found_file = next((p for p in file_lines if p.endswith(file)), None)
    if not found_file:
        raise RuntimeError(f"No matching file section found for {file}")

    lines = file_lines[found_file]

    # --- Parse range ---
    if not line_range:
        start_line, end_line = 1, len(lines)
    elif ":" in line_range:
        start_line, end_line = map(int, line_range.split(":"))
    elif "-" in line_range:
        start_line, end_line = map(int, line_range.split("-"))
    else:
        start_line = end_line = int(line_range)

    start_line = max(1, start_line)
    end_line = max(start_line, end_line)

    if start_line > len(lines):
        raise IndexError(f"Start line {start_line} beyond file end ({len(lines)}).")

    isolated = lines[start_line - 1:end_line]
    isolated_code = "\n".join(l for l in isolated if l.strip())
    if not isolated_code.strip():
        raise RuntimeError(f"No content found in range {start_line}-{end_line}.")

    # --- clang-format ---
    with tempfile.NamedTemporaryFile(delete=False, suffix=".cpp", mode="w") as tmp:
        tmp.write(isolated_code)
        tmp_path = tmp.name

    try:
        fmt = subprocess.run(["clang-format", "-style=file", tmp_path],
                             capture_output=True, text=True)
        return fmt.stdout if fmt.returncode == 0 else isolated_code
    finally:
        if os.path.exists(tmp_path):
            os.remove(tmp_path)

# ---------------------------------------------------------------------- #
# Debounce
# ---------------------------------------------------------------------- #
class Debouncer:
    """Coalesce repeated per-key work onto a trailing idle timer.

    The editor autosaves every 100ms, and both save-triggered paths -- the
    index refresh and the stage 0 diagnostics run -- cost far more than 100ms.
    Without coalescing the server spawns processes faster than they finish and
    falls permanently behind, which is what made the editor feel bad.

    Trailing, not leading: the last save in a burst is the one whose content
    the user actually wants analysed.  Each new call for a key cancels the
    pending timer for that key, so a continuous burst does no work at all until
    it stops.  At most one job per key is ever in flight; a save that lands
    mid-job re-arms the timer so the newer content is picked up afterwards.
    """

    def __init__(self, delay: float) -> None:
        self.delay = delay
        self._lock = threading.Lock()
        self._timers: Dict[str, threading.Timer] = {}
        self._running: set[str] = set()
        self._again: set[str] = set()

    def schedule(self, key: str, fn: Callable[[], None]) -> None:
        with self._lock:
            if key in self._running:
                # Job in flight; remember that its input is already stale.
                self._again.add(key)
                return
            timer_ = self._timers.pop(key, None)
            if timer_ is not None:
                timer_.cancel()
            t = threading.Timer(self.delay, self._fire, args=(key, fn))
            t.daemon = True
            self._timers[key] = t
            t.start()

    def _fire(self, key: str, fn: Callable[[], None]) -> None:
        with self._lock:
            self._timers.pop(key, None)
            self._running.add(key)
        try:
            fn()
        except Exception:
            logger.exception("debounced job for %s failed", key)
        finally:
            with self._lock:
                self._running.discard(key)
                rerun = key in self._again
                self._again.discard(key)
            if rerun:
                # Content changed while we were working -- go again.
                self.schedule(key, fn)

    def cancel(self, key: str) -> None:
        with self._lock:
            t = self._timers.pop(key, None)
            self._again.discard(key)
        if t is not None:
            t.cancel()


#: Idle window before save-triggered work runs.  Long enough to swallow a burst
#: of 100ms autosaves, short enough that a deliberate pause feels responsive.
DEBOUNCE_DELAY = 0.4


# ---------------------------------------------------------------------- #
# KairoLanguageServer
# ---------------------------------------------------------------------- #
class KairoLanguageServer(LanguageServer):
    """Custom Language Server for Kairo."""

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        super().__init__(*args, **kwargs)
        self.diagnostics: Dict[str, List[Diagnostic]] = {}
        self.last_request_t = time.time()
        self.parse_interval = None
        self.analyze_failed = False
        self.basic_parse_failed = False
        self.kairo_path = sys.argv[1] if len(sys.argv) > 1 else None
        self.compile_db = None
        # Diagnostics run stage 0 over the full import closure and routinely hit
        # their timeout; at a 100ms autosave they must be coalesced or they
        # queue up faster than they drain.
        self.diag_debounce = Debouncer(DEBOUNCE_DELAY)

    @property
    def server_capabilities(self):
        caps = super().server_capabilities
        caps.text_document_sync = {
            "openClose": True,
            "change": None,
            "willSave": False,
            "willSaveWaitUntil": False,
            "save": True,
        }
        return caps

    # ------------------------------------------------------------------ #
    def _parse_with_analyze(self, document: TextDocumentItem) -> None:
        with timer() as elapsed:
            self.parse(document, analyze=True)
        self.parse_interval = elapsed()

    def queue_parse(self, document: TextDocumentItem) -> None:
        self.analyze_failed = not self.parse(document, analyze=True)
        self.last_request_t = time.time()
        return

    # ------------------------------------------------------------------ #
    def parse(self, document: TextDocumentItem, analyze: bool = False) -> bool:
        if self.compile_db is None:
            try:
                self.compile_db = CompileCommands(self)
            except Exception as e:
                logger.warning("Lazy re-init of CompileCommands failed: %s", e)

        diagnostics = []
        try:
            uri_path = urlparse(document.uri).path
            decoded_path = unquote(uri_path)
            file_path = os.path.abspath(decoded_path)

            if not self.kairo_path or not os.path.exists(self.kairo_path):
                logger.critical("Kairo binary not found: %s", self.kairo_path)
                raise FileNotFoundError(f"Kairo binary does not exist: {self.kairo_path}")

            command = [self.kairo_path, file_path, "--lsp-mode"]

            if analyze:
                command.append("--emit-ir")

            # Use persistent cached compile_commands
            self.compile_db.load(file_path)
            if self.compile_db.commands:
                command.extend(self.compile_db.commands)


            env = os.environ.copy()

            # stage 0 resolves relative -I paths against PWD, not the process
            # cwd, and an editor-hosted server inherits PWD=/.
            env["PWD"] = self.compile_db.directory

            logger.info(f"Running command: {' '.join(command)} in {self.compile_db.directory}")
            process = subprocess.Popen(
                command,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                cwd=self.compile_db.directory,
                env=env,
            )
            try:
                stdout, stderr = process.communicate(timeout=10)  # 10 second timeout
            except subprocess.TimeoutExpired:
                process.kill()
                stdout, stderr = process.communicate()
                logger.error("Kairo compiler timed out after 10 seconds: %s", command)

            if stderr:
                logger.error("Kairo stderr: %s", stderr.decode("utf-8"))

            if process.returncode == 0:
                self.diagnostics[document.uri] = (document.version, [])
                return True

            result = stdout.decode("utf-8").strip()
            result = self._remove_ansi_colors(result)

            if not result and process.returncode != 0:
                self.diagnostics[document.uri] = (document.version, diagnostics)
                return False

            try:
                json_result = json.loads(result)
                diagnostics = self._convert_to_diagnostics(json_result, file_path)
                self.diagnostics[document.uri] = (document.version, diagnostics)
            
            except json.JSONDecodeError as e:
                logger.error("Failed to parse JSON from Kairo output: %s\nOutput: %s", e, result)
                self.diagnostics[document.uri] = (document.version, diagnostics)
                return False

        except Exception as e:
            traceback.print_exc()
            logger.error("Error parsing document %s: %s", document.uri, e)

        if not analyze:
            return not any(d.severity == DiagnosticSeverity.Error for d in diagnostics)
        return not bool(diagnostics)

    # ------------------------------------------------------------------ #
    @staticmethod
    def _remove_ansi_colors(text: str) -> str:
        ansi_escape_pattern = re.compile(r"(?:\x1b|\033|\u001b|\001b)(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")
        return ansi_escape_pattern.sub("", text)

    @staticmethod
    def _convert_to_diagnostics(json_result: dict, file_path: str) -> List[Diagnostic]:
        diagnostics = []
        for error in json_result.get("error", {}).get("errors", []):
            severity = {
                "error": DiagnosticSeverity.Error,
                "note": DiagnosticSeverity.Information,
                "warn": DiagnosticSeverity.Warning,
                "fatal": DiagnosticSeverity.Error,
            }.get(str(error["level"]).strip(), DiagnosticSeverity.Information)

            if not compare_paths(error["file"].replace("\\\\", "\\"), file_path):
                continue

            diagnostics.append(
                Diagnostic(
                    message=error["msg"],
                    severity=severity,
                    range=Range(
                        start=Position(line=int(error["line"]) - 1, character=int(error["col"])),
                        end=Position(
                            line=int(error["line"]) - 1,
                            character=int(error["col"]) + int(error["offset"]),
                        ),
                    ),
                )
            )
        return diagnostics


# ---------------------------------------------------------------------- #
# Symbol index
# ---------------------------------------------------------------------- #
SYMBOL_KINDS = {
    "class": SymbolKind.Class,
    "struct": SymbolKind.Struct,
    "enum": SymbolKind.Enum,
    "enum_member": SymbolKind.EnumMember,
    "module": SymbolKind.Module,
    "func": SymbolKind.Function,
    "field": SymbolKind.Field,
    "const": SymbolKind.Constant,
    "typealias": SymbolKind.TypeParameter,
    "ffi": SymbolKind.Function,
}

_WORD = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


class IndexManager:
    """Owns the symbol index and keeps it off the request thread.

    Indexing a file also indexes its entire import closure -- one stage 0 run
    over `Compiler/Driver/Kairo.k` yields all 144 files of stage 1 -- so the
    first opened file usually populates everything, and later opens are free.

    Queries never block on a build in progress: an incomplete index answers
    with what it has rather than stalling the editor.
    """

    def __init__(self, server: "KairoLanguageServer") -> None:
        self.server = server
        self.index = ki.Index()
        self._lock = threading.Lock()
        self._pending: set[str] = set()
        self._debounce = Debouncer(DEBOUNCE_DELAY)
        self.cache: Optional[ki.IndexCache] = None
        self.ready = threading.Event()

    # ------------------------------------------------------------------ #
    def bootstrap(self, root: str) -> None:
        """Index the whole workspace up front, in the background.

        Lazy per-file indexing meant the first jump into an unindexed file
        always missed -- the request arrived before the file had ever been
        parsed, so it returned nothing and only the *second* attempt worked.
        Indexing everything at startup removes that entirely.

        This is affordable only because of ``--index-file``: at a median 3 ms
        per file, ~500 files is a couple of seconds across a thread pool, where
        closure walks would have been minutes.  Subsequent starts are near
        instant, served from ``.cache/kairo-index/``.
        """
        threading.Thread(target=self._bootstrap, args=(root,), daemon=True).start()

    def _supports_progress(self) -> bool:
        """Whether the client accepts server-initiated work-done progress.

        Creating progress against a client that never advertised it makes the
        `window/workDoneProgress/create` request fail, so this is checked
        rather than assumed.  vscode-languageclient sets it by default.
        """
        try:
            return bool(self.server.client_capabilities.window.work_done_progress)
        except AttributeError:
            return False

    def _bootstrap(self, root: str) -> None:
        token = f"kairo-index-{os.getpid()}"
        progress = None

        if self._supports_progress():
            try:
                # Sync variant: this runs on a worker thread, not the event
                # loop.  Safe with the stdio transport -- StdoutWriter.write is
                # synchronous, so nothing is scheduled onto the loop, and
                # _send_data emits header+body in a single write, so it cannot
                # interleave with the loop's own writes.
                self.server.work_done_progress.create(token).result(timeout=5)
                self.server.work_done_progress.begin(token, WorkDoneProgressBegin(
                    title="Kairo", message="indexing workspace", percentage=0,
                ))

                def progress(done: int, total: int) -> None:
                    self.server.work_done_progress.report(token, WorkDoneProgressReport(
                        message=f"indexing {done}/{total}",
                        percentage=int(done * 100 / total) if total else 0,
                    ))
            except Exception:
                # Progress is decoration; never let it stop the index.
                logger.warning("work-done progress unavailable", exc_info=True)
                progress = None

        try:
            cache = ki.IndexCache(root)
            with timer() as elapsed:
                index, parsed, cached = ki.build_index_eager(
                    self.server.kairo_path, root, cache=cache, cwd=root,
                    progress=progress,
                )
            decls = sum(len(v) for v in index.decls_by_file.values())
            if decls == 0:
                # Refuse to install an empty index, unconditionally.  An index
                # with no declarations cannot answer anything, so keeping the
                # lazy path is never worse -- and the failure case reports
                # parsed == cached == 0, so this must not be predicated on
                # those counts.
                logger.error(
                    "eager index yielded 0 declarations -- discarding it and "
                    "staying on lazy per-file indexing",
                )
                return

            # Swap wholesale: readers hold no lock, and rebinding the attribute
            # is atomic, so nobody ever observes a partially-built index.
            with self._lock:
                self.index = index
                self.cache = cache
            logger.info(
                "workspace indexed in %.1fs (%d parsed, %d from cache): %s",
                elapsed(), parsed, cached, index.stats(),
            )
        except Exception:
            logger.exception("eager workspace index failed; "
                             "falling back to lazy per-file indexing")
        finally:
            if progress is not None:
                try:
                    n = len(self.index.seen_files)
                    self.server.work_done_progress.end(token, WorkDoneProgressEnd(
                        message=f"indexed {n} files",
                    ))
                except Exception:
                    logger.warning("could not end progress", exc_info=True)
            self.ready.set()

    # ------------------------------------------------------------------ #
    def ensure(self, path: str) -> None:
        """Index ``path``'s closure in the background if it is not covered."""
        path = os.path.realpath(path)
        with self._lock:
            if path in self.index.seen_files or path in self._pending:
                return
            self._pending.add(path)
        threading.Thread(target=self._build, args=(path,), daemon=True).start()

    def invalidate(self, path: str) -> None:
        """Re-index a single file on save, debounced.

        This is the warm path and it does NOT walk the import closure: stage 0's
        ``--index-file`` parses just this file, and cross-file linking already
        happens here by name.  That is the difference between ~9s and ~150ms on
        stage 1's largest files, which is what makes refresh-on-save viable at
        a 100ms autosave interval at all.

        The stale entries are deliberately NOT dropped up front.  Readers query
        without a lock, so dropping first would leave a window where every
        symbol declared in this file does not exist -- reading as
        "go-to-definition stopped working" for every file pointing into it.
        The swap happens in _refresh once the new data is actually in hand.
        """
        path = os.path.realpath(path)
        self._debounce.schedule(path, lambda: self._refresh(path))

    def cancel(self, path: str) -> None:
        """Drop a queued refresh; an in-flight one is left to finish."""
        self._debounce.cancel(os.path.realpath(path))

    # ------------------------------------------------------------------ #
    def _refresh(self, path: str) -> None:
        """Single-file reparse, staged then swapped in."""
        with self._lock:
            if path in self._pending:
                return  # a cold closure build is already covering this file
            self._pending.add(path)
        try:
            cwd = None
            db = self.server.compile_db
            if db is not None:
                # --index-file needs no -I flags, but cwd/PWD still matter.
                db.load(path)
                cwd = db.directory

            with timer() as elapsed:
                docs = ki.run_index_file(self.server.kairo_path, path, cwd)
                if not docs:
                    logger.warning("single-file reparse of %s yielded nothing; "
                                   "keeping previous entries", path)
                    return

                # Stage into a throwaway index, then swap as pure dict writes so
                # the drop and the refill are adjacent.
                staged = ki.Index()
                ki.fold_documents(staged, docs)
                with self._lock:
                    self.index.absorb(staged, path)
                    cache = self.cache
                if cache is not None:
                    # Write through, so the next cold start does not re-parse
                    # a file that is already current.
                    cache.store(self.index, path)
            logger.info("reindexed %s in %.0fms (%s)",
                        path, elapsed() * 1000, self.index.stats())
        except Exception:
            logger.exception("single-file reindex of %s failed", path)
        finally:
            with self._lock:
                self._pending.discard(path)

    # ------------------------------------------------------------------ #
    def _build(self, path: str) -> None:
        """Cold path: one closure walk to populate the index in bulk.

        Only runs on first sight of a file.  Refresh-on-save goes through
        _refresh instead, which is ~60x cheaper.
        """
        try:
            # `commands` is populated by load(), not by construction -- reading
            # it cold yields [], which means no -I flags, which means stage 0
            # resolves no imports and only the prelude gets indexed.
            args: List[str] = []
            cwd = None
            db = self.server.compile_db
            if db is not None:
                db.load(path)
                args = list(db.commands or [])
                cwd = db.directory

            with timer() as elapsed:
                # Slow part, deliberately outside the lock: the old index stays
                # fully queryable while stage 0 runs.
                docs = ki.run_emit_ast(self.server.kairo_path, path, args, cwd)
                folded = ki.fold_documents(self.index, docs, self._lock)
            logger.info(
                "indexed %s: +%d files in %.1fs (%s)",
                path, folded, elapsed(), self.index.stats(),
            )
        except Exception:
            logger.exception("indexing %s failed", path)
        finally:
            with self._lock:
                self._pending.discard(path)


def _uri_to_path(uri: str) -> str:
    return os.path.realpath(unquote(urlparse(uri).path))


def _path_to_uri(path: str) -> str:
    return pathlib.Path(path).as_uri()


def _decl_location(decl: ki.Decl) -> Location:
    # Stage 0's two location fields do NOT share a base.  `line_number` is
    # 1-based and LSP lines are 0-based, so line converts.  `column_number` is
    # already 0-based -- verified against abi.k:15, where col 8 lands exactly on
    # the `M` of `Module` -- and LSP characters are 0-based too, so col passes
    # through untouched.  Subtracting from it put every jump one column early.
    line = max(decl.line - 1, 0)
    col = max(decl.col, 0)
    return Location(
        uri=_path_to_uri(decl.file),
        range=Range(
            start=Position(line=line, character=col),
            end=Position(line=line, character=col + max(decl.length, 1)),
        ),
    )


def _word_at(server: "KairoLanguageServer", uri: str, line: int, col: int) -> str:
    """The identifier under the cursor, for non-member-access positions."""
    # This is only a fallback for non-member-access positions, so it must never
    # be able to fail the request that called it.
    try:
        text = server.workspace.get_text_document(uri).lines[line]
    except Exception:
        return ""
    for m in _WORD.finditer(text):
        if m.start() <= col <= m.end():
            return m.group(0)
    return ""


# ---------------------------------------------------------------------- #
# LSP Feature Registration
# ---------------------------------------------------------------------- #
SERVER = KairoLanguageServer("KairoLSP", "1.0")
INDEX = IndexManager(SERVER)


@SERVER.feature(INITIALIZED)
def on_initialized(server: KairoLanguageServer, params: Any) -> None:
    logger.info("Kairo Language Server initialized.")
    root = None
    try:
        server.compile_db = CompileCommands(server)
        root = getattr(server.compile_db, "directory", None)
        logger.info("CompileCommands initialized successfully.")
    except Exception as e:
        logger.error("Failed to initialize CompileCommands: %s", e)

    if root:
        INDEX.bootstrap(root)
    else:
        logger.warning("no workspace root; indexing stays lazy per-file")


@SERVER.feature(TEXT_DOCUMENT_DID_OPEN)
def did_open(server: KairoLanguageServer, params: DidOpenTextDocumentParams) -> None:
    doc = server.workspace.get_text_document(params.text_document.uri)
    INDEX.ensure(_uri_to_path(params.text_document.uri))
    server.queue_parse(doc)
    send_diagnostics(server, params.text_document.uri)


@SERVER.feature(TEXT_DOCUMENT_DID_CLOSE)
def did_close(server: KairoLanguageServer, params: DidCloseTextDocumentParams) -> None:
    # Drop any debounced work still queued for a file nobody is looking at.
    path = _uri_to_path(params.text_document.uri)
    server.diag_debounce.cancel(path)
    INDEX.cancel(path)

    server.diagnostics.pop(params.text_document.uri, None)
    send_diagnostics(server, params.text_document.uri)


@SERVER.feature(TEXT_DOCUMENT_DID_SAVE)
def did_save(server: KairoLanguageServer, params: DidChangeTextDocumentParams) -> None:
    """Handles document saving.

    Both halves are debounced.  The editor autosaves every 100ms while typing,
    and neither the index refresh nor the diagnostics run finishes anywhere
    near that fast -- running them per-save means never finishing one before
    the next arrives.
    """
    uri = getattr(params.text_document, "uri", None)
    if not uri:
        logger.warning("DidSave event missing URI: %s", params)
        return

    path = _uri_to_path(uri)
    INDEX.invalidate(path)

    def _diagnose() -> None:
        # Re-fetch inside the timer: by the time this fires the document has
        # almost certainly moved on from the version that scheduled it.
        try:
            doc = server.workspace.get_text_document(uri)
        except Exception:
            logger.warning("document %s vanished before diagnostics ran", uri)
            return
        server.queue_parse(doc)
        send_diagnostics(server, uri)

    server.diag_debounce.schedule(path, _diagnose)

@SERVER.feature(TEXT_DOCUMENT_DEFINITION)
@safe_handler
def goto_definition(
    server: KairoLanguageServer, params: DefinitionParams
) -> List[Location]:
    """Go-to-definition, returning a ranked candidate *set*.

    stage 0 has no symbol table, so a single answer is not always available.
    Returning every plausible declaration best-first lets the editor show a
    picker instead of silently jumping to the wrong overload -- precision
    degrades, correctness does not.  See kairo_resolve for the tiers.
    """
    path = _uri_to_path(params.text_document.uri)
    INDEX.ensure(path)

    line = params.position.line + 1  # stage 0 emits 1-based lines
    col = params.position.character

    res = kr.resolve(
        INDEX.index,
        path,
        line,
        col,
        name=_word_at(server, params.text_document.uri, params.position.line, col),
    )

    if not res.candidates:
        logger.info("definition miss at %s:%d:%d (%s)", path, line, col, res.note)
        return []

    logger.info(
        "definition %s -> %s (%d candidate(s))",
        res.describe(), res.candidates[0].decl.qualified, len(res.candidates),
    )
    return [_decl_location(c.decl) for c in res.candidates[:32]]


@SERVER.feature(TEXT_DOCUMENT_DOCUMENT_SYMBOL)
@safe_handler
def document_symbol(
    server: KairoLanguageServer, params: DocumentSymbolParams
) -> List[SymbolInformation]:
    """The outline stage 0's LSP never provided."""
    path = _uri_to_path(params.text_document.uri)
    INDEX.ensure(path)

    out: List[SymbolInformation] = []
    for decl in INDEX.index.decls_by_file.get(path, []):
        out.append(
            SymbolInformation(
                name=decl.name,
                kind=SYMBOL_KINDS.get(decl.kind, SymbolKind.Variable),
                location=_decl_location(decl),
                container_name=decl.owner,
            )
        )
    out.sort(key=lambda s: (s.location.range.start.line, s.name))
    return out


@SERVER.feature(WORKSPACE_SYMBOL)
@safe_handler
def workspace_symbol(
    server: KairoLanguageServer, params: WorkspaceSymbolParams
) -> List[SymbolInformation]:
    query = (params.query or "").lower()
    if not query:
        return []

    out: List[SymbolInformation] = []
    for name, decls in INDEX.index.decls_by_name.items():
        if query not in name.lower():
            continue
        for decl in decls:
            out.append(
                SymbolInformation(
                    name=decl.name,
                    kind=SYMBOL_KINDS.get(decl.kind, SymbolKind.Variable),
                    location=_decl_location(decl),
                    container_name=decl.owner,
                )
            )
            if len(out) >= 512:  # editors choke well before this
                return out
    return out


@SERVER.command("kairo.showIR")
def show_ir(server: KairoLanguageServer, params: ExecuteCommandParams):
    """Handles actual IR generation when command is invoked."""
    logger.info(f"Received kairo.showIR for {params}")
    try:
        uri, start_line, end_line = params.arguments
        file = Path(url2pathname(unquote(urlparse(uri).path))).absolute()
        line_range = f"{start_line}:{end_line}"

        result = extract_cpp_from_ir(
            server.kairo_path, server.compile_db, str(file), line_range
        )
        return result
    except Exception as e:
        logger.exception("Show IR failed")
        return f"Error: {e}"

def send_diagnostics(server: KairoLanguageServer, uri: str) -> None:
    diag = server.diagnostics.get(uri)
    if not diag:
        return
    version, diagnostics = diag
    server.text_document_publish_diagnostics(
        PublishDiagnosticsParams(uri=uri, version=version, diagnostics=diagnostics)
    )
    server.diagnostics.clear()


# ---------------------------------------------------------------------- #
# Log maintenance
# ---------------------------------------------------------------------- #
class LogClearerThread(threading.Thread):
    def __init__(self, interval: int) -> None:
        super().__init__(daemon=True)
        self.interval = interval

    def run(self) -> None:
        while True:
            time.sleep(self.interval)
            clear_log_file()
            logger.info("Log file cleared.")


# ---------------------------------------------------------------------- #
# Entrypoint
# ---------------------------------------------------------------------- #
if __name__ == "__main__":
    logger.info("Starting Kairo Language Server")
    try:
        LogClearerThread(LOG_CLEAR_INTERVAL).start()
        SERVER.start_io()
    except Exception as e:
        logger.critical("Server encountered a fatal error: %s", e)
        with open(os.path.join(os.path.dirname(__file__), "error.log"), "a") as f:
            f.write(f"Fatal error: {e}\n")
# ---------------------------------------------------------------------- #