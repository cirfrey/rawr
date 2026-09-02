#!/usr/bin/env python3

"""
rawr-amalgam
============

Generate the header-mode amalgamation of rawr.

The amalgamator is intentionally NOT a C++ preprocessor. It performs only
the transformations necessary to turn rawr's normal header tree into one
reversible header:

    1. Internal #include "..." directives:
           - recursively include the referenced rawr header
           - comment out the original include as a reversible breadcrumb

    2. External #include <...> directives:
           - leave completely untouched
           - never add them to the amalgamation dependency graph
           - emit a warning

    3. #pragma once:
           - comment out as a reversible breadcrumb

    4. export module rawr....:
           - comment out as a reversible breadcrumb

    Everything else is preserved.

IMPORTANT INVARIANTS
====================

This tool relies on invariants enforced by the rawr project/build:

    1. Quoted includes are internal rawr includes.

           #include "rawr/foo.hpp"

       always refers to:

           <include-root>/rawr/foo.hpp

       The include spelling is therefore already relative to the supplied
       include root. C++'s normal "search relative to the including file"
       semantics are deliberately NOT implemented.

    2. Angle-bracket includes are external.

           #include <foo.hpp>

       is never amalgamated and is left untouched.

    3. The amalgamation target runs only after the normal rawr library
       successfully builds.

       Therefore a missing quoted include is considered an error in the
       project's source/build invariants.

    4. Every reachable source file is read at most once.

    5. Every reachable source file is emitted at most once.

    6. Include cycles are errors.

    7. Dependency metadata is computed before emission so `required by`
       contains all direct includers.

    8. This tool never evaluates #if/#ifdef/#else/#endif.

       Conditional compilation remains source text. The only module-mode
       protection performed by the amalgamator is commenting out
       `export module ...` directives and undefining `RAWR_MODULE` in the
       generated header.

    9. The amalgam is a header.

       RAWR_MODULE is explicitly undefined at the top of the generated
       file so module mode cannot leak into the amalgam.

   10. Source mapping is controlled by RAWR_AMALGAM_SOURCE_MAPPING and
       defaults to enabled.

PERFORMANCE MODEL
=================

Rawr's quoted include paths are already include-root-relative, so there is
no include search and no filesystem indexing.

Discovery is performed in breadth-first waves. Independent files in the
same wave are read concurrently so filesystem latency can overlap.

A single ThreadPoolExecutor is kept alive for the entire discovery phase;
the pool is not repeatedly created and destroyed for each wave.

The graph is completed before emission, so emission remains deterministic
and dependency-first.
"""

from __future__ import annotations

import argparse
import os
import re
from concurrent.futures import ThreadPoolExecutor


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
    r'(?P<name>[a-zA-Z_][a-zA-Z0-9_.]*)[ \t]*;'
    r'(?P<tail>[ \t]*(?://.*)?)$'
)


def split_newline(line: str) -> tuple[str, str]:
    if line.endswith("\r\n"):
        return line[:-2], "\r\n"

    if line.endswith("\n") or line.endswith("\r"):
        return line[:-1], line[-1]

    return line, ""


def skip_space(
    text: str,
    index: int,
) -> int:
    length = len(text)

    while index < length and text[index] in " \t":
        index += 1

    return index


def parse_include(
    body: str,
) -> tuple[str, str] | None:
    """
    Parse the simple include syntax used by rawr.

    Returns:

        ("<", "foo.hpp")
        ('"', "rawr/foo.hpp")

    or None.
    """
    length = len(body)
    index = skip_space(body, 0)

    if index >= length or body[index] != "#":
        return None

    index += 1
    index = skip_space(body, index)

    keyword = "include"

    if not body.startswith(keyword, index):
        return None

    index += len(keyword)
    index = skip_space(body, index)

    if index >= length:
        return None

    opener = body[index]

    if opener not in '<"':
        return None

    closer = ">" if opener == "<" else '"'

    index += 1
    start = index

    end = body.find(closer, start)

    if end == -1:
        return None

    include = body[start:end]

    index = skip_space(body, end + 1)

    if index < length and not body.startswith("//", index):
        return None

    return opener, include


def is_pragma_once(
    body: str,
) -> bool:
    length = len(body)
    index = skip_space(body, 0)

    if index >= length or body[index] != "#":
        return False

    index += 1
    index = skip_space(body, index)

    keyword = "pragma"

    if not body.startswith(keyword, index):
        return False

    index += len(keyword)
    index = skip_space(body, index)

    keyword = "once"

    if not body.startswith(keyword, index):
        return False

    index += len(keyword)
    index = skip_space(body, index)

    return (
        index == length
        or body.startswith("//", index)
    )


