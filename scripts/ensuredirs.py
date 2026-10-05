import os
import sys

if __name__ == "__main__":
    for d in sys.argv[1:]:
        os.makedirs(d, exist_ok=True)
