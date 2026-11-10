#!/usr/bin/env python3
"""Print a compiler's predefined C++ macros to stdout."""

import os
import subprocess
import sys


def detect_compiler(exe: str) -> str:
    name = os.path.basename(exe).lower()
    if name.endswith(".exe"):
        name = name[:-4]

    # cl.exe doesn't implement --version as a query, but its banner is still
    # emitted before it errors about the missing source file.
    if name == "cl":
        return "msvc"
    if name.startswith("clang-cl"):
        return "clang-cl"

    try:
        result = subprocess.run(
            [exe, "--version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            errors="replace",
            check=False,
        )
    except OSError as error:
        raise RuntimeError(f"cannot run {exe!r}: {error}") from error

    banner = f"{result.stdout}\n{result.stderr}".lower()

    if "microsoft (r) c/c++" in banner or "microsoft c/c++" in banner:
        return "msvc"
    if "clang" in banner:
        return "clang"
    if "gcc" in banner or "free software foundation" in banner:
        return "gcc"

    # Useful fallback for versioned compiler executable names and wrappers
    # whose --version output doesn't identify the compiler clearly.
    if name.startswith(("gcc", "g++")):
        return "gcc"
    if name.startswith("clang"):
        return "clang"

    raise RuntimeError(
        f"could not identify compiler {exe!r} from its --version output; "
        "use gcc/g++, clang/clang++, or cl.exe as the executable"
    )


def main() -> int:
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} EXE [EXEFLAGS ...]", file=sys.stderr)
        return 2

    exe, *exe_flags = sys.argv[1:]

    try:
        compiler = detect_compiler(exe)
    except RuntimeError as error:
        print(f"{sys.argv[0]}: {error}", file=sys.stderr)
        return 2

    if compiler in {"gcc", "clang"}:
        command = [exe, *exe_flags, "-dM", "-E", "-x", "c++", "-"]
        return subprocess.run(command, stdin=subprocess.DEVNULL, check=False).returncode

    if compiler == "clang-cl":
        # clang-cl's MSVC-style driver uses Clang's /d1PP extension to retain
        # macro definitions in preprocessing output.
        command = [exe, *exe_flags, "/nologo", "/d1PP", "/EP", "/TP", "NUL"]
        return subprocess.run(command, check=False).returncode

    command = [exe, *exe_flags, "/nologo", "/PD", "/EP", "/TP", "NUL"]
    return subprocess.run(command, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())
