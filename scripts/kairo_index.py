"""Symbol index built from stage 0's ``--emit-ast`` output.

This is the *mechanical* half of go-to-definition:

    * run stage 0 over a root file, parse its NDJSON AST output
    * extract declarations, typed bindings, and member-access sites
    * dedupe by source file, so the prelude is walked once instead of once
      per invocation
    * keep flat tables and throw the AST away

Resolution -- deciding *which* ``bar`` a given ``foo.bar()`` means -- lives in
``kairo_resolve.py``.

AST node shapes this file depends on.  All verified against stage 0 output for
``Compiler/Driver/Kairo.k`` (144 files, 3427 FuncDecl), not guessed::

    ClassDecl   { name: IdentExpr, derives: UDTDeriveDecl, body: SuiteState }
    StructDecl  { name: IdentExpr, ... }
    UDTDeriveDecl [ Type, ... ]                 -- the `derives` mixin list
    FuncDecl    { name: PathExpr, params: [VarDecl], returns: Type,
                  is_op: int, op: [tok], body: SuiteState }
    TypeDecl    { name: IdentExpr, type: Type }  -- `type X = Y`
    VarDecl     { var: NamedVarSpecifier, value: ... }
    NamedVarSpecifier { path: IdentExpr, type: Type }
    Type        { value: IdentExpr | ScopePathExpr, generics: GenericInvokeExpr }
    GenericInvokeExpr [ Type, ... ]              -- the `::<...>` arguments
    PathExpr    { path: IdentExpr | ScopePathExpr, type: int }
    ScopePathExpr { access: IdentExpr, path: [IdentExpr] }   -- access is LAST
    IdentExpr   { value: str, length: int, loc: Loc }
    Loc         { filename, line_number, column_number, offset }

Two things that are easy to get wrong and are load-bearing here:

``a.b`` and ``a->b`` are *different node types*.  Only ``.`` produces a
``DotPathExpr``; ``->`` is a ``BinaryExpr`` whose ``op.kind`` is ``"->"``.
Stage 1 has 17385 of the former and 11955 of the latter, so handling one and
not the other loses ~40% of all member accesses.

``loc`` carries a start position only -- there is no end position anywhere in
the tree.  Declaration extents are derived by taking min/max ``line_number``
over a subtree (``_extent``), which is accurate for well-formed decls and
degrades gracefully for malformed ones.
"""

from __future__ import annotations

import hashlib
import json
import logging
import os
import re
import subprocess
import threading
from concurrent.futures import ThreadPoolExecutor
from dataclasses import asdict, dataclass, field
from functools import lru_cache
from typing import Any, Dict, Iterable, Iterator, List, Optional, Tuple

logger = logging.getLogger("KairoLSP.index")

# stage 0 writes the AST to stdout through its logger, so every document is
# prefixed and colourised.  Output is NDJSON: one complete JSON document per
# translation unit, followed by any diagnostics.
_ANSI = re.compile(r"\x1b(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")
_LOG_PREFIX = re.compile(r"^(?:debug|info|warn|error|fatal):\s*")

DECL_KINDS = {
    "ClassDecl": "class",
    "StructDecl": "struct",
    "EnumDecl": "enum",
    "EnumMemberDecl": "enum_member",
    "ModuleDecl": "module",
    "FuncDecl": "func",
    "FFIDecl": "ffi",
    "ConstDecl": "const",
    "TypeDecl": "typealias",
}

#: Declarations that open a named scope, so nested members get owner-qualified.
SCOPE_KINDS = ("ClassDecl", "StructDecl", "EnumDecl", "ModuleDecl")

#: Declarations that can carry a ``derives`` list.
DERIVING_KINDS = ("ClassDecl", "StructDecl", "EnumDecl")

# Receiver classifications recorded at an access site.
RECV_SELF = "self"  # self.foo() -- resolves against the enclosing type
RECV_FIELD = "field"  # self.bar->foo() -- resolves via a field of the enclosing type
RECV_NAME = "name"  # ident.foo() -- resolves via a binding lookup
RECV_CHAIN = "chain"  # anything else: call results, indexing, nested access


@dataclass(slots=True)
class Decl:
    """A single declaration, flattened out of the AST."""

    name: str
    kind: str
    file: str
    line: int  # 1-based, exactly as stage 0 emits it
    col: int
    length: int
    owner: Optional[str] = None  # enclosing type/module, or qualified prefix
    arity: Optional[int] = None  # len(params) for FuncDecl, else None
    returns: Optional[str] = None  # return type, alias target, or field type
    type_args: List[str] = field(default_factory=list)  # generics on the above
    derives: List[str] = field(default_factory=list)
    is_op: bool = False
    start_line: int = 0
    end_line: int = 0

    @property
    def qualified(self) -> str:
        return f"{self.owner}::{self.name}" if self.owner else self.name

    @property
    def leaf_owner(self) -> Optional[str]:
        """The innermost segment of ``owner`` -- ``Foo`` from ``std::Bar::Foo``."""
        return self.owner.rsplit("::", 1)[-1] if self.owner else None


