#!/usr/bin/env python3
"""
rawrgen — generate header-mode distributions from canonical .cppm / .pp sources.

Subcommands
-----------

  replace-imports  FILES --when GLOB [--out DIR] [--outname TPL] [--root DIR]

      Replace 'import X;' lines whose module name matches GLOB with the
      equivalent '#include "X/path.hpp"'.  Pure text; no rawrscan needed.

  replace-includes FILES --when GLOB --with NAME [--out DIR] [--outname TPL] [--root DIR]

      Replace '#include "..."' lines whose include path matches GLOB with
      '#include "NAME"'.  Duplicate replacements are fine when NAME has
      #pragma once.  Pure text; no rawrscan needed.

  demodularize     FILES [--out DIR] [--root DIR]

      .cppm  →  {out}/<rel>.hpp
      .pp    →  {out}/<rel>.pp          (#pragma once + redirect to .local.pp)
               {out}/<rel>.local.pp    (imports→includes, re-includable)
               {out}/<rel>.undef.pp    (FILO macro cleanup, from rawrscan AST)

  amalgamate       FILES --outname NAME [--out DIR] [--root DIR]

      Splice all .cppm and .pp sources (topo-sorted) into a single header
      with #line markers pointing at the would-be .hpp paths.

--when GLOB   fnmatch pattern applied to the module name (replace-imports) or
              include path (replace-includes).
              Examples: "rawr.*"  "rawr/*"  "*.hpp"  "*"

--outname TPL  Output filename template: {name} {stem} {ext}.
               For amalgamate: the fixed output filename.

--root DIR    Base for display-path computation.  Defaults to the input
              directory itself per input path, enabling multi-root invocations:
                  rawrgen demodularize src/ include/cppm/ --out include/hpp/

Typical workflows
-----------------

  # Library hpp + amalgam distributions
  rawrgen demodularize src/ include/cppm/ --out include/hpp/
  rawrgen amalgamate   src/ include/cppm/ --out include/amalgam/ --outname rawr.amalgam.hpp

  # Test variants from canonical module-import test files
  rawrgen replace-imports  tests/unit/ \\
          --when "rawr.*" --out tests/gen/header/
  rawrgen replace-includes tests/gen/header/ \\
          --when "rawr/*" --with rawr.amalgam.hpp --out tests/gen/amalgam/

  The two replace steps convert:
    import rawr.lib.test;       →  #include "rawr/lib/test.hpp"   (step 1)
                                →  #include "rawr.amalgam.hpp"    (step 2)
    #include "rawr/lib/main.pp" →  (unchanged in step 1)
                                →  #include "rawr.amalgam.hpp"    (step 2)
  Duplicates are harmless — the amalgam has #pragma once.
"""
from __future__ import annotations

import argparse
import fnmatch
import os
import re
import sys
from dataclasses import dataclass

from rawrscan import (
    Define, ElseNode, IfChain, IfNode, Include,
    ManifestEntry, manifest_entries, scan_dir_with_lines,
)


# ── Shared patterns ───────────────────────────────────────────────────────────

_IMPORT      = re.compile(r'^(?P<ind>\s*)(?:export\s+)?import\s+(?P<name>[\w.]+)\s*;')
_ANY_INC     = re.compile(r'^(?P<ind>\s*)#\s*include\s*[<"](?P<path>[^>"]+)[>"]')
_INC_PP      = re.compile(r'^(?P<ind>\s*)#\s*include\s*"(?P<path>[^"]+\.pp)"')
_EXPORT_MOD  = re.compile(r'^\s*export\s+module\s+[\w.]+(?:\s*:\s*[\w.]+)?\s*;')
_EXPORT_COMP = re.compile(r'^(?P<ind>\s*)export\s+(?P<kw>namespace|extern)\b(?P<rest>.*)$')
_EXPORT_BLK  = re.compile(r'^\s*export\s*\{')


def _mod_to_include(name: str) -> str:
    return name.replace('.', '/') + '.hpp'


# ── Shared helpers ────────────────────────────────────────────────────────────

