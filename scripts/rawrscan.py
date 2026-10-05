#!/usr/bin/env python3

"""
rawrscan  —  structural module + preprocessor scanner for rawr .cppm / .pp / .cpp files.

Core:  parse_content(text: str) -> list[Node]   (pure, no IO)

Everything else is IO and processing around this one function.

Node types
----------

    Include(path, angle, line)
    ModuleDecl(name, line)
    ModuleImport(name, re_export, line)
    Define(name, body, line)

        body: everything after the macro name, backslash continuation collapsed.

    PragmaOnce(line)
    LineDir(target_line, file, line)
    IfNode(line, condition, inner)
    ElseNode(line, inner)
    IfChain(if_node, elif_nodes, else_node, end_line)

        Flat: elif branches are siblings, not nested.
        end_line: 1-based line number of the closing #endif.

Manifest format v5
------------------

    # rawr module manifest v5
    # kind|path|module|requires|pp_includes|size|mtime_ns|sha256|root

    pp_includes  Quoted .pp files directly #included by this file, across all
                 branches (comma-separated display paths matching pp entries).

                 Consumers (e.g. Meson) use this to transitively resolve module
                 deps that come through pp files instead of direct imports.

    root         Absolute path of the --root used when this entry was created.
                 Stale entries (root matches current --root but file gone) are
                 dropped on next write; entries from other roots are preserved,
                 enabling multi-root manifests (src/ modules + include/cppm/ pp).

Backward compat:  v4 (8 cols, no pp_includes), v3 (7 cols, no pp_includes/root).
"""

from __future__ import annotations

import argparse
import graphlib
import hashlib
import json
import os
import re
import sys
from dataclasses import dataclass, field
from typing import Generator, Union


# ── Patterns ──────────────────────────────────────────────────────────────────

_INC    = re.compile(r'^[ \t]*#[ \t]*include[ \t]*(?P<q>[<"])(?P<p>[^>"]+)[>"]')
_DIR    = re.compile(r'^[ \t]*#[ \t]*(?P<kw>\w+)(?P<body>.*)')
_DECL   = re.compile(r'^[ \t]*export[ \t]+module[ \t]+(?P<n>[\w.]+)[ \t]*;')
_IMP    = re.compile(r'^[ \t]*(?P<x>export[ \t]+)?import[ \t]+(?P<n>[\w.]+)[ \t]*;')
_LDIR   = re.compile(r'^[ \t]*#[ \t]*line[ \t]+(?P<n>\d+)[ \t]+"(?P<f>[^"]+)"')
_ONCE   = re.compile(r'^[ \t]*#[ \t]*pragma[ \t]+once\b')
_CMT    = re.compile(r'^[ \t]*//')
_DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(?P<name>[A-Za-z_]\w*)(?P<rest>.*)')


# ── Node types ────────────────────────────────────────────────────────────────

@dataclass
class Include:
    path: str; angle: bool; line: int

@dataclass
class ModuleDecl:
    name: str; line: int

@dataclass
class ModuleImport:
    name: str; re_export: bool; line: int

@dataclass
class Define:
    """#define with backslash-continuation lines joined and \ stripped."""
    name: str; body: str; line: int

@dataclass
class PragmaOnce:
    line: int

@dataclass
class LineDir:
    target_line: int; file: str; line: int

@dataclass
class IfNode:
    """One conditional branch (#if or #elif)."""
    line:     int
    condition: str
    inner:     list['Node'] = field(default_factory=list)

@dataclass
class ElseNode:
    """The #else branch."""
    line:  int
    inner: list['Node'] = field(default_factory=list)

@dataclass
class IfChain:
    """A complete #if / #elif* / #else? / #endif chain.

    elif branches are flat siblings (not nested).  end_line is the 1-based
    line number of the closing #endif, enabling consumers to extract exact
    source line ranges for each branch without re-parsing.
    """
    if_node:   IfNode
    elif_nodes: list[IfNode]    = field(default_factory=list)
    else_node:  ElseNode | None = None
    end_line:   int             = 0   # set when #endif is parsed


Node = Union[Include, ModuleDecl, ModuleImport, Define, PragmaOnce, LineDir, IfChain]


# ── Condition normalisation ───────────────────────────────────────────────────

