#!/usr/bin/env python3
import argparse, subprocess, sys
from pathlib import Path

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', required=True)
    ap.add_argument('--root',  required=True)
    args = ap.parse_args()

    headers = [
        str(h) for h in Path(args.root).rglob('*')
        if h.is_file() # and h.suffix == '.hpp'
    ]
    if not headers:
        print('run_tidy: no headers found', file=sys.stderr)
        sys.exit(1)


    result = subprocess.run(
        ['clang-tidy', '-p', args.build] + [str(h) for h in headers],
        check=False
    )
    sys.exit(result.returncode)

if __name__ == '__main__':
    main()
