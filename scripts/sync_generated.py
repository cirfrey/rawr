#!/usr/bin/env python3

import filecmp
import os
import shutil
import sys


def files_under(root: str) -> set[str]:
    result: set[str] = set()

    if not os.path.isdir(root):
        return result

    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            result.add(
                os.path.relpath(path, root)
            )

    return result


def sync(src: str, dst: str) -> None:
    src = os.path.abspath(src)
    dst = os.path.abspath(dst)

    if not os.path.isdir(src):
        raise SystemExit(f'sync_generated: source directory does not exist: {src}')

    src_files = files_under(src)
    dst_files = files_under(dst)

    copied = 0
    removed = 0
    unchanged = 0

    #
    # Copy new/modified files only.
    #
    for rel in sorted(src_files):
        src_path = os.path.join(src, rel)
        dst_path = os.path.join(dst, rel)

        os.makedirs(
            os.path.dirname(dst_path),
            exist_ok=True,
        )

        if os.path.isfile(dst_path):
            if filecmp.cmp(
                src_path,
                dst_path,
                shallow=False,
            ):
                unchanged += 1
                continue

        shutil.copy2(src_path, dst_path)
        copied += 1

    #
    # Remove files which the generated tree no longer contains.
    #
    for rel in sorted(dst_files - src_files):
        os.remove(os.path.join(dst, rel))
        removed += 1

    #
    # Remove empty directories left behind.
    #
    if os.path.isdir(dst):
        for dirpath, dirnames, filenames in os.walk(
            dst,
            topdown=False,
        ):
            if not dirnames and not filenames:
                os.rmdir(dirpath)

    print(
        f'sync_generated: {dst}: '
        f'{copied} copied, '
        f'{removed} removed, '
        f'{unchanged} unchanged'
    )


def main() -> int:
    if len(sys.argv) != 3:
        print(
            f'usage: {sys.argv[0]} SOURCE DEST',
            file=sys.stderr,
        )
        return 2

    sync(sys.argv[1], sys.argv[2])
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