@dataclass(slots=True)
class Binding:
    """A name bound to a syntactically-known type.

    Produced from function parameters and from annotated ``let``/``var``
    declarations.  This is the raw material for answering "what is ``foo``"
    in ``foo.bar()`` without a symbol table.
    """

    name: str
    type_name: str
    type_args: List[str]  # generic arguments, e.g. obs::<ASTContext> -> [ASTContext]
    file: str
    line: int
    col: int
    scope_start: int
    scope_end: int

    def in_scope(self, line: int) -> bool:
        return self.scope_start <= line <= self.scope_end


@dataclass(slots=True)
class Access:
    """A member access site: the ``bar`` in ``foo.bar()`` or ``foo->bar()``."""

    name: str  # the member being accessed
    file: str
    line: int
    col: int
    length: int
    arrow: bool  # True for ``->``, False for ``.``
    recv_kind: str  # RECV_SELF | RECV_NAME | RECV_CHAIN
    recv_name: Optional[str]  # populated when recv_kind is RECV_NAME
    enclosing_owner: Optional[str]  # owner of the function this site sits in
    argc: Optional[int]  # argument count when this is a call, else None


@dataclass
class Index:
    decls_by_name: Dict[str, List[Decl]] = field(default_factory=dict)
    decls_by_file: Dict[str, List[Decl]] = field(default_factory=dict)
    #: type name -> its declaration(s); the spine of receiver resolution
    types_by_name: Dict[str, List[Decl]] = field(default_factory=dict)
    bindings_by_file: Dict[str, List[Binding]] = field(default_factory=dict)
    accesses_by_file: Dict[str, List[Access]] = field(default_factory=dict)
    #: `type X = Y` aliases, name -> target type name
    aliases: Dict[str, str] = field(default_factory=dict)
    #: files already folded in, so the prelude is not re-walked per invocation
    seen_files: set[str] = field(default_factory=set)

    def add_decl(self, d: Decl) -> None:
        self.decls_by_name.setdefault(d.name, []).append(d)
        self.decls_by_file.setdefault(d.file, []).append(d)
        if d.kind in ("class", "struct", "enum"):
            self.types_by_name.setdefault(d.name, []).append(d)
        if d.kind == "typealias" and d.returns:
            self.aliases.setdefault(d.name, d.returns)

    def add_binding(self, b: Binding) -> None:
        self.bindings_by_file.setdefault(b.file, []).append(b)

    def add_access(self, a: Access) -> None:
        self.accesses_by_file.setdefault(a.file, []).append(a)

    def drop_file(self, path: str) -> None:
        """Forget everything from ``path`` so it can be re-indexed on save."""
        for d in self.decls_by_file.pop(path, []):
            for table in (self.decls_by_name, self.types_by_name):
                bucket = table.get(d.name)
                if not bucket:
                    continue
                bucket[:] = [x for x in bucket if x.file != path]
                if not bucket:
                    del table[d.name]
        self.bindings_by_file.pop(path, None)
        self.accesses_by_file.pop(path, None)
        self.seen_files.discard(path)

    def absorb(self, other: "Index", path: str) -> None:
        """Replace everything from ``path`` with ``other``'s version of it.

        The caller stages ``other`` off to the side first, so the only work
        done here is dict manipulation -- microseconds.  That matters because
        readers query without taking a lock: a drop followed by a slow refold
        leaves a window where the file's symbols do not exist, and a reader
        landing in it sees go-to-definition fail for every symbol declared in
        that file.
        """
        self.drop_file(path)
        for decl in other.decls_by_file.get(path, []):
            self.add_decl(decl)
        for binding in other.bindings_by_file.get(path, []):
            self.add_binding(binding)
        for access in other.accesses_by_file.get(path, []):
            self.add_access(access)
        self.aliases.update(other.aliases)
        self.seen_files.add(path)

    def lookup(self, name: str) -> List[Decl]:
        return self.decls_by_name.get(name, [])

    def members_of(self, type_name: str) -> List[Decl]:
        """Every decl whose innermost owner segment is ``type_name``."""
        out: List[Decl] = []
        for decls in self.decls_by_file.values():
            out.extend(d for d in decls if d.leaf_owner == type_name)
        return out

    def stats(self) -> str:
        return (
            f"{len(self.seen_files)} files, "
            f"{sum(len(v) for v in self.decls_by_file.values())} decls, "
            f"{len(self.types_by_name)} types, "
            f"{sum(len(v) for v in self.bindings_by_file.values())} bindings, "
            f"{sum(len(v) for v in self.accesses_by_file.values())} accesses"
        )