def _cond(kw: str, body: str) -> str:
    b = body.strip()
    if kw in ('ifdef',  'elifdef'):  return f'defined({b})'
    if kw in ('ifndef', 'elifndef'): return f'!defined({b})'
    return b


def eval_condition(cond: str, defines: dict[str, bool]) -> bool:
    """Evaluate a single normalised condition string against *defines*."""
    c = cond.strip()
    if m := re.fullmatch(r'defined\(\s*(\w+)\s*\)', c): return defines.get(m[1], False)
    if m := re.fullmatch(r'!\s*defined\(\s*(\w+)\s*\)', c): return not defines.get(m[1], False)
    if re.fullmatch(r'\w+', c): return defines.get(c, False)
    return False


# ── Core parser ───────────────────────────────────────────────────────────────

def parse_content(text: str) -> list[Node]:
    """Parse *text* into a Node list.  Pure — no IO, no side effects.

    Handles backslash continuation in both #define bodies and #if/#elif
    conditions:
        #if RAWR_PLATFORM_LINUX   || \
            RAWR_PLATFORM_MACOS   || \
            RAWR_ENV_CYGWIN

    The parts are joined with a single space into one condition string.
    This prevents the continuation backslash from leaking into the stored
    condition and subsequently into generated #if/#elif lines in .undef.pp
    files (where it would cause the preprocessor to join the following
    #undef into the condition expression, producing "invalid token" errors).
    """
    root:  list[Node] = []
    stack: list[tuple[IfChain, list[Node]]] = []

    # State: multi-line #define body accumulation
    _in_def    = False
    _def_name  = ''
    _def_parts: list[str] = []
    _def_line  = 0

    # State: multi-line #if / #elif condition accumulation
    _in_dir_cond  = False
    _dir_cond_kw  = ''
    _dir_cond_parts: list[str] = []
    _dir_cond_line = 0

    def cur() -> list[Node]:
        return stack[-1][1] if stack else root

    def _commit_dir_cond() -> None:
        """Apply the fully-accumulated multi-line directive condition."""
        full = ' '.join(p for p in _dir_cond_parts if p)
        kw   = _dir_cond_kw
        ln   = _dir_cond_line

        if kw in ('if', 'ifdef', 'ifndef'):
            chain = IfChain(if_node=IfNode(ln, _cond(kw, full)))
            stack.append((chain, chain.if_node.inner))

        elif kw in ('elif', 'elifdef', 'elifndef'):
            if stack:
                chain, _ = stack[-1]
                new_elif  = IfNode(ln, _cond(kw, full))
                chain.elif_nodes.append(new_elif)
                stack[-1] = (chain, new_elif.inner)

    for lineno, raw in enumerate(text.splitlines(), 1):
        # ── Multi-line #define continuation ──────────────────────────────────
        if _in_def:
            stripped = raw.rstrip()

            if stripped.endswith('\\'):
                _def_parts.append(stripped[:-1].strip())
            else:
                _def_parts.append(stripped.strip())
                body = ' '.join(p for p in _def_parts if p)
                cur().append(Define(_def_name, body, _def_line))
                _in_def = False

            continue

        # ── Multi-line #if / #elif condition continuation ─────────────────────
        if _in_dir_cond:
            stripped = raw.rstrip()

            if stripped.endswith('\\'):
                _dir_cond_parts.append(stripped[:-1].strip())
            else:
                _dir_cond_parts.append(stripped.strip())
                _commit_dir_cond()
                _in_dir_cond = False

            continue

        if _CMT.match(raw): continue

        if m := _LDIR.match(raw):
            cur().append(LineDir(int(m['n']), m['f'], lineno)); continue

        if _ONCE.match(raw):
            cur().append(PragmaOnce(lineno)); continue

        if m := _DECL.match(raw):
            cur().append(ModuleDecl(m['n'], lineno)); continue

        if m := _IMP.match(raw):
            n = m['n']

            if n.replace('.', '').replace('_', '').isalnum():
                cur().append(ModuleImport(n, bool(m['x']), lineno))

            continue

        if m := _INC.match(raw):
            cur().append(Include(m['p'], m['q'] == '<', lineno)); continue

        if m := _DEFINE.match(raw):
            name = m['name']
            rest = m['rest'].rstrip()

            if rest.endswith('\\'):
                _in_def    = True
                _def_name  = name
                _def_parts = [rest[:-1].strip()]
                _def_line  = lineno
            else:
                cur().append(Define(name, rest.strip(), lineno))

            continue

        if not (m := _DIR.match(raw)):
            continue

        kw   = m['kw'].lower()
        body = m['body'].strip()

        if kw in ('if', 'ifdef', 'ifndef', 'elif', 'elifdef', 'elifndef'):
            # Strip trailing backslash; if present, begin multi-line accumulation.
            body_r = body.rstrip('\\').rstrip() if body.endswith('\\') else None

            if body_r is not None:
                _in_dir_cond    = True
                _dir_cond_kw    = kw
                _dir_cond_parts = [body_r]
                _dir_cond_line  = lineno
            else:
                # Single-line condition — handle immediately.
                if kw in ('if', 'ifdef', 'ifndef'):
                    chain = IfChain(if_node=IfNode(lineno, _cond(kw, body)))
                    stack.append((chain, chain.if_node.inner))
                else:
                    if stack:
                        chain, _ = stack[-1]
                        new_elif  = IfNode(lineno, _cond(kw, body))
                        chain.elif_nodes.append(new_elif)
                        stack[-1] = (chain, new_elif.inner)

        elif kw == 'else':
            if stack:
                chain, _ = stack[-1]
                chain.else_node = ElseNode(lineno)
                stack[-1] = (chain, chain.else_node.inner)

        elif kw == 'endif':
            if stack:
                chain, _ = stack.pop()
                chain.end_line = lineno
                cur().append(chain)

    return root