def _infer_root(path: str) -> str:
    """For a directory input: use it directly.  For a file: use its parent."""
    ap = os.path.abspath(path)
    return ap if os.path.isdir(ap) else os.path.dirname(ap)


def _find_files(paths: list[str], root: str) -> list[tuple[str, str]]:
    """Return [(display_path, abs_path)] for all files under *paths*.
    display_path is relative to *root* with forward slashes."""
    root_abs = os.path.abspath(root)
    prefix   = root_abs + os.sep
    results: list[tuple[str, str]] = []

    def disp(p: str) -> str:
        return p[len(prefix):].replace(os.sep, '/') if p.startswith(prefix) else os.path.basename(p)

    for path in paths:
        ap = os.path.abspath(path)
        if os.path.isfile(ap):
            results.append((disp(ap), ap))
        elif os.path.isdir(ap):
            for dp, dns, fns in os.walk(ap):
                dns.sort()
                for fn in sorted(fns):
                    results.append((disp(os.path.join(dp, fn)), os.path.join(dp, fn)))
        else:
            print(f'rawrgen: not found: {path}', file=sys.stderr)
    return results


def apply_outname(filename: str, template: str) -> str:
    """Apply an --outname template to a filename.
    Variables: {name} (full basename), {stem} (no last ext), {ext} (last ext)."""
    if not template:
        return filename
    stem, ext = os.path.splitext(filename)
    return template.format(name=filename, stem=stem, ext=ext)


def _out_path(out_root: str, display: str, outname: str = '') -> str:
    dir_part  = os.path.dirname(display)
    base_part = apply_outname(os.path.basename(display), outname)
    return os.path.join(out_root, dir_part, base_part)


def _read(path: str) -> list[str]:
    with open(path, 'r', encoding='utf-8', newline='') as f:
        return f.readlines()


def _write(path: str, content: str, dry_run: bool) -> None:
    if dry_run:
        print(f'  {os.path.normpath(path)}  ({len(content):,} bytes)')
        return
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)


# ── Text command: replace-imports ─────────────────────────────────────────────

def _do_replace_imports(lines: list[str], pattern: str) -> list[str]:
    """Replace import lines matching *pattern* (fnmatch on module name)."""
    out = []
    for line in lines:
        m = _IMPORT.match(line)
        if m and fnmatch.fnmatch(m['name'], pattern):
            out.append(f'{m["ind"]}#include "{_mod_to_include(m["name"])}"\n')
        else:
            out.append(line)
    return out


def cmd_replace_imports(args) -> int:
    """replace-imports: import X; → #include "X/path.hpp" for matching imports."""
    out_root = os.path.abspath(args.out)
    written  = 0

    for inp in args.files:
        root  = os.path.abspath(args.root) if args.root else _infer_root(inp)
        files = _find_files([inp], root)
        for display, abs_path in files:
            lines   = _read(abs_path)
            content = ''.join(_do_replace_imports(lines, args.when))
            dst     = _out_path(out_root, display, args.outname)
            _write(dst, content, args.dry_run)
            written += 1

    verb = 'Would write' if args.dry_run else 'Wrote'
    print(f'rawrgen replace-imports: {verb} {written} files')
    return 0


# ── Text command: replace-includes ────────────────────────────────────────────

def _do_replace_includes(lines: list[str], pattern: str, replacement: str) -> list[str]:
    """Replace include lines matching *pattern* (fnmatch on include path) with *replacement*."""
    out = []
    for line in lines:
        m = _ANY_INC.match(line)
        if m and fnmatch.fnmatch(m['path'], pattern):
            out.append(f'{m["ind"]}#include "{replacement}"\n')
        else:
            out.append(line)
    return out


def cmd_replace_includes(args) -> int:
    """replace-includes: #include "match" → #include "NAME" for matching includes."""
    out_root = os.path.abspath(args.out)
    written  = 0

    for inp in args.files:
        root  = os.path.abspath(args.root) if args.root else _infer_root(inp)
        files = _find_files([inp], root)
        for display, abs_path in files:
            lines   = _read(abs_path)
            content = ''.join(_do_replace_includes(lines, args.when, args.with_name))
            dst     = _out_path(out_root, display, args.outname)
            _write(dst, content, args.dry_run)
            written += 1

    verb = 'Would write' if args.dry_run else 'Wrote'
    print(f'rawrgen replace-includes: {verb} {written} files')
    return 0


