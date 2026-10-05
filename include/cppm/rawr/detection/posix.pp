#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.posix.pp
#endif

#include "rawr/detection/platform.pp"
#include "rawr/detection/env.pp"

// POSIX: meaningful syscall-level POSIX APIs exist.
// WASM deliberately excluded — Emscripten emulates POSIX in userspace,
// standalone WASM/WASI has a completely different interface.
// Cygwin provides POSIX over Windows, hence included.
#define RAWR_IS_POSIX 0
#if RAWR_PLATFORM_LINUX   || \
    RAWR_PLATFORM_MACOS   || \
    RAWR_PLATFORM_IOS     || \
    RAWR_PLATFORM_ANDROID || \
    RAWR_PLATFORM_OPENBSD || \
    RAWR_PLATFORM_FREEBSD || \
    RAWR_PLATFORM_NETBSD  || \
    RAWR_ENV_CYGWIN
    #undef  RAWR_IS_POSIX
    #define RAWR_IS_POSIX 1
#endif
