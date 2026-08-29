FROM alpine:3.21

RUN apk add --no-cache \
    clang lld compiler-rt \
    llvm \
    clang-extra-tools \
    musl-dev linux-headers \
    cmake ninja \
    python3 py3-pip \
    boost-dev \
    git curl wget

RUN pip3 install --break-system-packages meson ninja

# Default to clang for everything — no accidental GCC fallback
ENV CC=clang
ENV CXX=clang++
ENV AR=llvm-ar
ENV NM=llvm-nm
ENV RANLIB=llvm-ranlib

WORKDIR /workspace