# ---------------------------------------------------------------------- #
# Running stage 0
# ---------------------------------------------------------------------- #
def run_emit_ast(
    kairo_bin: str,
    path: str,
    extra_args: Optional[List[str]] = None,
    cwd: Optional[str] = None,
    timeout: int = 60,
) -> List[dict]:
    """Return every AST document stage 0 emits for ``path``.

    One invocation yields the target file *and* everything it imports, so a
    single call can populate a large slice of the index.  Indexing
    ``Compiler/Driver/Kairo.k`` yields all 144 files of stage 1 in one go.

    stage 0 emits the AST before it aborts on semantic errors, so this returns
    useful trees for files that do not compile -- the normal case while
    stage 1's syntax is mid-migration.
    """
    cmd = [kairo_bin, path, "--emit-ast", "--lsp-mode"]
    if extra_args:
        cmd.extend(extra_args)

    # stage 0 resolves relative -I paths against PWD, not the process cwd, and
    # an editor-hosted server inherits PWD=/ from the extension host.  Without
    # this every include misses and only the prelude gets indexed.
    env = os.environ.copy()
    if cwd:
        env["PWD"] = cwd

    try:
        proc = subprocess.run(
            cmd, capture_output=True, cwd=cwd, env=env, timeout=timeout
        )
    except subprocess.TimeoutExpired:
        logger.error("stage 0 timed out indexing %s", path)
        return []
    except OSError as e:
        logger.error("failed to launch stage 0 (%s): %s", kairo_bin, e)
        return []

    return _parse_ast_stream(proc.stdout, path)


def _parse_ast_stream(stdout: bytes, path: str) -> List[dict]:
    """Decode stage 0's NDJSON AST stream.

    One document per translation unit, each prefixed with ``debug: `` and ANSI
    colour codes.  ``json.load`` over the whole stream fails with "Extra data".
    """
    docs: List[dict] = []
    for raw in stdout.decode("utf-8", errors="replace").splitlines():
        line = _LOG_PREFIX.sub("", _ANSI.sub("", raw)).strip()
        if not line.startswith("{"):
            continue  # diagnostics and other chatter share this stream
        try:
            docs.append(json.loads(line))
        except json.JSONDecodeError as e:
            logger.warning("undecodable AST document from %s: %s", path, e)
    return docs


def run_index_file(
    kairo_bin: str,
    path: str,
    cwd: Optional[str] = None,
    timeout: int = 20,
) -> List[dict]:
    """Return the AST for ``path`` alone, without walking its import closure.

    This is the warm-refresh counterpart to :func:`run_emit_ast`.  Stage 0's
    ``--index-file`` lexes and parses exactly one file: no import expansion, no
    codegen, no linking.  Cross-file linking happens in this index by name, so
    the closure is dead weight when only one file changed.

    Measured against stage 1: ``Compiler/Parser/ASTParse.k`` 151ms vs 3.29s for
    the closure walk, yielding 5833 of 5883 nodes.  The handful of missing
    nodes are the import statements themselves, which are stripped from the
    token stream and carry nothing the index wants.

    No ``-I`` flags are passed, and none are needed -- nothing is resolved.
    """
    cmd = [kairo_bin, path, "--index-file"]

    # Kept for parity with run_emit_ast: --index-file resolves no includes, but
    # stage 0 still reads PWD, and an editor-hosted server inherits PWD=/.
    env = os.environ.copy()
    if cwd:
        env["PWD"] = cwd

    try:
        proc = subprocess.run(
            cmd, capture_output=True, cwd=cwd, env=env, timeout=timeout
        )
    except subprocess.TimeoutExpired:
        logger.error("stage 0 --index-file timed out on %s", path)
        return []
    except OSError as e:
        logger.error("failed to launch stage 0 (%s): %s", kairo_bin, e)
        return []

    return _parse_ast_stream(proc.stdout, path)


def load_ast_dump(path: str) -> List[dict]:
    """Read AST documents from a saved ``--emit-ast`` dump."""
    docs: List[dict] = []
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = _LOG_PREFIX.sub("", _ANSI.sub("", raw)).strip()
            if not line.startswith("{"):
                continue
            try:
                docs.append(json.loads(line))
            except json.JSONDecodeError as e:
                logger.warning("undecodable AST document in %s: %s", path, e)
    return docs


# ---------------------------------------------------------------------- #
# Tree helpers
# ---------------------------------------------------------------------- #
@lru_cache(maxsize=8192)
def _norm(path: str) -> str:
    """Canonicalise a stage 0 filename.

    Index tables are keyed by file, and the LSP layer arrives with a URI that
    has been through ``realpath``.  Stage 0 emits whatever path it was handed,
    so without normalising here every lookup silently misses on any workspace
    reached through a symlink.
    """
    if not path or path == "?":
        return "?"
    return os.path.realpath(path)


def _walk(node: Any) -> Iterator[Tuple[str, Any]]:
    """Yield every ``(node_kind, payload)`` pair in the tree, depth first."""
    stack = [node]
    while stack:
        cur = stack.pop()
        if isinstance(cur, dict):
            for key, value in cur.items():
                if key and key[0].isupper():
                    yield key, value
                stack.append(value)
        elif isinstance(cur, list):
            stack.extend(cur)


def _locs(node: Any) -> Iterator[dict]:
    stack = [node]
    while stack:
        cur = stack.pop()
        if isinstance(cur, dict):
            for key, value in cur.items():
                if key == "loc" and isinstance(value, dict) and "line_number" in value:
                    yield value
                else:
                    stack.append(value)
        elif isinstance(cur, list):
            stack.extend(cur)


def _extent(node: Any) -> Tuple[int, int]:
    """Approximate a subtree's line range (min/max over descendant locs)."""
    lines = [loc["line_number"] for loc in _locs(node)]
    return (min(lines), max(lines)) if lines else (0, 0)


