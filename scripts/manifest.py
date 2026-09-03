#!/usr/bin/env python3

from __future__ import annotations

import hashlib
import heapq
import json
import os
import shutil
import subprocess
import sys

from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class ModuleData:
    src: Path
    name: str
    requires: list[str]


@dataclass(frozen=True)
class ManifestEntry:
    name: str
    requires: list[str]
    size: int | None
    mtime_ns: int | None
    digest: str


class ManifestError(Exception):
    pass


SCANNER = shutil.which("clang-scan-deps")

SCANNABLE_SUFFIXES = {".hpp"}
MAX_WORKERS = 16

MANIFEST_HEADER = "# rawr module manifest v2"
MANIFEST_COLUMNS = "# path|module|requires|size|mtime_ns|sha256"


def source_key(path: Path, root: Path) -> str:
    """
    Return the canonical source-tree path used by the manifest.

    Paths are always stored relative to --root so that the manifest is
    independent of whether the scanner was invoked from the source or build
    directory.
    """
    path = Path(os.path.abspath(path))
    root = Path(os.path.abspath(root))

    return Path(os.path.relpath(path, root)).as_posix()


def collect_sources(
    inputs: list[Path],
) -> dict[Path, Path]:
    """
    Collect all .hpp files.

    The dictionary maps:
        canonical absolute path -> source path

    Traversal is deterministic.
    """
    result: dict[Path, Path] = {}

    def visit(directory: Path) -> None:
        try:
            with os.scandir(directory) as it:
                entries = sorted(it, key=lambda e: e.name)

                for entry in entries:
                    path = directory / entry.name

                    try:
                        if entry.is_dir(follow_symlinks=False):
                            visit(path)
                        elif (
                            entry.is_file(follow_symlinks=False)
                            and path.suffix in SCANNABLE_SUFFIXES
                        ):
                            absolute = Path(
                                os.path.abspath(path)
                            )
                            result[absolute] = absolute

                    except OSError as e:
                        raise ManifestError(
                            f"failed to inspect {path}: {e}"
                        ) from e

        except OSError as e:
            raise ManifestError(
                f"failed to scan directory {directory}: {e}"
            ) from e

    for input_path in inputs:
        path = Path(os.path.abspath(input_path))

        if path.is_dir():
            visit(path)

        elif (
            path.is_file()
            and path.suffix in SCANNABLE_SUFFIXES
        ):
            result[path] = path

        else:
            raise ManifestError(
                f"module input does not exist or is not a .hpp: {path}"
            )

    return result


def stat_file(path: Path) -> tuple[int, int]:
    st = os.stat(path)
    return st.st_size, st.st_mtime_ns


def get_source_metadata(
    sources: dict[Path, Path],
) -> dict[Path, tuple[int, int]]:
    """
    Stat all sources concurrently.

    This allows the common unchanged case to avoid opening the files.
    """
    metadata: dict[Path, tuple[int, int]] = {}

    with ThreadPoolExecutor(
        max_workers=MAX_WORKERS,
    ) as executor:
        futures = {
            executor.submit(stat_file, path): path
            for path in sources
        }

        for future in as_completed(futures):
            path = futures[future]
            metadata[path] = future.result()

    return metadata


def hash_file(path: Path) -> str:
    h = hashlib.sha256()

    with path.open("rb") as f:
        while True:
            chunk = f.read(1024 * 1024)

            if not chunk:
                break

            h.update(chunk)

    return h.hexdigest()


def load_manifest(
    path: Path | None,
) -> dict[str, ManifestEntry]:
    if path is None or not path.is_file():
        return {}

    result: dict[str, ManifestEntry] = {}

    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")

            if not line or line.startswith("#"):
                continue

            parts = line.split("|")

            if len(parts) == 4:
                # v1:
                #
                # path|module|requires|sha256
                src, name, requires, digest = parts

                result[src] = ManifestEntry(
                    name=name,
                    requires=(
                        []
                        if not requires
                        else requires.split(",")
                    ),
                    size=None,
                    mtime_ns=None,
                    digest=digest,
                )

            elif len(parts) == 6:
                # v2:
                #
                # path|module|requires|size|mtime_ns|sha256
                (
                    src,
                    name,
                    requires,
                    size,
                    mtime_ns,
                    digest,
                ) = parts

                result[src] = ManifestEntry(
                    name=name,
                    requires=(
                        []
                        if not requires
                        else requires.split(",")
                    ),
                    size=int(size),
                    mtime_ns=int(mtime_ns),
                    digest=digest,
                )

            else:
                raise ManifestError(
                    f"invalid manifest line:\n{line}"
                )

    return result


