import subprocess
import shutil
import json
import sys
from pathlib import Path
from dataclasses import dataclass
from concurrent.futures import ThreadPoolExecutor, as_completed
from graphlib import TopologicalSorter

@dataclass(frozen=True)
class ModuleData:
    src: Path
    name: str
    requires: list[str]

class DependencyScanError(Exception):
    pass

scanner = shutil.which('clang-scan-deps')
def scan_deps(src: Path, compiler: str, flags: list[str]) -> ModuleData:
    global scanner

    if scanner:
        cmd = [scanner, '-format=p1689', '--', compiler, *flags, '-x', 'c++', '-c', str(src)]
    else:
        cmd = [compiler, *flags, '-std=c++20', '-fmodules-ts',
               '-fdeps-format=p1689', '-fdeps-file=/dev/stdout',
               '-x', 'c++', '-c', str(src), '-o', '/dev/null']

    # run() handles the pipe buffers safely and blocks the worker thread, not the main thread
    r = subprocess.run(cmd, text=True, capture_output=True)

    if r.returncode != 0:
        # Usually this means its a .pp file.
        print(f"[scan_deps.py] ERROR: Failed to parse dependencies for {src}:\nSTDOUT: {r.stdout}\nSTDERR: {r.stderr}", file=sys.stderr)
        return ModuleData(
            src=src,
            name="",
            requires=[],
        )
        #raise DependencyScanError(f"Scanning {src} failed: {cmd}\n{r.stderr}{r.stdout}")

    deps = json.loads(r.stdout)['rules'][0]

    # On non-module translation units, the "provides" key may not exist, that is, when the file is a bare
    # header with no module declaration (but maybe a module import).
    if "provides" in deps:
        source_path = deps["provides"][0]["source-path"]
        name = deps["provides"][0]["logical-name"]
    else:
        source_path = src
        name = ""

    return ModuleData(
        src=Path(source_path),
        name=name,
        requires=[d["logical-name"] for d in deps.get("requires", [])]
    )

def main():
    split_index = sys.argv.index('--modules--')
    compiler = sys.argv[1]
    flags = sys.argv[2:split_index]
    srcs = [f for l in (
        [Path(p)] if Path(p).is_file() else list(Path(p).rglob('*')) for p in sys.argv[split_index + 1:]
        ) for f in l if f.is_file()
    ]

    parsed_modules: dict[str, ModuleData] = {}

    # 1. Bounded Concurrency: Limits subprocesses to CPU cores
    with ThreadPoolExecutor() as executor:
        # Submit all tasks
        future_to_src = {
            executor.submit(scan_deps, src, compiler, flags): src
            for src in srcs
        }

        # Collect results as they finish
        for future in as_completed(future_to_src):
            try:
                mod_data = future.result()
                parsed_modules[mod_data.name if mod_data.name else str(mod_data.src)] = mod_data
            except DependencyScanError as e:
                print(f"[scan_deps.py] ERROR: {e}", file=sys.stderr)
                sys.exit(1)

    # 2. Idiomatic Topological Sorting
    # graphlib expects {node: [dependencies...]}
    graph = {
        name: sorted(mod.requires)
        for name, mod in sorted(parsed_modules.items())
    }

    sorter = TopologicalSorter(graph)

    try:
        for mod_name in sorter.static_order():
            if mod_name in parsed_modules:
                mod = parsed_modules[mod_name]
                requires_str = ','.join(mod.requires)
                print(f"{mod.src.relative_to(Path.cwd()).as_posix()}|{mod.name}|{requires_str}")
    except ValueError as e:
        print(f"[scan_deps.py] ERROR: Circular dependency detected: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