def _first_ident(node: Any) -> Optional[dict]:
    for kind, payload in _walk(node):
        if kind == "IdentExpr" and isinstance(payload, dict):
            return payload
    return None


def flatten_path(node: Any) -> List[dict]:
    """Flatten a ``PathExpr`` / ``ScopePathExpr`` into its ``IdentExpr`` parts.

    ``ScopePathExpr`` keeps the *final* segment in ``access`` and the prefix in
    ``path``, so ``std::Error::Foo`` arrives as ``path=[std, Error]`` plus
    ``access=Foo``.  Returned in source order; the last element is the leaf.
    """
    if not isinstance(node, dict):
        return []

    if "PathExpr" in node:
        return flatten_path(node["PathExpr"].get("path"))

    if "ScopePathExpr" in node:
        scope = node["ScopePathExpr"]
        parts: List[dict] = []
        for seg in scope.get("path") or []:
            if isinstance(seg, dict) and "IdentExpr" in seg:
                parts.append(seg["IdentExpr"])
        access = scope.get("access")
        if isinstance(access, dict) and "IdentExpr" in access:
            parts.append(access["IdentExpr"])
        return parts

    if "IdentExpr" in node:
        return [node["IdentExpr"]]

    ident = _first_ident(node)
    return [ident] if ident else []


def _type_parts(node: Any) -> Tuple[Optional[str], List[str]]:
    """``(base_name, generic_args)`` from a ``Type`` node.

    ``obs::<ASTContext>`` gives ``("obs", ["ASTContext"])``.  The wrapper is
    kept rather than unwrapped here -- deciding that ``obs::<T>`` dereferences
    to ``T`` is a resolution policy, not an indexing fact.
    """
    if not isinstance(node, dict):
        return None, []
    ty = node.get("Type") if "Type" in node else node
    if not isinstance(ty, dict):
        return None, []

    base = None
    value = ty.get("value")
    if isinstance(value, dict):
        parts = flatten_path(value)
        if parts:
            base = parts[-1].get("value")

    args: List[str] = []
    generics = ty.get("generics")
    if isinstance(generics, dict):
        for entry in generics.get("GenericInvokeExpr") or []:
            arg, _ = _type_parts(entry)
            if arg:
                args.append(arg)

    return base, args


def _named_var(node: Any) -> Optional[Tuple[dict, Optional[str], List[str]]]:
    """``(IdentExpr, type_name, type_args)`` from a ``VarDecl``/``NamedVarSpecifier``."""
    if not isinstance(node, dict):
        return None
    spec = None
    if "NamedVarSpecifier" in node:
        spec = node["NamedVarSpecifier"]
    else:
        for key in ("VarDecl", "LetDecl", "ConstDecl"):
            inner = node.get(key)
            if isinstance(inner, dict):
                var = inner.get("var")
                if isinstance(var, dict) and "NamedVarSpecifier" in var:
                    spec = var["NamedVarSpecifier"]
                    break
    if not isinstance(spec, dict):
        return None

    path = spec.get("path")
    ident = path.get("IdentExpr") if isinstance(path, dict) else None
    if not isinstance(ident, dict):
        ident = _first_ident(path)
    if not isinstance(ident, dict):
        return None

    base, args = _type_parts(spec.get("type"))
    return ident, base, args


def _derives_of(payload: dict) -> List[str]:
    """Mixin/base type names from a ``derives`` clause.

    ``derives ExprParse::<ASTParse>, StmtParse::<ASTParse>`` yields
    ``["ExprParse", "StmtParse"]`` -- the CRTP argument is dropped, since what
    matters for member lookup is which mixin body to search.
    """
    out: List[str] = []
    derives = payload.get("derives")
    if not isinstance(derives, dict):
        return out
    for entry in derives.get("UDTDeriveDecl") or []:
        base, _ = _type_parts(entry)
        if base:
            out.append(base)
    return out