# ── Undo tracking (from rawrscan AST — no scanning in rawrgen) ────────────────

@dataclass
class UndoItem:
    kind:  str   # 'macro' | 'pp'
    value: str

@dataclass
class UndoChain:
    """Reconstructed #if/#elif/#else/#endif for structured undo output.
    Using IfChain's flat elif representation produces readable output
    without deeply nested negated conditions."""
    branches: list[tuple[str | None, list]]  # (condition | None-for-else, items)


def _collect_undo(nodes: list) -> list:
    """Walk rawrscan AST → UndoItem | UndoChain list in source order."""
    items = []
    for n in nodes:
        if isinstance(n, Define):
            items.append(UndoItem('macro', n.name))
        elif isinstance(n, Include) and not n.angle and n.path.endswith('.pp'):
            items.append(UndoItem('pp', n.path))
        elif isinstance(n, IfChain):
            branches = []
            if_items = _collect_undo(n.if_node.inner)
            if if_items: branches.append((n.if_node.condition, if_items))
            for e in n.elif_nodes:
                ei = _collect_undo(e.inner)
                if ei: branches.append((e.condition, ei))
            if n.else_node:
                el = _collect_undo(n.else_node.inner)
                if el: branches.append((None, el))
            if branches:
                items.append(UndoChain(branches))
    return items


def _emit_undo(items: list, indent: str = '') -> list[str]:
    """Emit undo items in FILO order.

    UndoChain uses the original #if/#elif/#else/#endif structure from IfChain,
    giving clean output rather than deeply nested negated conditions.
    """
    out: list[str] = []
    for item in reversed(items):
        if isinstance(item, UndoItem):
            if item.kind == 'macro':
                out.append(f'{indent}#undef {item.value}\n')
            else:
                out.append(f'{indent}#include "{item.value[:-3]}.undef.pp"\n')
        elif isinstance(item, UndoChain):
            for i, (cond, branch_items) in enumerate(item.branches):
                if i == 0:       out.append(f'{indent}#if {cond}\n')
                elif cond is None: out.append(f'{indent}#else\n')
                else:            out.append(f'{indent}#elif {cond}\n')
                out.extend(_emit_undo(branch_items, indent + '    '))
            out.append(f'{indent}#endif\n')
    return out


# ── Library transform ─────────────────────────────────────────────────────────

_BANNER = '// GENERATED — do not edit\n'
_UNDO_S = '\n// ── end-of-header scope: undo pp fragments and macros (FILO)\n'


def _transform(lines: list[str], pp_to_local: bool = False) -> list[str]:
    """Apply library substitution rules.  Core of demodularize."""
    out:   list[str] = []
    in_blk: bool     = False
    depth:  int      = 0

    for line in lines:
        s = line.rstrip()
        if in_blk:
            depth += s.count('{') - s.count('}')
            if depth <= 0: in_blk = False
            else:          out.append(line)
            continue
        if _EXPORT_MOD.match(s):  continue
        if m := _IMPORT.match(s):
            out.append(f'{m["ind"]}#include "{_mod_to_include(m["name"])}"\n'); continue
        if m := _EXPORT_COMP.match(s):
            out.append(f'{m["ind"]}{m["kw"]}{m["rest"]}\n'); continue
        if _EXPORT_BLK.match(s):
            rm = re.match(r'^\s*export\s*\{(.*)', s)
            rest = rm.group(1) if rm else ''
            depth = 1 + rest.count('{') - rest.count('}')
            if rest.strip():
                if depth <= 0:
                    inner = rest.rsplit('}', 1)[0].strip()
                    if inner: out.append(f'{inner}\n')
                else:
                    out.append(f'{rest}\n'); in_blk = True
            else:
                in_blk = (depth > 0)
            continue
        if pp_to_local:
            if m := _INC_PP.match(s):
                out.append(f'{m["ind"]}#include "{m["path"][:-3]}.local.pp"\n'); continue
        out.append(line)
    return out


