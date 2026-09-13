#!/usr/bin/env python3
"""
rawr-testdeps
=============

Emit the set of rawr modules a test source (transitively) imports.

Under RAWR_PP_TRANSITIVE_AS_MODULE, .pp files carry `import rawr.X;` inside
their module-mode block. Test .cpp files include .pp files (directly or via
other .pp files). This script walks that include graph and emits every
discovered module name.

The Meson build maps each name to its declare_dependency. Transitive
dependencies are resolved by Meson, so the scanner only needs direct imports.

Usage:
    testdeps.py --root include/ test.cpp [more.cpp ...]

Output:
    One module name per line, sorted.
"""

from __future__ import annotations

import argparse
import os
import re

IMPORT_RE     = re.compile(r'^\s*import\s+(rawr(?:\.[A-Za-z_][A-Za-z0-9_]*)+)\s*;')
INCLUDE_PP_RE = re.compile(r'^\s*#\s*include\s+"([^"]+\.pp)"')


def scan(path: str, root: str, visited: set[str], modules: set[str]) -> None:
    if path in visited:
        return
    visited.add(path)

    try:
        with open(path, "r", encoding="utf-8") as f:
            lines = f.readlines()
    except OSError:
        return

    for line in lines:
        m = IMPORT_RE.match(line)
        if m:
            modules.add(m.group(1))
            continue

        m = INCLUDE_PP_RE.match(line)
        if m:
            include = m.group(1)
            if not include.startswith("rawr/"):
                continue
            target = os.path.join(root, include)
            if os.path.isfile(target):
                scan(target, root, visited, modules)


def main() -> int:
    parser = argparse.ArgumentParser(prog="rawr-testdeps")
    parser.add_argument("--root", required=True, help="rawr include root")
    parser.add_argument("sources", nargs="+")
    args = parser.parse_args()

    root = os.path.abspath(args.root)
    visited: set[str] = set()
    modules: set[str] = set()

    for src in args.sources:
        scan(os.path.abspath(src), root, visited, modules)

    for name in sorted(modules):
        print(name)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