# ---------------------------------------------------------------------- #
# Extraction
# ---------------------------------------------------------------------- #
class Extractor:
    """Folds one AST document into an :class:`Index`.

    Carries two pieces of context down the tree: ``owner`` (the enclosing
    named scope, for qualifying declarations) and ``func`` (the enclosing
    function's owner and line extent, needed to scope local bindings and to
    give ``self`` at an access site a type).
    """

    def __init__(self, index: Index) -> None:
        self.index = index

    def feed(self, doc: dict) -> None:
        program = doc.get("ast", {}).get("Program")
        if program is not None:
            self._visit(program, None, None, (0, 0))

    # ------------------------------------------------------------------ #
    def _visit(
        self,
        node: Any,
        owner: Optional[str],
        func_owner: Optional[str],
        func_span: Tuple[int, int],
    ) -> None:
        if isinstance(node, list):
            for item in node:
                self._visit(item, owner, func_owner, func_span)
            return
        if not isinstance(node, dict):
            return

        for kind, payload in node.items():
            if kind in DECL_KINDS:
                self._decl(kind, payload, owner, func_owner, func_span)
            elif kind == "DotPathExpr":
                self._access(payload, arrow=False, func_owner=func_owner)
                self._visit(payload, owner, func_owner, func_span)
            elif kind == "BinaryExpr":
                op = payload.get("op") if isinstance(payload, dict) else None
                if isinstance(op, dict) and op.get("kind") == "->":
                    self._access(payload, arrow=True, func_owner=func_owner)
                self._visit(payload, owner, func_owner, func_span)
            elif kind in ("LetDecl", "VarDecl"):
                if func_owner is not None:
                    self._local(payload, kind, func_span)
                elif owner is not None:
                    # A var/let at type scope is a field, and fields are the
                    # single most-accessed member kind in stage 1 -- without
                    # them every `self.ctx` lands in the fallback tier.
                    self._field(payload, kind, owner)
                self._visit(payload, owner, func_owner, func_span)
            else:
                self._visit(payload, owner, func_owner, func_span)

    # ------------------------------------------------------------------ #
    def _decl(
        self,
        kind: str,
        payload: Any,
        owner: Optional[str],
        func_owner: Optional[str],
        func_span: Tuple[int, int],
    ) -> None:
        if not isinstance(payload, dict):
            return

        name_node = payload.get("name")
        idents = flatten_path(name_node) if name_node is not None else []

        # An out-of-line definition carries its owner in the path itself:
        #   fn std::Panic::FrameContext::crash()
        # ...so a qualified prefix wins over the lexically enclosing scope.
        decl_owner = owner
        if len(idents) > 1:
            decl_owner = "::".join(p.get("value", "?") for p in idents[:-1])

        leaf = idents[-1] if idents else None
        start, end = _extent(payload)
        decl: Optional[Decl] = None

        if leaf is not None:
            loc = leaf.get("loc") or {}
            arity = None
            returns = None
            is_op = False

            if kind == "FuncDecl":
                params = payload.get("params")
                arity = len(params) if isinstance(params, list) else 0
                returns, _ = _type_parts(payload.get("returns"))
                is_op = bool(payload.get("is_op"))
            elif kind == "TypeDecl":
                # `type X = Y` -- stash the target in `returns` so the alias
                # map can be built without a dedicated field.
                returns, _ = _type_parts(payload.get("type"))

            decl = Decl(
                name=leaf.get("value", "?"),
                kind=DECL_KINDS[kind],
                file=_norm(loc.get("filename", "?")),
                line=int(loc.get("line_number", 0)),
                col=int(loc.get("column_number", 0)),
                length=int(leaf.get("length", 0) or 0),
                owner=decl_owner,
                arity=arity,
                returns=returns,
                derives=_derives_of(payload) if kind in DERIVING_KINDS else [],
                is_op=is_op,
                start_line=start,
                end_line=end,
            )
            self.index.add_decl(decl)

        # Descend, widening the owner for nodes that open a named scope.
        inner_owner = decl_owner
        if kind in SCOPE_KINDS and leaf is not None:
            leaf_name = leaf.get("value", "?")
            inner_owner = f"{decl_owner}::{leaf_name}" if decl_owner else leaf_name

        inner_func_owner = func_owner
        inner_span = func_span
        if kind == "FuncDecl":
            inner_func_owner = decl_owner
            inner_span = (start, end)
            self._params(payload, inner_span)

        for key, value in payload.items():
            if key == "name":
                continue
            self._visit(value, inner_owner, inner_func_owner, inner_span)

    # ------------------------------------------------------------------ #
    def _params(self, func: dict, span: Tuple[int, int]) -> None:
        params = func.get("params")
        if not isinstance(params, list):
            return
        for param in params:
            got = _named_var(param)
            if got is None:
                continue
            ident, type_name, type_args = got
            if not type_name:
                continue
            loc = ident.get("loc") or {}
            self.index.add_binding(
                Binding(
                    name=ident.get("value", "?"),
                    type_name=type_name,
                    type_args=type_args,
                    file=_norm(loc.get("filename", "?")),
                    line=int(loc.get("line_number", 0)),
                    col=int(loc.get("column_number", 0)),
                    scope_start=span[0],
                    scope_end=span[1],
                )
            )

    # ------------------------------------------------------------------ #
    def _local(self, payload: Any, kind: str, span: Tuple[int, int]) -> None:
        """An annotated local, scoped from its own line to the function's end.

        Without end positions this over-approximates: a local declared inside
        an inner block stays "visible" past that block's close.  Shadowing is
        handled at lookup time by preferring the latest declaration at or
        before the query line.
        """
        got = _named_var({kind: payload})
        if got is None:
            return
        ident, type_name, type_args = got
        if not type_name:
            return
        loc = ident.get("loc") or {}
        line = int(loc.get("line_number", 0))
        self.index.add_binding(
            Binding(
                name=ident.get("value", "?"),
                type_name=type_name,
                type_args=type_args,
                file=_norm(loc.get("filename", "?")),
                line=line,
                col=int(loc.get("column_number", 0)),
                scope_start=line,
                scope_end=max(span[1], line),
            )
        )

    # ------------------------------------------------------------------ #
    def _field(self, payload: Any, kind: str, owner: str) -> None:
        """A ``var``/``let`` declared at type scope, i.e. a member field."""
        got = _named_var({kind: payload})
        if got is None:
            return
        ident, type_name, type_args = got
        loc = ident.get("loc") or {}
        line = int(loc.get("line_number", 0))
        self.index.add_decl(
            Decl(
                name=ident.get("value", "?"),
                kind="field",
                file=_norm(loc.get("filename", "?")),
                line=line,
                col=int(loc.get("column_number", 0)),
                length=int(ident.get("length", 0) or 0),
                owner=owner,
                returns=type_name,
                type_args=type_args,
                start_line=line,
                end_line=line,
            )
        )

    # ------------------------------------------------------------------ #
    def _access(self, payload: Any, arrow: bool, func_owner: Optional[str]) -> None:
        """Record one member-access site.

        ``a.b`` arrives as ``DotPathExpr{lhs, rhs}`` and ``a->b`` as
        ``BinaryExpr{lhs, op:'->', rhs}``; both have the same lhs/rhs shape, so
        one routine handles both.
        """
        if not isinstance(payload, dict):
            return
        lhs, rhs = payload.get("lhs"), payload.get("rhs")
        if not isinstance(lhs, dict) or not isinstance(rhs, dict):
            return

        # Receiver: `self`, a field of `self`, a plain name, or something we
        # decline to model.
        recv_kind, recv_name = RECV_CHAIN, None
        lhs_ident = lhs.get("IdentExpr")
        if isinstance(lhs_ident, dict):
            value = lhs_ident.get("value")
            if value == "self":
                recv_kind = RECV_SELF
            elif value:
                recv_kind, recv_name = RECV_NAME, value
        else:
            # `self.diag->report(...)`: the receiver of the arrow is itself a
            # `self.` access.  Very common in stage 1, and resolvable, so it
            # gets its own kind rather than being written off as a chain.
            inner = lhs.get("DotPathExpr")
            if isinstance(inner, dict):
                inner_lhs = inner.get("lhs")
                inner_rhs = inner.get("rhs")
                if (
                    isinstance(inner_lhs, dict)
                    and isinstance(inner_rhs, dict)
                    and inner_lhs.get("IdentExpr", {}).get("value") == "self"
                    and "IdentExpr" in inner_rhs
                ):
                    recv_kind = RECV_FIELD
                    recv_name = inner_rhs["IdentExpr"].get("value")

        # Member: either a bare field or a call.  A call also gives us argc,
        # which is the cheapest overload discriminator available.
        argc = None
        member = rhs
        call = rhs.get("FunctionCallExpr")
        if isinstance(call, dict):
            member = call.get("path") or {}
            args = call.get("args")
            if isinstance(args, dict):
                arg_list = args.get("ArgumentListExpr")
                argc = len(arg_list) if isinstance(arg_list, list) else 0

        parts = flatten_path(member)
        if not parts:
            return
        leaf = parts[-1]
        loc = leaf.get("loc") or {}

        self.index.add_access(
            Access(
                name=leaf.get("value", "?"),
                file=_norm(loc.get("filename", "?")),
                line=int(loc.get("line_number", 0)),
                col=int(loc.get("column_number", 0)),
                length=int(leaf.get("length", 0) or 0),
                arrow=arrow,
                recv_kind=recv_kind,
                recv_name=recv_name,
                enclosing_owner=func_owner,
                argc=argc,
            )
        )


