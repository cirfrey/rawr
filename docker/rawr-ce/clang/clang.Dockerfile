ARG UBUNTU_VERSION=22.04
FROM ubuntu:${UBUNTU_VERSION}

ARG LLVM_VERSION=13
ARG WATCHEXEC_VERSION=1.25.1
ARG DEBIAN_FRONTEND=noninteractive

# LLVM's install script sets up the apt repo and installs the requested version.
RUN apt-get update && apt-get install -y --no-install-recommends \
        curl \
        gnupg \
        lsb-release \
        software-properties-common \
        meson \
        ninja-build \
        python3 \
        xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://apt.llvm.org/llvm.sh | bash -s -- ${LLVM_VERSION} \
    && apt-get install -y --no-install-recommends \
        clang-${LLVM_VERSION} \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/cc  cc  /usr/bin/clang-${LLVM_VERSION}   100 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-${LLVM_VERSION} 100

RUN curl -fsSL \
    "https://github.com/watchexec/watchexec/releases/download/v${WATCHEXEC_VERSION}/watchexec-${WATCHEXEC_VERSION}-x86_64-unknown-linux-musl.tar.xz" \
    | tar -xJ --strip-components=1 -C /usr/local/bin \
        "watchexec-${WATCHEXEC_VERSION}-x86_64-unknown-linux-musl/watchexec"

RUN mkdir -p /build /usr/local/lib/rawr/modules /etc/rawr

COPY clang/c++.local.config /etc/rawr/native.ini
COPY scripts/entrypoint.sh /entrypoint.sh
COPY scripts/install-bmis.sh /install-bmis.sh
RUN chmod +x /entrypoint.sh /install-bmis.sh

ENTRYPOINT ["/entrypoint.sh"]
