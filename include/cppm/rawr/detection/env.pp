#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.env.pp
#endif

// ENV describes the execution environment layered above the host.
// A HOST_LINUX target with freestanding toolchain gets ENV_FREESTANDING.
// A HOST_WINDOWS target with Cygwin gets ENV_CYGWIN AND HOST_WINDOWS.
#define RAWR_ENV_FREESTANDING 0
#define RAWR_ENV_CYGWIN       0
#define RAWR_ENV_MINGW        0
#define RAWR_ENV_EMSCRIPTEN   0
#define RAWR_ENV_NATIVE       0
#define RAWR_ENV_UNKNOWN      0
#if defined(__EMSCRIPTEN__)
    #undef  RAWR_ENV_EMSCRIPTEN
    #define RAWR_ENV_EMSCRIPTEN 1
// __STDC_HOSTED__ == 0 means the compiler was invoked in freestanding mode (-ffreestanding).
// Embedded targets should set this. Bare toolchains often set it by default.
#elif defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 0
    #undef  RAWR_ENV_FREESTANDING
    #define RAWR_ENV_FREESTANDING 1
#elif defined(__CYGWIN__)
    #undef  RAWR_ENV_CYGWIN
    #define RAWR_ENV_CYGWIN 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #undef  RAWR_ENV_MINGW
    #define RAWR_ENV_MINGW 1
#elif RAWR_PLATFORM_LINUX   || RAWR_PLATFORM_MACOS  || RAWR_PLATFORM_IOS || \
      RAWR_PLATFORM_ANDROID || RAWR_PLATFORM_WINDOWS
    #undef  RAWR_ENV_NATIVE
    #define RAWR_ENV_NATIVE 1
#else
    #undef  RAWR_ENV_UNKNOWN
    #define RAWR_ENV_UNKNOWN 1
#endif