# ---------------------------------------------------------------------- #
# Building
# ---------------------------------------------------------------------- #
def fold_documents(
    index: Index,
    docs: Iterable[dict],
    lock: Optional[threading.Lock] = None,
) -> int:
    """Fold AST documents into ``index``, skipping files already seen."""
    folded = 0
    for doc in docs:
        locs = list(_locs(doc))
        origin = locs[0].get("filename") if locs else None
        if origin is None:
            continue
        origin = os.path.realpath(origin)

        if lock:
            lock.acquire()
        try:
            if origin in index.seen_files:
                continue
            index.seen_files.add(origin)
            Extractor(index).feed(doc)
            folded += 1
        finally:
            if lock:
                lock.release()
    return folded


def index_file(
    index: Index,
    kairo_bin: str,
    path: str,
    extra_args: Optional[List[str]] = None,
    cwd: Optional[str] = None,
    lock: Optional[threading.Lock] = None,
) -> int:
    """Index ``path`` and everything it imports."""
    return fold_documents(index, run_emit_ast(kairo_bin, path, extra_args, cwd), lock)


def build_index(
    kairo_bin: str,
    roots: Iterable[str],
    extra_args: Optional[List[str]] = None,
    cwd: Optional[str] = None,
    jobs: Optional[int] = None,
) -> Index:
    """Index every file in ``roots``.

    Measured on stage 0: ~400 bytes of JSON per source line, ~106 MB/s to
    parse.  Stage 1's ~82k lines is ~55 MB of JSON, folded in a few seconds.
    The cost is stage 0 process spawns, not parsing, so this fans out -- and
    because one root pulls in its whole import closure, indexing
    ``Compiler/Driver/Kairo.k`` alone covers all 144 files in ~5 s.

    Parsed trees are dropped as soon as they are folded in; only the flat
    tables survive.
    """
    index = Index()
    lock = threading.Lock()
    jobs = jobs or min(8, (os.cpu_count() or 4))

    with ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = [
            pool.submit(index_file, index, kairo_bin, root, extra_args, cwd, lock)
            for root in roots
        ]
        for f in futures:
            try:
                f.result()
            except Exception:
                logger.exception("indexing task failed")

    logger.info("index built: %s", index.stats())
    return index


