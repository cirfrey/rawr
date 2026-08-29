ARG UBUNTU_VERSION=22.04
FROM ubuntu:${UBUNTU_VERSION}

ARG GCC_VERSION=11
ARG WATCHEXEC_VERSION=1.25.1
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        gcc-${GCC_VERSION} \
        g++-${GCC_VERSION} \
        ninja-build \
        python3 \
        python3-pip \
        curl \
        xz-utils \
    && pip3 install meson \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/cc  cc  /usr/bin/gcc-${GCC_VERSION} 100 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-${GCC_VERSION} 100

RUN curl -fsSL \
    "https://github.com/watchexec/watchexec/releases/download/v${WATCHEXEC_VERSION}/watchexec-${WATCHEXEC_VERSION}-x86_64-unknown-linux-musl.tar.xz" \
    | tar -xJ --strip-components=1 -C /usr/local/bin \
        "watchexec-${WATCHEXEC_VERSION}-x86_64-unknown-linux-musl/watchexec"

RUN mkdir -p /build /usr/local/lib/rawr/modules /etc/rawr

COPY gcc/c++.local.config /etc/rawr/native.ini
COPY scripts/entrypoint.sh /entrypoint.sh
COPY scripts/install-bmis.sh /install-bmis.sh
RUN chmod +x /entrypoint.sh /install-bmis.sh

ENTRYPOINT ["/entrypoint.sh"]
