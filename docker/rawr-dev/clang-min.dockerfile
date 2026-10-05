FROM silkeh/clang:12-bullseye

# Pin main + security to a pre-EOL Bullseye snapshot
RUN echo "deb [check-valid-until=no] http://snapshot.debian.org/archive/debian/20260825T000000Z bullseye main" \
      > /etc/apt/sources.list && \
    echo "deb [check-valid-until=no] http://snapshot.debian.org/archive/debian/20260825T000000Z bullseye-updates main" \
      >> /etc/apt/sources.list && \
    echo "deb [check-valid-until=no] http://snapshot.debian.org/archive/debian-security/20260825T000000Z bullseye-security main" \
      >> /etc/apt/sources.list && \
    # Add third-party Python 3.10 backport repository
    echo "deb http://deb.pascalroeleven.nl/python3.10 bullseye-backports main" \
      >> /etc/apt/sources.list

# Import the repository's PGP key
RUN wget -qO- https://pascalroeleven.nl/deb-pascalroeleven.gpg | tee /etc/apt/trusted.gpg.d/deb-pascalroeleven.gpg

# Install additional tools and Python 3.10
RUN apt-get update && apt-get install -y --no-install-recommends \
      musl-dev linux-headers-amd64 \
      cmake ninja-build \
      python3.10 python3.10-venv python3.10-dev \
      libboost-dev \
      git curl wget \
    && rm -rf /var/lib/apt/lists/*

# Make python3.10 the default "python3"
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.10 1

# Install Python build tools
RUN wget -qO /tmp/get-pip.py https://bootstrap.pypa.io/get-pip.py && \
    python3.10 /tmp/get-pip.py && \
    rm /tmp/get-pip.py && \
    python3.10 -m pip install --no-cache-dir meson ninja

# Force Clang for everything
ENV CC=clang
ENV CXX=clang++
ENV AR=llvm-ar
ENV NM=llvm-nm
ENV RANLIB=llvm-ranlib

WORKDIR /workspace
