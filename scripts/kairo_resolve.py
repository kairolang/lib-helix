"""Go-to-definition over a :class:`kairo_index.Index`.

The contract is deliberately *not* "return one answer".  It is "return a
correct set, best first".  Stage 0 has no symbol table, so precision is
allowed to degrade -- correctness is not.  A jump that silently lands on the
wrong ``bar()`` is worse than a picker with three entries.

Three tiers, in order:

``EXACT``
    The receiver's type is known syntactically -- ``self.foo()``, a parameter
    or annotated local with a declared type, or an already-qualified path.
    Candidates are restricted to that type and its ``derives`` closure.

``TYPED``
    The receiver's type is known but the member is not declared anywhere in
    its closure.  Usually means the member is inherited through something the
    index cannot see (an ``inline "c++"`` block, an FFI type), or the closure
    is incomplete.  Falls back to a workspace-wide search but says so.

``GUESS``
    The receiver is a call result, an index expression, or another chain, so
    the type is unknown.  Every member of that name in the workspace, ranked.

On stage 1's own source, ~51% of ``.`` accesses have ``self`` as the direct
receiver, so the EXACT path carries most of the traffic -- but only if the
``derives`` closure is walked: ``ASTParse`` derives seven mixins and
``self.parse_decl()`` is declared in ``DeclParse``, not in ``ASTParse``.
"""

from __future__ import annotations

import logging
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Sequence, Set, Tuple

from kairo_index import (
    RECV_CHAIN,
    RECV_FIELD,
    RECV_NAME,
    RECV_SELF,
    Access,
    Binding,
    Decl,
    Index,
)

logger = logging.getLogger("KairoLSP.resolve")

EXACT = "exact"
TYPED = "typed"
GUESS = "guess"

#: Single-argument wrappers that ``->`` dereferences through.  ``obs::<T>``
#: behaves as a ``T`` on the far side of an arrow, so the generic argument is
#: the interesting type.  Tune this list as the standard library moves; it is
#: policy, not a fact the AST can supply.
DEREF_WRAPPERS = {
    "obs",
    "view",
    "ref",
    "ptr",
    "unique",
    "shared",
    "weak",
    "opt",
    "Optional",
}

#: Containers whose element type is what member access usually means after an
#: iteration or subscript.  Only consulted for RECV_NAME receivers.
ELEMENT_WRAPPERS = {"vec", "set", "map", "array", "span", "list"}


@dataclass(slots=True)
class Candidate:
    decl: Decl
    score: int  # lower is better
    why: str

    @property
    def location(self) -> Tuple[str, int, int]:
        return self.decl.file, self.decl.line, self.decl.col


@dataclass
class Resolution:
    """The answer to one go-to-definition query."""

    name: str
    tier: str  # EXACT | TYPED | GUESS
    candidates: List[Candidate] = field(default_factory=list)
    receiver_type: Optional[str] = None
    closure: List[str] = field(default_factory=list)
    note: str = ""

    def __bool__(self) -> bool:
        return bool(self.candidates)

    @property
    def best(self) -> Optional[Decl]:
        return self.candidates[0].decl if self.candidates else None

    def describe(self) -> str:
        head = f"{self.name}: {self.tier}"
        if self.receiver_type:
            head += f" (receiver {self.receiver_type})"
        return head


# ---------------------------------------------------------------------- #
# Type-name plumbing
# ---------------------------------------------------------------------- #
def resolve_alias(index: Index, name: str, _depth: int = 0) -> str:
    """Follow ``type X = Y`` chains to a concrete name."""
    seen: Set[str] = set()
    while name in index.aliases and name not in seen:
        seen.add(name)
        name = index.aliases[name]
    return name


def deref(type_name: str, type_args: Sequence[str], arrow: bool) -> str:
    """The type a member access actually lands on.

    ``ctx: obs::<ASTContext>`` accessed with ``->`` yields ``ASTContext``.  A
    ``.`` on the same binding yields ``obs`` -- accessing the wrapper's own
    members -- which is the correct distinction, not an approximation.
    """
    if arrow and type_name in DEREF_WRAPPERS and type_args:
        return type_args[0]
    return type_name


def derives_closure(index: Index, type_name: str, limit: int = 64) -> List[str]:
    """``type_name`` plus every type it derives, transitively.

    Breadth first so nearer bases sort earlier, which is also the order a
    reader expects in a picker.  Cycles and runaway hierarchies are bounded by
    ``limit``; Kairo permits diamond mixins and this must not hang the editor.
    """
    root = resolve_alias(index, type_name)
    order: List[str] = []
    seen: Set[str] = {root}
    queue: List[str] = [root]

    while queue and len(order) < limit:
        current = queue.pop(0)
        order.append(current)
        for decl in index.types_by_name.get(current, []):
            for base in decl.derives:
                base = resolve_alias(index, base)
                if base not in seen:
                    seen.add(base)
                    queue.append(base)

    return order