# ── IO ────────────────────────────────────────────────────────────────────────

def parse_file(path: str) -> list[Node]:
    with open(path, 'r', encoding='utf-8', newline='') as f:
        return parse_content(f.read())


def parse_file_with_lines(path: str) -> tuple[list[Node], list[str]]:
    """Parse *path*, return (nodes, lines).  Single file open."""
    with open(path, 'r', encoding='utf-8', newline='') as f:
        text = f.read()

    return parse_content(text), text.splitlines(keepends=True)


# ── Tree traversal ────────────────────────────────────────────────────────────

def _all_branches(chain: IfChain) -> Generator[list[Node], None, None]:
    yield chain.if_node.inner

    for e in chain.elif_nodes:
        yield e.inner

    if chain.else_node:
        yield chain.else_node.inner


def walk_includes(nodes: list[Node]) -> Generator[Include, None, None]:
    for n in nodes:
        if isinstance(n, Include):
            yield n
        elif isinstance(n, IfChain):
            for b in _all_branches(n):
                yield from walk_includes(b)


def walk_imports(nodes: list[Node]) -> Generator[ModuleImport, None, None]:
    for n in nodes:
        if isinstance(n, ModuleImport):
            yield n
        elif isinstance(n, IfChain):
            for b in _all_branches(n):
                yield from walk_imports(b)


def walk_decls(nodes: list[Node]) -> Generator[ModuleDecl, None, None]:
    for n in nodes:
        if isinstance(n, ModuleDecl):
            yield n
        elif isinstance(n, IfChain):
            for b in _all_branches(n):
                yield from walk_decls(b)


def walk_defines(nodes: list[Node]) -> Generator[Define, None, None]:
    for n in nodes:
        if isinstance(n, Define):
            yield n
        elif isinstance(n, IfChain):
            for b in _all_branches(n):
                yield from walk_defines(b)


def _pp_module(nodes: list[Node]) -> str:
    """Read RAWR_PP_MODULE from a RAWRSCAN_METADATA conditional block."""
    for n in nodes:
        if isinstance(n, IfChain):
            if n.if_node.condition.strip() == 'RAWRSCAN_METADATA':
                return next(
                    (
                        d.body.strip()
                        for d in walk_defines(n.if_node.inner)
                        if d.name == 'RAWR_PP_MODULE'
                    ),
                    '',
                )

            for b in _all_branches(n):
                module = _pp_module(b)
                if module:
                    return module

    return ''


def eval_branch(nodes: list[Node], defines: dict[str, bool]) -> list[Node]:
    """Flatten IfChains by evaluating against *defines*.  Unknown → False."""
    out = []

    for n in nodes:
        if not isinstance(n, IfChain):
            out.append(n)
            continue

        branches = [(n.if_node.condition, n.if_node.inner)] + \
                   [(e.condition, e.inner) for e in n.elif_nodes]

        for cond, inner in branches:
            if eval_condition(cond, defines):
                out.extend(eval_branch(inner, defines)); break
        else:
            if n.else_node:
                out.extend(eval_branch(n.else_node.inner, defines))

    return out