def gen_hpp(nodes: list, lines: list[str]) -> str:
    """.cppm → .hpp: strip module decl, imports→#includes, pp→.local.pp, undo at end."""
    body = _transform(lines, pp_to_local=True)
    undo = _emit_undo(_collect_undo(nodes))
    parts: list[str] = [_BANNER, '#pragma once\n', '\n']
    parts.extend(body)
    if undo:
        parts.append(_UNDO_S)
        parts.extend(undo)
    return ''.join(parts)


def gen_hpp_pp(display: str) -> str:
    """.pp → header.pp: just a #pragma once redirect to .local.pp."""
    return f'{_BANNER}#pragma once\n#include "{display[:-3]}.local.pp"\n'


_DEFINE_LINE = re.compile(r'^(?P<ind>[ \t]*)#\s*define\s+(?P<name>[A-Za-z_]\w*)')
_UNDEF_LINE  = re.compile(r'^[ \t]*#\s*undef\s+(?P<name>[A-Za-z_]\w*)')


def gen_local_pp(nodes: list, lines: list[str]) -> str:
    """.pp → header.local.pp: re-includable, no #pragma once, sub-pp→.local.pp.

    Prepends #undef before each #define so that re-including this file in the
    same TU (which is intentional — .local.pp has no pragma once) silently
    resets macros rather than producing -Wmacro-redefined warnings.

    Does not emit a duplicate #undef when the immediately preceding non-blank
    line is already '#undef NAME' for the same name — this avoids doubling up
    on the existing pattern:

        #undef  RAWR_IS_POSIX       ← already in source before conditional
        #define RAWR_IS_POSIX 1
    """
    stripped = [l for l in lines if l.rstrip() != '#pragma once']
    body     = _transform(stripped, pp_to_local=True)

    result: list[str]  = []
    last_undef: str | None = None   # name of the most-recent #undef seen

    for line in body:
        s = line.rstrip()

        if m := _UNDEF_LINE.match(s):
            last_undef = m['name']
            result.append(line)
            continue

        if m := _DEFINE_LINE.match(s):
            name = m['name']
            if name != last_undef:
                # No preceding #undef for this name — add one so re-inclusion
                # of this file doesn't produce a redefinition warning.
                result.append(f'{m["ind"]}#undef {name}\n')
            last_undef = None
            result.append(line)
            continue

        # Any non-blank, non-undef, non-define line breaks the undef→define
        # adjacency: reset tracking so we don't accidentally skip an #undef
        # that's separated from its #define by intervening code.
        if s:
            last_undef = None
        result.append(line)

    return ''.join([_BANNER] + result)


def gen_undef_pp(nodes: list) -> str:
    """.pp → header.undef.pp: FILO cleanup derived from rawrscan AST."""
    undo = _emit_undo(_collect_undo(nodes))
    return ''.join([_BANNER] + undo) if undo else f'{_BANNER}// nothing to undo\n'


# ── AST command: demodularize ─────────────────────────────────────────────────

def cmd_demodularize(args) -> int:
    """demodularize: .cppm/.pp → .hpp / .local.pp / .undef.pp"""
    out_root = os.path.abspath(args.out)
    written  = 0

    for inp in args.files:
        root    = os.path.abspath(args.root) if args.root else _infer_root(inp)
        headers = scan_dir_with_lines([inp], root, extensions=('.cppm', '.pp'))
        entries = manifest_entries({dp: nl[0] for dp, nl in headers.items()})

        for e in entries:
            entry = headers.get(e.file)
            if entry is None: continue
            nodes, lines = entry

            if e.file.endswith('.cppm'):
                _write(os.path.join(out_root, e.file[:-5] + '.hpp'),
                       gen_hpp(nodes, lines), args.dry_run)
                written += 1

            elif e.file.endswith('.pp'):
                _write(os.path.join(out_root, e.file),
                       gen_hpp_pp(e.file), args.dry_run)
                _write(os.path.join(out_root, e.file[:-3] + '.local.pp'),
                       gen_local_pp(nodes, lines), args.dry_run)
                _write(os.path.join(out_root, e.file[:-3] + '.undef.pp'),
                       gen_undef_pp(nodes), args.dry_run)
                written += 3

    verb = 'Would write' if args.dry_run else 'Wrote'
    print(f'rawrgen demodularize: {verb} {written} files')
    return 0