# ---------------------------------------------------------------------- #
# Receiver typing
# ---------------------------------------------------------------------- #
def lookup_binding(
    index: Index, file: str, name: str, line: int
) -> Optional[Binding]:
    """The binding for ``name`` visible at ``line``.

    Locals are recorded with an over-approximate extent (no end positions
    exist in the AST), so shadowing is resolved here: of every candidate whose
    range covers ``line``, the one declared latest at or before ``line`` wins,
    and a declaration below the cursor never does.
    """
    best: Optional[Binding] = None
    for b in index.bindings_by_file.get(file, []):
        if b.name != name or not b.in_scope(line):
            continue
        if b.line > line:
            continue
        if best is None or b.line > best.line:
            best = b
    return best


def receiver_type(index: Index, access: Access) -> Optional[str]:
    """The declared type of an access site's receiver, if it is knowable."""
    if access.recv_kind == RECV_SELF:
        # `self` is typed by the enclosing function's owner.  Out-of-line
        # definitions carry a qualified owner, so take the innermost segment.
        owner = access.enclosing_owner
        return owner.rsplit("::", 1)[-1] if owner else None

    if access.recv_kind == RECV_FIELD and access.recv_name:
        # `self.diag->report(...)`: find `diag` as a field of the enclosing
        # type's closure, then dereference through the arrow.
        owner = access.enclosing_owner
        if not owner:
            return None
        for type_name in derives_closure(index, owner.rsplit("::", 1)[-1]):
            for d in index.lookup(access.recv_name):
                if d.kind == "field" and d.leaf_owner == type_name and d.returns:
                    return deref(d.returns, d.type_args, access.arrow)
        return None

    if access.recv_kind == RECV_NAME and access.recv_name:
        binding = lookup_binding(index, access.file, access.recv_name, access.line)
        if binding is None:
            # A bare name that is not a local may be a type used as a scope,
            # e.g. `TokenKind::TkEof`.  Treat it as its own type if known.
            if access.recv_name in index.types_by_name:
                return access.recv_name
            return None
        return deref(binding.type_name, binding.type_args, access.arrow)

    return None


# ---------------------------------------------------------------------- #
# Ranking
# ---------------------------------------------------------------------- #
def _rank(
    decls: Sequence[Decl],
    access: Optional[Access],
    closure: Sequence[str],
    origin_file: Optional[str],
) -> List[Candidate]:
    """Order candidates best-first.

    The signals, strongest first:

    * position in the ``derives`` closure -- the receiver's own type beats its
      first mixin, which beats the mixin's base
    * arity agreement with the call site, which separates most overloads for
      free (``range/1`` vs ``range/3``) without any type checking
    * same file, then same owner prefix as the access site
    """
    closure_rank = {name: i for i, name in enumerate(closure)}
    out: List[Candidate] = []

    for d in decls:
        score = 0
        reasons: List[str] = []

        owner_leaf = d.leaf_owner
        if owner_leaf in closure_rank:
            depth = closure_rank[owner_leaf]
            score += depth
            reasons.append("own type" if depth == 0 else f"derives depth {depth}")
        elif closure:
            score += 100
            reasons.append("outside closure")

        if access is not None and access.argc is not None and d.arity is not None:
            # `self` occupies a parameter slot in method declarations, so a
            # call with N arguments matches an arity of N or N+1.
            if d.arity in (access.argc, access.argc + 1):
                reasons.append(f"arity {d.arity}")
            else:
                score += 20
                reasons.append(f"arity {d.arity} != {access.argc}")

        if origin_file and d.file == origin_file:
            score -= 2
            reasons.append("same file")

        if d.kind == "func":
            score -= 1

        out.append(Candidate(decl=d, score=score, why=", ".join(reasons)))

    out.sort(key=lambda c: (c.score, c.decl.file, c.decl.line))
    return out


# ---------------------------------------------------------------------- #
# Entry points
# ---------------------------------------------------------------------- #
def access_at(index: Index, file: str, line: int, col: int) -> Optional[Access]:
    """The member-access site under a cursor position.

    ``col`` is 0-based as LSP delivers it; stage 0 emits 1-based columns, and
    the comparison below works in stage 0's space after adjusting.
    """
    target = col + 1
    for a in index.accesses_by_file.get(file, []):
        if a.line != line:
            continue
        if a.col <= target <= a.col + max(a.length, 1):
            return a
    return None