# ── Directory scan ────────────────────────────────────────────────────────────

def _scan_entries(
    paths: list[str],
    root_abs: str,
    extensions: tuple[str, ...],
) -> Generator[tuple[str, str, int, int], None, None]:
    """Yield (display, abs_path, size, mtime_ns) via os.scandir (stat free)."""
    prefix = root_abs + os.sep

    def display(p: str) -> str:
        return p[len(prefix):].replace(os.sep, '/') if p.startswith(prefix) else p

    def walk(d: str) -> Generator:
        try:
            with os.scandir(d) as it:
                entries = sorted(it, key=lambda e: e.name)
        except OSError:
            return

        for e in entries:
            if e.is_dir(follow_symlinks=False):
                yield from walk(e.path)
            elif e.is_file(follow_symlinks=False) and any(e.name.endswith(x) for x in extensions):
                st = e.stat()
                yield display(e.path), e.path, st.st_size, st.st_mtime_ns

    for path in paths:
        ap = os.path.abspath(path)

        if os.path.isdir(ap):
            yield from walk(ap)
        elif os.path.isfile(ap):
            st = os.stat(ap)
            yield display(ap), ap, st.st_size, st.st_mtime_ns
        else:
            print(f'rawrscan: not found: {path}', file=sys.stderr)


def scan_dir(
    paths: list[str],
    root: str,
    extensions: tuple[str, ...] = ('.cppm', '.pp', '.cpp'),
) -> dict[str, list[Node]]:
    root_abs = os.path.abspath(root)
    headers: dict[str, list[Node]] = {}

    for disp, abs_path, *_ in _scan_entries(paths, root_abs, extensions):
        try:
            headers[disp] = parse_file(abs_path)
        except (OSError, UnicodeDecodeError) as exc:
            print(f'rawrscan: warning: {exc}', file=sys.stderr)

    return headers


def scan_dir_with_lines(
    paths: list[str],
    root: str,
    extensions: tuple[str, ...] = ('.cppm', '.pp', '.cpp'),
) -> dict[str, tuple[list[Node], list[str]]]:
    """Like scan_dir but returns (nodes, lines) per file.  Single read per file."""
    root_abs = os.path.abspath(root)
    result: dict[str, tuple[list[Node], list[str]]] = {}

    for disp, abs_path, *_ in _scan_entries(paths, root_abs, extensions):
        try:
            result[disp] = parse_file_with_lines(abs_path)
        except (OSError, UnicodeDecodeError) as exc:
            print(f'rawrscan: warning: {exc}', file=sys.stderr)

    return result


# ── Manifest ──────────────────────────────────────────────────────────────────

_MANIFEST_VERSION = '# rawr module manifest v5'
_MANIFEST_HEADER  = '# kind|path|module|requires|pp_includes|size|mtime_ns|sha256|root'


def _collect_pp_includes(nodes: list[Node]) -> list[str]:
    """Collect all quoted .pp include paths from all branches of the AST."""
    return sorted({
        inc.path
        for inc in walk_includes(nodes)
        if not inc.angle and inc.path.endswith('.pp')
    })


@dataclass
class ManifestEntry:
    """One row in the topo-sorted manifest.

    kind         'module' | 'pp' | 'cpp'
    key          module name (.cppm) or display path (.pp / .cpp)
    file         display path relative to the scanned root
    module       C++ module name (.cppm) or virtual PP module name (.pp)
    deps         ALL direct module imports (unfiltered; external deps included)
    pp_includes  Quoted .pp files this file directly #includes (all branches).
                 Consumers resolve transitive module deps through these.
    size         file size in bytes  (0 = not computed)
    mtime_ns     modification time   (0 = not computed)
    sha256       SHA-256 hex digest   ('' = not computed)
    root         absolute --root used when this entry was created
    """
    key:         str
    file:        str
    kind:        str
    deps:        list[str]
    pp_includes: list[str] = field(default_factory=list)
    module:      str = ''
    size:        int = 0
    mtime_ns:    int = 0
    sha256:      str = ''
    root:        str = ''