def scan_module(
    src: Path,
    compiler: str,
    flags: list[str],
) -> ModuleData:
    if SCANNER is not None:
        cmd = [
            SCANNER,
            "-format=p1689",
            "--",
            compiler,
            *flags,
            "-x",
            "c++",
            "-c",
            str(src),
        ]

    else:
        cmd = [
            compiler,
            *flags,
            "-fmodules-ts",
            "-fdeps-format=p1689",
            "-fdeps-file=/dev/stdout",
            "-x",
            "c++",
            "-c",
            str(src),
            "-o",
            "/dev/null",
        ]

    result = subprocess.run(
        cmd,
        text=True,
        capture_output=True,
    )

    if result.returncode != 0:
        raise ManifestError(
            f"dependency scan failed for {src}:\n"
            f"{result.stderr.strip()}"
        )

    try:
        rules = json.loads(result.stdout)["rules"]
        rule = rules[0]

    except (json.JSONDecodeError, KeyError, IndexError) as e:
        raise ManifestError(
            f"invalid P1689 output for {src}"
        ) from e

    if rule.get("provides"):
        name = rule["provides"][0]["logical-name"]
    else:
        name = ""

    requires = [
        dep["logical-name"]
        for dep in rule.get("requires", [])
    ]

    return ModuleData(
        src=src,
        name=name,
        requires=requires,
    )


def build_modules(
    sources: dict[Path, Path],
    metadata: dict[Path, tuple[int, int]],
    cached: dict[str, ManifestEntry],
    root: Path,
    compiler: str,
    flags: list[str],
) -> tuple[dict[Path, ModuleData], dict[Path, str]]:
    """
    Build module information using the manifest as an incremental cache.

    Fast path:
        size + mtime unchanged
            -> reuse cached entry

    Slow path:
        metadata changed/new
            -> hash file

        hash unchanged
            -> reuse cached dependency information

        hash changed/new
            -> run dependency scanner
    """
    modules: dict[Path, ModuleData] = {}
    hashes: dict[Path, str] = {}

    hash_jobs = {}
    scan_jobs = {}

    cache_hits = 0
    hash_count = 0
    scan_count = 0

    with ThreadPoolExecutor(
        max_workers=MAX_WORKERS,
    ) as executor:

        for path in sources:
            key = source_key(path, root)
            size, mtime_ns = metadata[path]
            old = cached.get(key)

            #
            # Metadata fast path.
            #
            if (
                old is not None
                and old.size == size
                and old.mtime_ns == mtime_ns
            ):
                modules[path] = ModuleData(
                    src=path,
                    name=old.name,
                    requires=old.requires,
                )

                hashes[path] = old.digest
                cache_hits += 1

                continue

            #
            # New file or metadata changed.
            #
            hash_jobs[
                executor.submit(hash_file, path)
            ] = (
                path,
                key,
            )

        #
        # Resolve content hashes.
        #
        for future in as_completed(hash_jobs):
            path, key = hash_jobs[future]

            digest = future.result()

            hashes[path] = digest
            hash_count += 1

            old = cached.get(key)

            #
            # Metadata changed but content did not.
            #
            if (
                old is not None
                and old.digest == digest
            ):
                modules[path] = ModuleData(
                    src=path,
                    name=old.name,
                    requires=old.requires,
                )

                cache_hits += 1

                continue

            #
            # Actual content change or new file.
            #
            scan_jobs[
                executor.submit(
                    scan_module,
                    path,
                    compiler,
                    flags,
                )
            ] = path

        #
        # Resolve dependency scans.
        #
        for future in as_completed(scan_jobs):
            path = scan_jobs[future]

            modules[path] = future.result()
            scan_count += 1

    print(
        f"# cache: {cache_hits} hit, "
        f"{hash_count} hashed, "
        f"{scan_count} scanned",
        file=sys.stderr,
    )

    return modules, hashes


def topo_sort(
    modules: dict[Path, ModuleData],
) -> list[ModuleData]:
    """
    Deterministic topological ordering.

    When multiple modules are ready at the same time, the lexicographically
    smallest module name is selected.
    """
    by_name: dict[str, ModuleData] = {
        module.name: module
        for module in modules.values()
        if module.name
    }

    names = set(by_name)

    indegree: dict[str, int] = {
        name: sum(
            dependency in names
            for dependency in module.requires
        )
        for name, module in by_name.items()
    }

    dependents: dict[str, list[str]] = {
        name: []
        for name in names
    }

    for module in by_name.values():
        for dependency in module.requires:
            if dependency in names:
                dependents[dependency].append(module.name)

    for values in dependents.values():
        values.sort()

    ready = [
        name
        for name, degree in indegree.items()
        if degree == 0
    ]

    heapq.heapify(ready)

    order: list[str] = []

    while ready:
        name = heapq.heappop(ready)
        order.append(name)

        for dependent in dependents[name]:
            indegree[dependent] -= 1

            if indegree[dependent] == 0:
                heapq.heappush(
                    ready,
                    dependent,
                )

    if len(order) != len(by_name):
        remaining = sorted(
            name
            for name, degree in indegree.items()
            if degree != 0
        )

        raise ManifestError(
            "circular module dependency involving: "
            + ", ".join(remaining)
        )

    return [
        by_name[name]
        for name in order
    ]


