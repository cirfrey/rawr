#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


IGNORE = "//RAWR_AMALGAM_IGNORE "

INCLUDE_RE = re.compile(
    r'^(?P<indent>[ \t]*)#[ \t]*include[ \t]*'
    r'(?P<open>[<"])(?P<path>[^>"]+)[>"]'
    r'(?P<tail>[ \t]*(?://.*)?)$'
)

PRAGMA_ONCE_RE = re.compile(
    r'^(?P<indent>[ \t]*)#[ \t]*pragma[ \t]+once'
    r'(?P<tail>[ \t]*(?://.*)?)$'
)

EXPORT_MODULE_RE = re.compile(
    r'^(?P<indent>[ \t]*)export[ \t]+module[ \t]+'
    r'[A-Za-z_][A-Za-z0-9_.]*[ \t]*;'
    r'[ \t]*(?://.*)?$'
)


class Amalgamator:
    def __init__(self, roots: list[Path]) -> None:
        self.roots = [
            root.resolve()
            for root in roots
        ]

        # Every file reachable from the root.
        self.files: set[Path] = set()

        # file -> direct dependencies.
        self.dependencies: dict[Path, list[Path]] = {}

        # dependency -> direct includers.
        self.required_by: dict[Path, set[Path]] = {}

        # Files currently being discovered.
        self.active: set[Path] = set()

        # Files already emitted.
        self.emitted: set[Path] = set()

        self.output: list[str] = []

    def find_include(self, include: str) -> Path | None:
        """
        Search the supplied include paths in order.

        Deliberately does not attempt to emulate C++'s complete
        include-search semantics.
        """
        for root in self.roots:
            path = (root / include).resolve()

            if path.is_file():
                return path

        return None

    def warn_missing(
        self,
        including: Path,
        include: str,
    ) -> None:
        print(
            f"rawr-amalgam: warning: "
            f"cannot find '{include}' included by '{including}'",
            file=sys.stderr,
        )

    def display_path(self, path: Path) -> str:
        """
        Return a useful path for generated metadata.

        Paths inside the first include root are displayed relative
        to that root.
        """
        try:
            return path.relative_to(
                self.roots[0]
            ).as_posix()
        except ValueError:
            return path.as_posix()

    def read(self, path: Path) -> str:
        try:
            with path.open(
                "r",
                encoding="utf-8",
                newline="",
            ) as file:
                return file.read()

        except OSError as exc:
            raise RuntimeError(
                f"cannot read '{path}': {exc}"
            ) from exc

    def discover(self, path: Path) -> None:
        """
        Discover the complete reachable include graph.

        Nothing is emitted here.

        This separation is important because required_by information
        must be complete before any file is written.
        """
        path = path.resolve()

        if path in self.files:
            return

        if path in self.active:
            raise RuntimeError(
                f"include cycle detected involving '{path}'"
            )

        self.active.add(path)

        try:
            text = self.read(path)

            dependencies: list[Path] = []
            seen: set[Path] = set()

            for line in text.splitlines():
                match = INCLUDE_RE.match(line)

                if not match:
                    continue

                include = match.group("path")
                dependency = self.find_include(include)

                if dependency is None:
                    self.warn_missing(
                        path,
                        include,
                    )
                    continue

                dependency = dependency.resolve()

                # Keep each dependency once in this file's graph.
                if dependency in seen:
                    continue

                seen.add(dependency)
                dependencies.append(dependency)

                # Build the reverse relationship immediately.
                self.required_by.setdefault(
                    dependency,
                    set(),
                ).add(path)

            self.dependencies[path] = dependencies
            self.files.add(path)

            # Continue discovering the complete graph.
            for dependency in dependencies:
                self.discover(dependency)

        finally:
            self.active.remove(path)

    def comment_directive(self, line: str) -> str:
        """
        Prefix a source directive with the reversible amalgamation
        marker while preserving indentation and line ending.

            #include "foo.hpp"

        becomes:

            //RAWR_AMALGAM_IGNORE #include "foo.hpp"
        """
        match = re.match(
            r'^(?P<indent>[ \t]*)'
            r'(?P<directive>#.*?|\bexport[ \t]+module\b.*)'
            r'(?P<newline>\r\n|\r|\n)?$',
            line,
        )

        if not match:
            return line

        return (
            match.group("indent")
            + IGNORE
            + match.group("directive")
            + (match.group("newline") or "")
        )

    def emit_required_by(self, path: Path) -> None:
        required_by = self.required_by.get(path)

        if not required_by:
            return

        self.output.append(
            "/* required by:\n"
        )

        for requirer in sorted(
            required_by,
            key=self.display_path,
        ):
            self.output.append(
                f"\t- {self.display_path(requirer)}\n"
            )

        self.output.append(
            "*/\n"
        )

    def emit_line_directive(self, path: Path) -> None:
        """
        Emit a source mapping directive for the beginning of a file.

        RAWR_AMALGAM_SOURCE_MAPPING may be defined to 0 by the
        consumer to disable amalgamator-generated #line directives.
        """
        display = self.display_path(path)

        self.output.append(
            "#if RAWR_AMALGAM_SOURCE_MAPPING\n"
            f'    #line 0 "{display}"\n'
            "#endif\n"
        )

    def emit(self, path: Path) -> None:
        """
        Emit a previously discovered file in dependency-first order.
        """
        path = path.resolve()

        if path in self.emitted:
            return

        # Dependencies are emitted before their includer.
        for dependency in self.dependencies.get(path, ()):
            self.emit(dependency)

        display = self.display_path(path)

        self.emit_required_by(path)

        self.output.append(
            f'#pragma region "{display}"\n'
        )

        self.emit_line_directive(path)

        text = self.read(path)

        # Preserve source line endings.
        for line in text.splitlines(keepends=True):
            body = line.rstrip("\r\n")

            # Keep source visually nested inside its region.
            indented = "\t" + line

            if INCLUDE_RE.match(body):
                include = INCLUDE_RE.match(body)
                assert include is not None

                dependency = self.find_include(
                    include.group("path")
                )

                if dependency is None:
                    # Unresolved include: leave untouched.
                    self.output.append(indented)
                else:
                    # Dependency has already been emitted.
                    self.output.append(
                        self.comment_directive(indented)
                    )

                continue

            if PRAGMA_ONCE_RE.match(body):
                self.output.append(
                    self.comment_directive(indented)
                )
                continue

            if EXPORT_MODULE_RE.match(body):
                self.output.append(
                    self.comment_directive(indented)
                )
                continue

            # Everything else passes through unchanged.
            self.output.append(indented)

        # If the source doesn't end in a newline, provide one so the
        # generated region delimiter is separated from its contents.
        if text and not text.endswith(("\n", "\r")):
            self.output.append("\n")

        self.output.append(
            "\n"
            f'#pragma endregion "{display}"\n'
            "\n"
        )

        self.emitted.add(path)

    def generate(self, root: Path) -> str:
        self.output = [
            "#pragma region rawr-amalgam\n",
            "// Generated by rawr-amalgam. Do not edit.\n",
            "//\n",
            "// This is a header-mode amalgamation of rawr.\n",
            "//\n",
            "// Define RAWR_AMALGAM_SOURCE_MAPPING=0 to disable source mapping.\n",
            "//\n",
            "// To restore amalgamated #include, #pragma once, and\n",
            "// export module directives, remove the prefix\n",
            "// //RAWR_AMALGAM_IGNORE from those lines.\n",
            "\n",

            "#ifndef RAWR_AMALGAM_SOURCE_MAPPING\n",
            "    #define RAWR_AMALGAM_SOURCE_MAPPING 1\n",
            "#endif\n",
            "\n",

            "// Amalgams are headers; never allow module mode to leak in.\n",
            "#ifdef RAWR_MODULE\n",
            "    #undef RAWR_MODULE\n",
            "#endif\n",
            "\n",
        ]

        root = root.resolve()

        # Phase 1: discover the complete include graph.
        self.discover(root)

        # Phase 2: emit dependency-first.
        self.emit(root)

        self.output.append(
            "#pragma endregion rawr-amalgam\n"
        )

        return "".join(self.output)


def main() -> int:
    parser = argparse.ArgumentParser(
        prog="rawr-amalgam",
        description="Generate a reversible rawr amalgamation.",
    )

    parser.add_argument(
        "-I",
        "--include-path",
        dest="include_paths",
        action="append",
        type=Path,
        required=True,
        help="directory to search for includes",
    )

    parser.add_argument(
        "input",
        type=Path,
        help="root header to amalgamate",
    )

    parser.add_argument(
        "output",
        type=Path,
        help="output amalgamated header",
    )

    args = parser.parse_args()

    try:
        amalgamator = Amalgamator(
            args.include_paths
        )

        result = amalgamator.generate(
            args.input
        )

        args.output.parent.mkdir(
            parents=True,
            exist_ok=True,
        )

        with args.output.open(
            "w",
            encoding="utf-8",
            newline="",
        ) as file:
            file.write(result)

    except RuntimeError as exc:
        print(
            f"rawr-amalgam: error: {exc}",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