def load_manifest_file(path: str) -> dict[str, ManifestEntry]:
    """Read manifest file; return {display_path: ManifestEntry}.

    Handles v5 (9 cols), v4 (8 cols), v3 (7 cols).
    """
    cache: dict[str, ManifestEntry] = {}

    try:
        with open(path, encoding='utf-8') as f:
            for line in f:
                line = line.rstrip('\n')

                if not line or line.startswith('#'):
                    continue

                parts = line.split('|')
                n = len(parts)

                if n not in (7, 8, 9):
                    continue

                kind  = parts[0]
                fpath = parts[1]
                module = parts[2]
                requires = parts[3]

                if n == 9:
                    pp_incs = parts[4]
                    size_s, mtime_s, sha, root_s = parts[5], parts[6], parts[7], parts[8]
                elif n == 8:
                    pp_incs = ''
                    size_s, mtime_s, sha, root_s = parts[4], parts[5], parts[6], parts[7]
                else:
                    pp_incs = root_s = ''
                    size_s, mtime_s, sha = parts[4], parts[5], parts[6]

                deps = [d for d in requires.split(',') if d]
                pp_list = [p for p in pp_incs.split(',') if p]
                key = module if (kind == 'module' and module) else fpath

                try:
                    size_i  = int(size_s)
                    mtime_i = int(mtime_s)
                except ValueError:
                    size_i = mtime_i = 0

                cache[fpath] = ManifestEntry(
                    key=key,
                    file=fpath,
                    kind=kind,
                    deps=deps,
                    pp_includes=pp_list,
                    module=module,
                    size=size_i,
                    mtime_ns=mtime_i,
                    sha256=sha,
                    root=root_s,
                )

    except FileNotFoundError:
        pass

    return cache


def write_manifest_file(path: str, entries: list[ManifestEntry]) -> None:
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)

    with open(path, 'w', encoding='utf-8') as f:
        f.write(_MANIFEST_VERSION + '\n')
        f.write(_MANIFEST_HEADER  + '\n')

        for e in entries:
            module   = e.module
            requires = ','.join(e.deps)
            pp_incs  = ','.join(e.pp_includes)

            f.write(
                f'{e.kind}|{e.file}|{module}|{requires}|{pp_incs}'
                f'|{e.size}|{e.mtime_ns}|{e.sha256}|{e.root}\n'
            )


def _topo_sort(entry_map: dict[str, ManifestEntry]) -> list[ManifestEntry]:
    ts: graphlib.TopologicalSorter = graphlib.TopologicalSorter()

    #
    # PP entries are keyed by their full display path, but pp_includes may
    # contain either the full display path or the short include path.
    #
    # For example:
    #
    #   include/cppm/rawr/detection/pp
    #
    # and:
    #
    #   rawr/detection/pp
    #
    # Both resolve to the same PP entry.
    #
    pp_keys: dict[str, str] = {}

    for k, e in entry_map.items():
        if e.kind != 'pp':
            continue

        pp_keys[e.file] = k

        prefix = 'include/cppm/'
        if e.file.startswith(prefix):
            pp_keys[e.file[len(prefix):]] = k

    for k, e in entry_map.items():
        deps = {
            d
            for d in e.deps
            if d in entry_map
        }

        deps.update(
            pp_keys[p]
            for p in e.pp_includes
            if p in pp_keys
        )

        ts.add(k, *deps)

    try:
        order = list(ts.static_order())
    except graphlib.CycleError as exc:
        print(f'rawrscan: warning: dependency cycle: {exc}', file=sys.stderr)
        order = sorted(entry_map)

    return [entry_map[k] for k in order if k in entry_map]


def manifest_entries(headers: dict[str, list[Node]]) -> list[ManifestEntry]:
    """Build manifest from already-parsed headers (no caching, no stats).

    Used by rawrgen which has already scanned via scan_dir_with_lines.
    """
    entry_map: dict[str, ManifestEntry] = {}

    for disp, nodes in sorted(headers.items()):
        deps        = sorted({imp.name for imp in walk_imports(nodes)})
        pp_includes = _collect_pp_includes(nodes)

        if disp.endswith('.cppm'):
            decl = next(walk_decls(nodes), None)

            if decl is None:
                print(
                    f'rawrscan: warning: {disp}: no export module; skipped',
                    file=sys.stderr,
                )
                continue

            key, kind = decl.name, 'module'
            module = decl.name

        elif disp.endswith('.cpp'):
            key, kind = disp, 'cpp'
            module = ''

        else:
            key, kind = disp, 'pp'
            module = _pp_module(nodes)

        entry_map[key] = ManifestEntry(
            key=key,
            file=disp,
            kind=kind,
            deps=deps,
            pp_includes=pp_includes,
            module=module,
        )

    return _topo_sort(entry_map)


