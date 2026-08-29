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
        cmd = [scanner, '-format=p1689', '--', compiler, *flags, '-c', str(src)]
    else:
        cmd = [compiler, *flags, '-std=c++20', '-fmodules-ts',
               '-fdeps-format=p1689', '-fdeps-file=/dev/stdout',
               '-x', 'c++', '-c', str(src), '-o', '/dev/null']

    # run() handles the pipe buffers safely and blocks the worker thread, not the main thread
    r = subprocess.run(cmd, text=True, capture_output=True)

    if r.returncode != 0:
        raise DependencyScanError(f"Scanning {src} failed:\n{cmd}\n{r.stderr or r.stdout}")

    deps = json.loads(r.stdout)['rules'][0]

    # Extract the primary module provided by this file
    primary_dep = deps["provides"][0]
    return ModuleData(
        src=Path(primary_dep["source-path"]),
        name=primary_dep["logical-name"],
        requires=[d["logical-name"] for d in deps.get("requires", [])]
    )

def main():
    split_index = sys.argv.index('--modules--')
    compiler = sys.argv[1]
    flags = sys.argv[2:split_index]
    srcs = [Path(p) for p in sys.argv[split_index + 1:]]

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
                parsed_modules[mod_data.name] = mod_data
            except DependencyScanError as e:
                print(f"[scan_deps.py] ERROR: {e}", file=sys.stderr)
                sys.exit(1)

    # 2. Idiomatic Topological Sorting
    # graphlib expects {node: [dependencies...]}
    graph = {
        name: mod.requires
        for name, mod in parsed_modules.items()
    }

    sorter = TopologicalSorter(graph)

    try:
        # static_order() returns an iterable of topologically sorted nodes
        for mod_name in sorter.static_order():
            if mod_name in parsed_modules:
                mod = parsed_modules[mod_name]
                requires_str = ','.join(mod.requires)
                print(f"{mod.src}|{mod.name}|{requires_str}")
    except ValueError as e:
        print(f"[scan_deps.py] ERROR: Circular dependency detected: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