# ── AST command: amalgamate ───────────────────────────────────────────────────

def _gen_amalgam(entries: list[ManifestEntry],
                 all_hwl: dict[str, tuple],
                 name:   str) -> str:
    """Splice all sources (topo order) into a single header with #line markers."""
    inlined_pp: set[str] = set()

    def indent(lines: list[str]) -> list[str]:
        return [f'\t{line}' if line.strip() else line for line in lines]

    def inline_pp(pp_display: str) -> list[str]:

        if pp_display in inlined_pp: return []
        inlined_pp.add(pp_display)

        e = all_hwl.get(pp_display)
        if e is None:
            return [f'// [rawrgen: {pp_display} not found]\n']

        _, pp_lines = e
        out = []
        out.extend(indent(_transform(pp_lines, pp_to_local=False)))
        out.append('\n')
        return out

    def body_with_inline(lines: list[str]) -> list[str]:

        out:    list[str] = []
        in_blk: bool      = False
        depth:  int       = 0

        for line in lines:
            s = line.rstrip()

            if in_blk:
                depth += s.count('{') - s.count('}')

                if depth <= 0:
                    in_blk = False
                else:
                    out.append(f'\t{line}')

                continue

            if _EXPORT_MOD.match(s):
                continue

            if m := _IMPORT.match(s):
                out.append(
                    f'\t{m["ind"]}#include "{_mod_to_include(m["name"])}"\n'
                )
                continue

            if m := _EXPORT_COMP.match(s):
                out.append(
                    f'\t{m["ind"]}{m["kw"]}{m["rest"]}\n'
                )
                continue

            if _EXPORT_BLK.match(s):
                rm = re.match(
                    r'^\s*export\s*\{(.*)',
                    s,
                )
                rest = rm.group(1) if rm else ''
                depth = (
                    1
                    + rest.count('{')
                    - rest.count('}')
                )

                if rest.strip():
                    if depth <= 0:
                        inner = rest.rsplit('}', 1)[0].strip()
                        if inner:
                            out.append(f'\t{inner}\n')
                    else:
                        out.append(f'\t{rest}\n')
                        in_blk = True
                else:
                    in_blk = depth > 0

                continue

            if m := _INC_PP.match(s):
                out.extend(inline_pp(m['path']))
                continue

            out.append(f'\t{line}')

        return out

    parts: list[str] = [
        _BANNER,
        f'// {name}\n',
        '#pragma once\n',
        '#define RAWR_AMALGAM 1\n\n',
    ]

    for e in entries:
        entry = all_hwl.get(e.file)

        if entry is None:
            continue

        nodes, lines = entry

        parts.append('#if !RAWR_AMALGAM_NO_SECTIONS\n')
        parts.append(f'\t#pragma section "{e.file}"\n')
        parts.append('#endif\n')

        if e.kind == 'pp':
            parts.append('\t#if !RAWR_AMALGAM_NO_SOURCE_MAPPING\n')
            parts.append(f'\t\t#line 3 "{e.file}"\n')
            parts.append('\t#endif\n')
            parts.extend(inline_pp(e.file))

        elif e.kind == 'module':
            hpp_disp = e.file[:-5] + '.hpp'
            parts.append('\t#if !RAWR_AMALGAM_NO_SOURCE_MAPPING\n')
            parts.append(f'\t\t#line 1 "{hpp_disp}"\n')
            parts.append('\t#endif\n')
            parts.extend(body_with_inline(lines))
            parts.append('\n')

        parts.append('#if !RAWR_AMALGAM_NO_SECTIONS\n')
        parts.append(f'\t#pragma endsection "{e.file}"\n')
        parts.append('#endif\n')

    content = ''.join(parts)
    restore_line = content.count('\n') + 1

    parts.extend([
        '#if !RAWR_AMALGAM_NO_SOURCE_MAPPING\n',
        f'#line {restore_line} "{name}"\n',
        '#endif\n',
    ])

    return ''.join(parts)