def make_manifest(
    paths: list[str],
    root: str,
    extensions: tuple[str, ...] = ('.cppm', '.pp', '.cpp'),
    cache_file: str | None      = None,
) -> list[ManifestEntry]:
    """Discover, cache, and topo-sort all entries.

    Staleness: entries from the current root whose file no longer exists are
    dropped.  Entries from other roots are preserved (multi-root manifest).

    Single binary read per cache-miss file: hash + parse in one open().
    """
    root_abs = os.path.abspath(root)
    cache   = load_manifest_file(cache_file) if cache_file else {}

    entry_map: dict[str, ManifestEntry] = {}
    scanned:   set[str] = set()

    for disp, abs_path, size, mtime_ns in _scan_entries(paths, root_abs, extensions):
        scanned.add(disp)

        cached = cache.get(disp)

        if cached and cached.size == size and cached.mtime_ns == mtime_ns:
            entry_map[cached.key] = cached
            continue

        try:
            with open(abs_path, 'rb') as f:
                raw = f.read()

            sha   = hashlib.sha256(raw).hexdigest()
            nodes = parse_content(raw.decode('utf-8'))

        except (OSError, UnicodeDecodeError) as exc:
            print(f'rawrscan: warning: {exc}', file=sys.stderr)
            continue

        deps        = sorted({imp.name for imp in walk_imports(nodes)})
        pp_includes = _collect_pp_includes(nodes)

        if disp.endswith('.cppm'):
            decl = next(walk_decls(nodes), None)

            if decl is None:
                print(
                    f'rawrscan: warning: {disp}: no export module; skipped',
                    file=sys.stderr,
                )
                continue

            key, kind = decl.name, 'module'
            module = decl.name

        elif disp.endswith('.cpp'):
            key, kind = disp, 'cpp'
            module = ''

        else:
            key, kind = disp, 'pp'
            module = _pp_module(nodes)

        entry_map[key] = ManifestEntry(
            key=key,
            file=disp,
            kind=kind,
            deps=deps,
            pp_includes=pp_includes,
            module=module,
            size=size,
            mtime_ns=mtime_ns,
            sha256=sha,
            root=root_abs,
        )

    # Preserve entries from other roots; drop stale entries from our root.
    for cached in cache.values():
        if cached.key in entry_map:
            continue

        is_ours = (cached.root == root_abs or cached.root == '')

        if not is_ours:
            entry_map[cached.key] = cached

        # else: from our root but not found → file was deleted/renamed → drop

    entries = _topo_sort(entry_map)

    if cache_file:
        write_manifest_file(cache_file, entries)

    return entries


# ── Serialisation ─────────────────────────────────────────────────────────────

def _to_dict(n: Node) -> dict:
    if isinstance(n, Include):
        return {'t':'inc','path':n.path,'angle':n.angle,'line':n.line}

    if isinstance(n, ModuleDecl):
        return {'t':'decl','name':n.name,'line':n.line}

    if isinstance(n, ModuleImport):
        return {'t':'imp','name':n.name,'re_export':n.re_export,'line':n.line}

    if isinstance(n, Define):
        return {'t':'def','name':n.name,'body':n.body,'line':n.line}

    if isinstance(n, PragmaOnce):
        return {'t':'once','line':n.line}

    if isinstance(n, LineDir):
        return {'t':'ldir','target':n.target_line,'file':n.file,'line':n.line}

    if isinstance(n, IfChain):
        return {
            't': 'chain', 'end_line': n.end_line,
            'if':   {'cond': n.if_node.condition,  'line': n.if_node.line,
                     'inner': [_to_dict(x) for x in n.if_node.inner]},
            'elifs': [{'cond': e.condition, 'line': e.line,
                      'inner': [_to_dict(x) for x in e.inner]} for e in n.elif_nodes],
            'else':  [_to_dict(x) for x in n.else_node.inner] if n.else_node else None,
        }

    return {}


def to_json(nodes: list[Node]) -> list:
    return [_to_dict(n) for n in nodes]


