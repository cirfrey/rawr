import rawrscan
from pathlib import Path

if __name__ == "__main__":
    parsed = rawrscan.parse_file(Path(__file__).parent.parent/'include/cppm/rawr/distribution/version.pp')
    major = [d for d in parsed if isinstance(d, rawrscan.Define) and d.name == "RAWR_VERSION_MAJOR"][-1].body
    minor = [d for d in parsed if isinstance(d, rawrscan.Define) and d.name == "RAWR_VERSION_MINOR"][-1].body
    patch = [d for d in parsed if isinstance(d, rawrscan.Define) and d.name == "RAWR_VERSION_PATCH"][-1].body
    print(f"{major}.{minor}.{patch}")