def index_from_dump(path: str) -> Index:
    """Build an index from a saved ``--emit-ast`` dump (no stage 0 spawn)."""
    index = Index()
    fold_documents(index, load_ast_dump(path))
    logger.info("index built from %s: %s", path, index.stats())
    return index


def discover_sources(root: str, exclude: Iterable[str] = ("build", ".git")) -> List[str]:
    """Every ``.k`` file under ``root``, minus the usual noise."""
    excluded = set(exclude)
    out: List[str] = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in excluded]
        for name in filenames:
            if name.endswith((".k", ".kro")):
                out.append(os.path.join(dirpath, name))
    return out


# ---------------------------------------------------------------------- #
# Persistence
# ---------------------------------------------------------------------- #
#: Bump when Decl/Binding/Access gain or lose a field.  A mismatch invalidates
#: every entry rather than half-loading a stale schema.
#:
#: v2: v1 could persist empty entries for files that failed to parse.  Those
#: reload as "fresh", mark the file seen, and suppress any retry -- so a single
#: bad indexing run poisoned the cache permanently.  The bump discards them.
CACHE_VERSION = 2


class IndexCache:
    """On-disk cache of the flat tables, one entry per source file.

    The raw AST is not cached -- it is ~55 MB of JSON for stage 1, versus a
    fraction of that for the extracted tables.  Only the by-file tables are
    stored; ``decls_by_name``, ``types_by_name`` and ``aliases`` are derived
    and rebuilt by replaying ``add_decl`` on load.

    One file per entry rather than a single blob, so a save rewrites exactly
    one small file instead of the whole index.
    """

    def __init__(self, root: str) -> None:
        self.dir = os.path.join(root, ".cache", "kairo-index")

    # ------------------------------------------------------------------ #
    @staticmethod
    def _stamp(path: str) -> Optional[Tuple[int, int]]:
        try:
            st = os.stat(path)
        except OSError:
            return None
        return st.st_mtime_ns, st.st_size

    def _entry_path(self, path: str) -> str:
        digest = hashlib.sha1(path.encode("utf-8")).hexdigest()[:16]
        return os.path.join(self.dir, f"{digest}.json")

    # ------------------------------------------------------------------ #
    def load_into(self, index: Index) -> Tuple[int, int]:
        """Replay every fresh cache entry into ``index``.

        Returns ``(loaded, stale)``.  An entry whose source file has changed or
        vanished is ignored and left for the caller to re-index.
        """
        loaded = stale = 0
        if not os.path.isdir(self.dir):
            return 0, 0

        for name in os.listdir(self.dir):
            if not name.endswith(".json"):
                continue
            try:
                with open(os.path.join(self.dir, name), "r", encoding="utf-8") as fh:
                    entry = json.load(fh)
            except Exception:
                stale += 1
                continue

            if entry.get("v") != CACHE_VERSION:
                stale += 1
                continue

            path = entry.get("path", "")
            stamp = self._stamp(path)
            if stamp is None or list(stamp) != entry.get("stamp"):
                stale += 1
                continue

            if not (entry.get("decls") or entry.get("bindings")
                    or entry.get("accesses")):
                # Belt and braces alongside the v2 bump: an entry carrying
                # nothing tells us only that a parse failed once.  Treating it
                # as authoritative would mark the file seen and stop it ever
                # being retried.
                stale += 1
                continue

            try:
                for d in entry.get("decls", []):
                    index.add_decl(Decl(**d))
                for b in entry.get("bindings", []):
                    index.add_binding(Binding(**b))
                for a in entry.get("accesses", []):
                    index.add_access(Access(**a))
            except TypeError:
                # Schema drifted without CACHE_VERSION being bumped.  Drop the
                # partial file rather than serve half a record.
                index.drop_file(path)
                stale += 1
                continue

            index.seen_files.add(path)
            loaded += 1

        return loaded, stale

    # ------------------------------------------------------------------ #
    def store(self, index: Index, path: str) -> None:
        """Persist just ``path``'s slice of ``index``."""
        stamp = self._stamp(path)
        if stamp is None:
            return

        entry = {
            "v": CACHE_VERSION,
            "path": path,
            "stamp": list(stamp),
            "decls": [asdict(d) for d in index.decls_by_file.get(path, [])],
            "bindings": [asdict(b) for b in index.bindings_by_file.get(path, [])],
            "accesses": [asdict(a) for a in index.accesses_by_file.get(path, [])],
        }

        try:
            os.makedirs(self.dir, exist_ok=True)
            target = self._entry_path(path)
            # Write-then-rename: a reader must never see a half-written entry.
            tmp = target + ".tmp"
            with open(tmp, "w", encoding="utf-8") as fh:
                json.dump(entry, fh)
            os.replace(tmp, target)
        except OSError as e:
            logger.warning("could not cache %s: %s", path, e)

    def store_all(self, index: Index, paths: Iterable[str]) -> None:
        for p in paths:
            self.store(index, p)