def _pp_lines(nodes: list[Node], depth: int = 0) -> list[str]:
    pad, out = '  ' * depth, []

    for n in nodes:
        if isinstance(n, Include):
            d = '<>' if n.angle else '""'
            out.append(f'{pad}Include({d[0]}{n.path}{d[1]})  @{n.line}')

        elif isinstance(n, ModuleDecl):
            out.append(f'{pad}ModuleDecl({n.name})  @{n.line}')

        elif isinstance(n, ModuleImport):
            out.append(
                f'{pad}ModuleImport({"export " if n.re_export else ""}{n.name})  @{n.line}'
            )

        elif isinstance(n, Define):
            out.append(
                f'{pad}Define({n.name}{" " + n.body if n.body else ""})  @{n.line}'
            )

        elif isinstance(n, PragmaOnce):
            out.append(f'{pad}PragmaOnce  @{n.line}')

        elif isinstance(n, LineDir):
            out.append(f'{pad}LineDir({n.target_line}, "{n.file}")  @{n.line}')

        elif isinstance(n, IfChain):
            out.append(f'{pad}If({n.if_node.condition})  @{n.if_node.line}')
            out.extend(_pp_lines(n.if_node.inner, depth + 1))

            for e in n.elif_nodes:
                out.append(f'{pad}ElseIf({e.condition})  @{e.line}')
                out.extend(_pp_lines(e.inner, depth + 1))

            if n.else_node:
                out.append(f'{pad}Else  @{n.else_node.line}')
                out.extend(_pp_lines(n.else_node.inner, depth + 1))

    return out


def pretty(nodes: list[Node]) -> str:
    return '\n'.join(_pp_lines(nodes))


# ── CLI ───────────────────────────────────────────────────────────────────────

def main() -> int:
    ap = argparse.ArgumentParser(
        prog='rawrscan',
        description='Structural module + preprocessor scanner for rawr .cppm / .pp / .cpp files.',
    )

    ap.add_argument('files', nargs='+', metavar='FILE_OR_DIR')

    ap.add_argument(
        '--root', '-R', required=True, metavar='DIR',
        help='source root for display paths',
    )

    ap.add_argument(
        '--manifest',
        action='store_true',
        help='emit topo-sorted manifest',
    )

    ap.add_argument(
        '--manifest-file',
        metavar='FILE',
        default='',
        help='read/update/write manifest cache (implies --manifest)',
    )

    ap.add_argument(
        '--json',
        action='store_true',
        help='JSON output',
    )

    args = ap.parse_args()

    if args.manifest_file:
        args.manifest = True

    try:
        if args.manifest:
            entries = make_manifest(
                args.files,
                args.root,
                cache_file=args.manifest_file or None,
            )

            if not entries:
                print('rawrscan: no files found', file=sys.stderr)
                return 1

            if args.json:
                print(
                    json.dumps(
                        [
                            {
                                'kind': e.kind,
                                'key': e.key,
                                'file': e.file,
                                'module': e.module,
                                'deps': e.deps,
                                'pp_includes': e.pp_includes,
                                'size': e.size,
                                'mtime_ns': e.mtime_ns,
                                'sha256': e.sha256,
                                'root': e.root,
                            }
                            for e in entries
                        ],
                        indent=2,
                    )
                )

            else:
                for e in entries:
                    module = e.module
                    print(
                        f'{e.kind}|{e.file}|{module}|{",".join(e.deps)}'
                        f'|{",".join(e.pp_includes)}'
                        f'|{e.size}|{e.mtime_ns}|{e.sha256}|{e.root}'
                    )

            return 0

        headers = scan_dir(args.files, args.root)

        if not headers:
            print('rawrscan: no files found', file=sys.stderr)
            return 1

        if args.json:
            print(
                json.dumps(
                    {
                        p: to_json(n)
                        for p, n in sorted(headers.items())
                    },
                    indent=2,
                )
            )

        else:
            multi = len(headers) > 1

            for path, nodes in sorted(headers.items()):
                if multi:
                    print(f'── {path}')

                print(pretty(nodes) or '(empty)')

                if multi:
                    print()

    except (OSError, UnicodeDecodeError) as exc:
        print(f'rawrscan: {exc}', file=sys.stderr)
        return 1

    return 0


if __name__ == '__main__':
    raise SystemExit(main())