def manifest_text(
    modules: list[ModuleData],
    hashes: dict[Path, str],
    metadata: dict[Path, tuple[int, int]],
    root: Path,
) -> str:
    lines = [
        MANIFEST_HEADER,
        MANIFEST_COLUMNS,
    ]

    for module in modules:
        key = source_key(module.src, root)
        size, mtime_ns = metadata[module.src]
        requires = ",".join(
            sorted(module.requires)
        )

        lines.append(
            f"{key}|"
            f"{module.name}|"
            f"{requires}|"
            f"{size}|"
            f"{mtime_ns}|"
            f"{hashes[module.src]}"
        )

    return "\n".join(lines) + "\n"


def write_manifest(
    path: Path,
    text: str,
) -> None:
    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    path.write_text(
        text,
        encoding="utf-8",
        newline="\n",
    )


def parse_args():
    if "--modules--" not in sys.argv:
        raise ManifestError(
            "missing --modules-- separator"
        )

    separator = sys.argv.index("--modules--")

    before = sys.argv[1:separator]
    module_inputs = sys.argv[separator + 1:]

    if not before:
        raise ManifestError(
            "missing compiler"
        )

    compiler = before[0]
    raw_args = before[1:]

    manifest_path: Path | None = None
    cache_path: Path | None = None
    root: Path | None = None
    depfile_path: Path | None = None

    compiler_flags: list[str] = []

    i = 0

    while i < len(raw_args):
        arg = raw_args[i]

        if arg == "--manifest":
            if i + 1 >= len(raw_args):
                raise ManifestError(
                    "--manifest requires a path"
                )

            manifest_path = Path(
                raw_args[i + 1]
            )

            i += 2
            continue

        if arg == "--cache":
            if i + 1 >= len(raw_args):
                raise ManifestError(
                    "--cache requires a path"
                )

            cache_path = Path(
                raw_args[i + 1]
            )

            i += 2
            continue

        if arg == "--root":
            if i + 1 >= len(raw_args):
                raise ManifestError(
                    "--root requires a path"
                )

            root = Path(
                raw_args[i + 1]
            )

            i += 2
            continue

        if arg == "--depfile":
            if i + 1 >= len(raw_args):
                raise ManifestError(
                    "--depfile requires a path"
                )

            depfile_path = Path(
                raw_args[i + 1]
            )

            i += 2
            continue

        compiler_flags.append(arg)
        i += 1

    return (
        compiler,
        compiler_flags,
        manifest_path,
        cache_path,
        root,
        depfile_path,
        [
            Path(arg)
            for arg in module_inputs
        ],
    )


def ninja_escape(path: str) -> str:
    """
    Escape a path for a Ninja depfile.
    """
    return (
        path
        .replace("\\", "\\\\")
        .replace("$", "$$")
        .replace(" ", "\\ ")
        .replace("#", "\\#")
    )


def write_depfile(
    depfile: Path,
    sources: dict[Path, Path],
    inputs: list[Path],
) -> None:
    """
    Write a Ninja dependency file for Meson's configure_file() machinery.

    Every source .hpp is listed individually so edits cause reconfiguration.

    Input directories are also listed so adding/removing an .hpp changes the
    directory mtime and causes reconfiguration.
    """
    dependencies: set[str] = set()

    for path in sources:
        dependencies.add(
            str(path.resolve())
        )

    for input_path in inputs:
        path = input_path.resolve()

        if path.is_dir():
            dependencies.add(str(path))

    target = depfile.with_suffix("")

    depfile.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    with depfile.open(
        "w",
        encoding="utf-8",
        newline="\n",
    ) as f:
        f.write(
            ninja_escape(str(target))
        )
        f.write(":")

        for dependency in sorted(dependencies):
            f.write(" ")
            f.write(
                ninja_escape(dependency)
            )

        f.write("\n")


def main() -> int:
    (
        compiler,
        compiler_flags,
        manifest_path,
        cache_path,
        root,
        depfile_path,
        inputs,
    ) = parse_args()

    if root is None:
        root = Path.cwd()

    root = root.resolve()

    if not inputs:
        raise ManifestError(
            "no module inputs"
        )

    #
    # If --manifest is supplied and --cache isn't, use the existing manifest
    # as the cache before replacing it. This is convenient for the Git hook.
    #
    if cache_path is None:
        cache_path = manifest_path

    sources = collect_sources(inputs)

    cached = load_manifest(
        cache_path
    )

    metadata = get_source_metadata(
        sources
    )

    modules, hashes = build_modules(
        sources,
        metadata,
        cached,
        root,
        compiler,
        compiler_flags,
    )

    ordered = topo_sort(
        modules
    )

    text = manifest_text(
        ordered,
        hashes,
        metadata,
        root,
    )

    #
    # No --manifest:
    #     stdout only.
    #
    # --manifest:
    #     write the file AND emit the same manifest to stdout.
    #
    if manifest_path is not None:
        write_manifest(
            manifest_path,
            text,
        )

    sys.stdout.write(text)

    if depfile_path is not None:
        write_depfile(
            depfile_path,
            sources,
            inputs,
        )

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())

    except ManifestError as e:
        print(
            f"manifest.py: error: {e}",
            file=sys.stderr,
        )

        raise SystemExit(1)