def is_export_module(
    body: str,
) -> bool:
    length = len(body)
    index = skip_space(body, 0)

    keyword = "export"

    if not body.startswith(keyword, index):
        return False

    index += len(keyword)
    index = skip_space(body, index)

    keyword = "module"

    if not body.startswith(keyword, index):
        return False

    index += len(keyword)
    index = skip_space(body, index)

    if index >= length:
        return False

    first = body[index]

    if not (
        first == "_"
        or first.isalpha()
    ):
        return False

    index += 1

    while index < length:
        character = body[index]

        if (
            character == "_"
            or character.isalnum()
            or character == "."
        ):
            index += 1
            continue

        break

    index = skip_space(body, index)

    if index >= length or body[index] != ";":
        return False

    index += 1
    index = skip_space(body, index)

    return (
        index == length
        or body.startswith("//", index)
    )


class Amalgamator:
    def __init__(
        self,
        root: str,
    ) -> None:
        # Rawr quoted includes are root-relative.
        self.root = root.rstrip("/\\")

        # file -> direct dependencies.
        self.dependencies: dict[
            str,
            list[str],
        ] = {}

        # dependency -> direct includers.
        self.required_by: dict[
            str,
            set[str],
        ] = {}

        # filesystem path -> display/include path.
        self.display_paths: dict[
            str,
            str,
        ] = {}

        # filesystem path -> transformed source.
        self.sources: dict[
            str,
            str,
        ] = {}

        # Files already discovered.
        self.files: set[str] = set()

        # Files already emitted.
        self.emitted: set[str] = set()

        self.output: list[str] = []

    def include_path(
        self,
        include: str,
    ) -> str:
        """
        Convert a rawr include spelling into its expected filesystem path.

        Rawr guarantees that quoted includes are relative to the supplied
        include root, so no filesystem search is necessary.
        """
        return (
            self.root
            + "/"
            + include
        )

    def display_path(
        self,
        path: str,
    ) -> str:
        """
        Return the stable rawr include path for a filesystem path.
        """
        display = self.display_paths.get(path)

        if display is not None:
            return display

        prefix = self.root + "/"

        if path.startswith(prefix):
            display = path[len(prefix):]
        else:
            display = path

        self.display_paths[path] = display

        return display

    def warn_external(
        self,
        including: str,
        include: str,
    ) -> None:
        print(
            f"rawr-amalgam: warning: "
            f"external include '<{include}>' "
            f"left untouched in "
            f"'{self.display_path(including)}'",
        )

    def read(
        self,
        path: str,
    ) -> str:
        """
        Read the complete file as bytes, then decode once.
        """
        try:
            with open(
                path,
                "rb",
            ) as file:
                return file.read().decode(
                    "utf-8"
                )

        except OSError as exc:
            raise RuntimeError(
                f"cannot read "
                f"'{self.display_path(path)}': "
                f"{exc}"
            ) from exc

        except UnicodeDecodeError as exc:
            raise RuntimeError(
                f"cannot decode "
                f"'{self.display_path(path)}' "
                f"as UTF-8: {exc}"
            ) from exc

    @staticmethod
    def comment_directive(
        line: str,
    ) -> str:
        body, newline = split_newline(line)

        index = skip_space(body, 0)

        return (
            body[:index]
            + IGNORE
            + body[index:]
            + newline
        )

    def transform(
        self,
        path: str,
        text: str,
    ) -> list[str]:
        """
        Parse and transform one source file exactly once.

        Returns the direct dependency paths.
        """
        dependencies: list[str] = []
        seen: set[str] = set()
        output: list[str] = []

        for source_line in text.splitlines(
            keepends=True
        ):
            body, _ = split_newline(
                source_line
            )

            include = parse_include(
                body
            )

            if include is not None:
                kind, include_name = include

                # External dependency.
                if kind == "<":
                    self.warn_external(
                        path,
                        include_name,
                    )

                    output.append(
                        "\t" + source_line
                    )
                    continue

                # Internal rawr dependency.
                dependency = self.include_path(
                    include_name
                )

                if dependency not in seen:
                    seen.add(dependency)
                    dependencies.append(
                        dependency
                    )

                    self.required_by.setdefault(
                        dependency,
                        set(),
                    ).add(path)

                # Dependency is emitted separately.
                output.append(
                    "\t"
                    + self.comment_directive(
                        source_line
                    )
                )
                continue

            if PRAGMA_ONCE_RE.match(body):
                output.append(
                    "\t"
                    + self.comment_directive(
                        source_line
                    )
                )
                continue

            if EXPORT_MODULE_RE.match(body):
                output.append(
                    "\t"
                    + self.comment_directive(
                        source_line
                    )
                )
                continue

            output.append(
                "\t" + source_line
            )

        self.sources[path] = "".join(output)

        return dependencies

    def read_and_transform(
        self,
        path: str,
    ) -> tuple[str, list[str]]:
        """
        Read and transform one source file.

        Safe to execute in a worker thread because each call operates only
        on its own path and source-local output.
        """
        text = self.read(path)

        dependencies = self.transform(
            path,
            text,
        )

        return path, dependencies

    def discover(
        self,
        root: str,
    ) -> None:
        """
        Discover the complete graph in breadth-first waves.

        A single thread pool remains alive for all waves so thread creation
        and shutdown are not repeatedly paid between dependency levels.
        """
        pending: list[str] = [
            root
        ]

        queued: set[str] = {
            root
        }

        workers = min(
            16,
            max(
                1,
                os.cpu_count() or 1,
            ),
        )

        with ThreadPoolExecutor(
            max_workers=workers,
        ) as executor:
            while pending:
                current = pending

                results = list(
                    executor.map(
                        self.read_and_transform,
                        current,
                    )
                )

                next_pending: list[str] = []

                for path, dependencies in results:
                    if path in self.files:
                        continue

                    self.files.add(path)
                    self.dependencies[path] = dependencies

                    for dependency in dependencies:
                        if dependency in queued:
                            continue

                        queued.add(dependency)
                        next_pending.append(
                            dependency
                        )

                pending = next_pending

        self.validate_cycles(
            root
        )

    def validate_cycles(
        self,
        root: str,
    ) -> None:
        """
        Validate that the discovered dependency graph is acyclic.
        """
        visiting: set[str] = set()
        visited: set[str] = set()

        def visit(
            path: str,
        ) -> None:
            if path in visited:
                return

            if path in visiting:
                raise RuntimeError(
                    f"include cycle detected involving "
                    f"'{self.display_path(path)}'"
                )

            visiting.add(path)

            for dependency in self.dependencies.get(
                path,
                (),
            ):
                visit(
                    dependency
                )

            visiting.remove(path)
            visited.add(path)

        visit(root)

    def emit_required_by(
        self,
        path: str,
    ) -> None:
        required_by = self.required_by.get(
            path
        )

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

    def emit_line_directive(
        self,
        path: str,
    ) -> None:
        display = self.display_path(
            path
        )

        self.output.append(
            "#if RAWR_AMALGAM_SOURCE_MAPPING\n"
            f'    #line 0 "{display}"\n'
            "#endif\n"
        )

    def emit(
        self,
        path: str,
    ) -> None:
        """
        Emit dependency-first.

        All parsing and filesystem access happened during discovery.
        """
        if path in self.emitted:
            return

        for dependency in self.dependencies.get(
            path,
            (),
        ):
            self.emit(
                dependency
            )

        display = self.display_path(
            path
        )

        self.emit_required_by(
            path
        )

        self.output.append(
            f'#pragma region "{display}"\n'
        )

        self.emit_line_directive(
            path
        )

        self.output.append(
            self.sources[path]
        )

        self.output.append(
            "\n"
            f'#pragma endregion "{display}"\n'
            "\n"
        )

        self.emitted.add(
            path
        )

    def generate(
        self,
        input_path: str,
    ) -> str:
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
            "#undef RAWR_MODULE\n",
            "#endif\n",
            "\n",
        ]

        root = os.path.abspath(
            input_path
        )

        prefix = self.root + "/"

        if root.startswith(prefix):
            self.display_paths[root] = root[
                len(prefix):
            ]
        else:
            self.display_paths[root] = root

        if not os.path.isfile(root):
            raise RuntimeError(
                f"cannot find input '{input_path}'"
            )

        # Phase 1:
        #   - read every reachable file once
        #   - parse every file once
        #   - overlap independent file reads
        #   - build the complete graph
        self.discover(
            root
        )

        # Phase 2:
        #   - deterministic dependency-first emission
        #   - no filesystem access
        #   - no parsing
        self.emit(
            root
        )

        self.output.append(
            "#pragma endregion rawr-amalgam\n"
        )

        return "".join(
            self.output
        )


def main() -> int:
    parser = argparse.ArgumentParser(
        prog="rawr-amalgam",
        description="Generate a reversible rawr amalgamation.",
    )

    parser.add_argument(
        "-I",
        "--include-path",
        dest="include_path",
        type=str,
        required=True,
        help="rawr include root",
    )

    parser.add_argument(
        "input",
        type=str,
        help="root header to amalgamate",
    )

    parser.add_argument(
        "output",
        type=str,
        help="output amalgamated header",
    )

    args = parser.parse_args()

    try:
        amalgamator = Amalgamator(
            args.include_path
        )

        result = amalgamator.generate(
            args.input
        )

        output_parent = os.path.dirname(
            os.path.abspath(
                args.output
            )
        )

        if output_parent:
            os.makedirs(
                output_parent,
                exist_ok=True,
            )

        with open(
            args.output,
            "w",
            encoding="utf-8",
            newline="",
        ) as file:
            file.write(
                result
            )

    except RuntimeError as exc:
        print(
            f"rawr-amalgam: error: {exc}",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(
        main()
    )