# ---------------------------------------------------------------------- #
# Eager whole-workspace indexing
# ---------------------------------------------------------------------- #
def index_single(
    index: Index,
    kairo_bin: str,
    path: str,
    cwd: Optional[str] = None,
    lock: Optional[threading.Lock] = None,
) -> int:
    """Index ``path`` alone, without walking its import closure."""
    return fold_documents(index, run_index_file(kairo_bin, path, cwd), lock)


def build_index_eager(
    kairo_bin: str,
    root: str,
    cache: Optional[IndexCache] = None,
    cwd: Optional[str] = None,
    jobs: Optional[int] = None,
    exclude: Iterable[str] = ("build", ".git", ".cache", "llvm-runtimes"),
    progress: Optional[Any] = None,
) -> Tuple[Index, int, int]:
    """Index every ``.k`` file under ``root`` up front.

    Uses ``--index-file`` per file rather than closure walks.  That is both
    faster (median 3 ms/file on stage 1) and more complete -- a closure walk
    only reaches files some driver imports, so anything unreferenced would stay
    invisible until opened.  Cross-file linking happens by name in this index,
    so per-file parses lose nothing.

    Returns ``(index, indexed, from_cache)``.
    """
    index = Index()
    lock = threading.Lock()
    jobs = jobs or min(8, (os.cpu_count() or 4))

    from_cache = 0
    if cache is not None:
        from_cache, stale = cache.load_into(index)
        logger.info("index cache: %d entries loaded, %d stale", from_cache, stale)

    sources = [os.path.realpath(p) for p in discover_sources(root, exclude)]
    todo = [p for p in sources if p not in index.seen_files]

    logger.info("eager index: %d files, %d cached, %d to parse",
                len(sources), len(sources) - len(todo), len(todo))

    done = 0
    produced: List[str] = []
    # Aim for ~50 updates regardless of corpus size, so the progress bar moves
    # smoothly on a small workspace and does not spam on a large one.
    step = max(1, len(todo) // 50)
    with ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = {
            pool.submit(index_single, index, kairo_bin, p, cwd, lock): p
            for p in todo
        }
        for f in futures:
            try:
                if f.result():
                    produced.append(futures[f])
            except Exception:
                logger.exception("eager index of %s failed", futures[f])
            done += 1
            if progress is not None and (done % step == 0 or done == len(todo)):
                try:
                    progress(done, len(todo))
                except Exception:
                    logger.warning("progress callback failed", exc_info=True)

    # Only files that actually yielded an AST are recorded as seen or cached.
    #
    # An earlier version marked every attempted file seen, to avoid re-parsing
    # ones stage 0 cannot handle.  That was wrong twice over: it suppressed the
    # closure-walk fallback in ensure() -- which is exactly the thing that can
    # succeed where a standalone parse fails, since it supplies the imports --
    # and when the whole run failed (a compiler without --index-file, say) it
    # marked all 540 files seen with zero decls and left go-to-definition dead
    # with no way to recover short of deleting the cache.
    if cache is not None and produced:
        cache.store_all(index, produced)

    failed = len(todo) - len(produced)
    if todo and not produced:
        # Nothing at all parsed.  Almost always environmental -- wrong binary,
        # missing flag -- rather than 540 individually broken files.  Say so
        # loudly and let the caller decide not to trust this index.
        logger.error(
            "eager index produced NOTHING from %d files -- is '%s' a stage 0 "
            "build with --index-file support?", len(todo), kairo_bin,
        )
    elif failed:
        logger.info("eager index: %d/%d files yielded no AST (left unseen so "
                    "ensure() can retry them via the import closure)",
                    failed, len(todo))

    logger.info("eager index complete: %s", index.stats())
    return index, len(produced), from_cache


if __name__ == "__main__":
    import argparse
    import time

    ap = argparse.ArgumentParser(description="build a Kairo symbol index")
    ap.add_argument("source", help="stage 0 binary, or a saved --emit-ast dump")
    ap.add_argument("root", nargs="?", help="file or directory to index")
    ap.add_argument("-j", "--jobs", type=int, default=None)
    ap.add_argument("-I", "--include", action="append", default=[])
    ap.add_argument("--name", help="dump every decl matching this name")
    args = ap.parse_args()

    logging.basicConfig(level=logging.INFO, format="%(message)s")
    t0 = time.time()

    if args.root is None:
        idx = index_from_dump(args.source)
    else:
        extra = [f"-I{p}" for p in args.include]
        roots = (
            [args.root]
            if os.path.isfile(args.root)
            else discover_sources(args.root)
        )
        print(f"indexing {len(roots)} root(s)...")
        idx = build_index(args.source, roots, extra_args=extra, jobs=args.jobs)

    print(f"{idx.stats()} in {time.time() - t0:.1f}s")

    if args.name:
        for d in idx.lookup(args.name):
            arity = "" if d.arity is None else f"/{d.arity}"
            drv = f"  derives {d.derives}" if d.derives else ""
            print(f"  {d.kind:10} {d.qualified}{arity}  {d.file}:{d.line}:{d.col}{drv}")