def resolve_access(index: Index, access: Access) -> Resolution:
    """Resolve one member access to a ranked candidate set."""
    rtype = receiver_type(index, access)

    if rtype is None:
        decls = [d for d in index.lookup(access.name) if d.owner]
        return Resolution(
            name=access.name,
            tier=GUESS,
            candidates=_rank(decls, access, (), access.file),
            note="receiver type unknown (call result, subscript, or chain)",
        )

    closure = derives_closure(index, rtype)
    in_closure = {t for t in closure}
    members = [
        d
        for d in index.lookup(access.name)
        if d.leaf_owner in in_closure
    ]

    if members:
        return Resolution(
            name=access.name,
            tier=EXACT,
            candidates=_rank(members, access, closure, access.file),
            receiver_type=rtype,
            closure=closure,
        )

    # Type is known, member is not in its closure.  Report honestly rather
    # than silently widening: this is the signature of a member that arrives
    # through an `inline "c++"` block or an FFI type the index cannot read.
    decls = [d for d in index.lookup(access.name) if d.owner]
    return Resolution(
        name=access.name,
        tier=TYPED,
        candidates=_rank(decls, access, (), access.file),
        receiver_type=rtype,
        closure=closure,
        note=f"'{access.name}' not declared in {rtype} or its {len(closure)} base(s)",
    )


def resolve(
    index: Index,
    file: str,
    line: int,
    col: int,
    name: Optional[str] = None,
) -> Resolution:
    """Resolve the symbol at ``file:line:col``.

    ``line`` is 1-based and ``col`` 0-based, matching LSP after the usual
    ``line + 1`` adjustment by the caller.  ``name`` is an optional fallback
    for positions that are not member accesses -- a bare type name or free
    function -- where the caller already knows the word under the cursor.
    """
    access = access_at(index, file, line, col)
    if access is not None:
        return resolve_access(index, access)

    if name:
        decls = index.lookup(name)
        return Resolution(
            name=name,
            tier=EXACT if len(decls) == 1 else GUESS,
            candidates=_rank(decls, None, (), file),
            note="" if decls else "no declaration of that name in the index",
        )

    return Resolution(name="", tier=GUESS, note="nothing resolvable at that position")


if __name__ == "__main__":
    import argparse
    import time

    import kairo_index as ki

    ap = argparse.ArgumentParser(description="resolve a Kairo symbol")
    ap.add_argument("dump", help="a saved --emit-ast dump")
    ap.add_argument("file", nargs="?", help="source file of the query")
    ap.add_argument("line", nargs="?", type=int)
    ap.add_argument("col", nargs="?", type=int)
    ap.add_argument("--name", help="resolve by name instead of position")
    ap.add_argument("--audit", action="store_true", help="tier breakdown over all sites")
    args = ap.parse_args()

    logging.basicConfig(level=logging.WARNING, format="%(message)s")

    t0 = time.time()
    idx = ki.index_from_dump(args.dump)
    print(f"{idx.stats()}  ({time.time() - t0:.1f}s)")

    if args.audit:
        import collections

        tiers = collections.Counter()
        single = collections.Counter()
        t0 = time.time()
        total = 0
        for sites in idx.accesses_by_file.values():
            for a in sites:
                r = resolve_access(idx, a)
                tiers[r.tier] += 1
                single[r.tier] += 1 if len(r.candidates) == 1 else 0
                total += 1
        elapsed = time.time() - t0
        print(f"\nresolved {total} access sites in {elapsed:.1f}s "
              f"({total / elapsed:,.0f}/s)")
        for tier in (EXACT, TYPED, GUESS):
            n = tiers[tier]
            pct = 100.0 * n / total if total else 0.0
            uniq = 100.0 * single[tier] / n if n else 0.0
            print(f"  {tier:6} {n:6} ({pct:5.1f}%)   single-candidate: {uniq:5.1f}%")
        raise SystemExit(0)

    if args.name:
        res = resolve(idx, args.file or "", 0, 0, name=args.name)
    else:
        res = resolve(idx, args.file, args.line, args.col)

    print(f"\n{res.describe()}")
    if res.note:
        print(f"  note: {res.note}")
    if res.closure:
        print(f"  closure: {' -> '.join(res.closure[:8])}")
    for c in res.candidates[:10]:
        f, ln, cl = c.location
        print(f"  [{c.score:4}] {c.decl.qualified}  {f}:{ln}:{cl}   ({c.why})")