def cmd_amalgamate(args) -> int:
    """amalgamate: splice .cppm/.pp sources into a single-file amalgam."""
    out_root = os.path.abspath(args.out)
    all_hwl: dict[str, tuple] = {}
    all_nodes: dict[str, list] = {}

    for inp in args.files:
        root = os.path.abspath(args.root) if args.root else _infer_root(inp)
        hwl  = scan_dir_with_lines([inp], root, extensions=('.cppm', '.pp'))
        all_hwl.update(hwl)
        all_nodes.update({dp: nl[0] for dp, nl in hwl.items()})

    entries = manifest_entries(all_nodes)
    content = _gen_amalgam(entries, all_hwl, args.outname)
    dst     = os.path.join(out_root, args.outname)
    _write(dst, content, args.dry_run)

    verb = 'Would write' if args.dry_run else 'Wrote'
    print(f'rawrgen amalgamate: {verb} {dst}  ({len(content):,} bytes)')
    return 0


# ── CLI ───────────────────────────────────────────────────────────────────────

def main() -> int:
    ap = argparse.ArgumentParser(
        prog='rawrgen',
        description='Generate header-mode distributions from canonical .cppm / .pp sources.',
    )
    sub = ap.add_subparsers(dest='cmd', required=True)

    # ── shared args helper ────────────────────────────────────────────────────
    def add_base(p: argparse.ArgumentParser) -> None:
        p.add_argument('files', nargs='+', metavar='FILE_OR_DIR')
        p.add_argument('--out', default='.', metavar='DIR',
                       help='output root (default: current directory)')
        p.add_argument('--root', default='', metavar='DIR',
                       help='display-path root; default: input dir itself (per input)')
        p.add_argument('--dry-run', action='store_true',
                       help='print what would be written without touching the filesystem')

    # ── replace-imports ───────────────────────────────────────────────────────
    ri = sub.add_parser('replace-imports',
                        help="convert 'import X;' matching a glob to '#include \"X/path.hpp\"'")
    add_base(ri)
    ri.add_argument('--when', required=True, metavar='GLOB',
                    help='fnmatch pattern applied to the module name (e.g. "rawr.*")')
    ri.add_argument('--outname', default='', metavar='TPL',
                    help='output filename template: {name} {stem} {ext}')

    # ── replace-includes ──────────────────────────────────────────────────────
    rin = sub.add_parser('replace-includes',
                         help="replace '#include \"...\"' matching a glob with a fixed name")
    add_base(rin)
    rin.add_argument('--when', required=True, metavar='GLOB',
                     help='fnmatch pattern applied to the include path (e.g. "rawr/*")')
    rin.add_argument('--with', dest='with_name', required=True, metavar='NAME',
                     help='replacement include name (e.g. rawr.amalgam.hpp)')
    rin.add_argument('--outname', default='', metavar='TPL',
                     help='output filename template: {name} {stem} {ext}')

    # ── demodularize ──────────────────────────────────────────────────────────
    dm = sub.add_parser('demodularize',
                        help='.cppm → .hpp;  .pp → .pp + .local.pp + .undef.pp')
    add_base(dm)

    # ── amalgamate ────────────────────────────────────────────────────────────
    ag = sub.add_parser('amalgamate',
                        help='splice .cppm/.pp sources into a single-file amalgam')
    add_base(ag)
    ag.add_argument('--outname', required=True, metavar='NAME',
                    help='amalgam output filename (e.g. rawr.amalgam.hpp)')

    args = ap.parse_args()

    dispatch = {
        'replace-imports':  cmd_replace_imports,
        'replace-includes': cmd_replace_includes,
        'demodularize':     cmd_demodularize,
        'amalgamate':       cmd_amalgamate,
    }
    return dispatch[args.cmd](args)


if __name__ == '__main__':
    raise SystemExit(main())
