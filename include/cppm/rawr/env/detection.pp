// ENV describes the execution environment layered above the host.
// A HOST_LINUX target with freestanding toolchain gets ENV_FREESTANDING.
// A HOST_WINDOWS target with Cygwin gets ENV_CYGWIN AND HOST_WINDOWS.
#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.env.detection.pp
#endif

#include "rawr/platform/detection.pp"

// --- Ids. ---

#define RAWR_ENVID_UNKNOWN        1
#define RAWR_ENVID_CYGWIN         2
#define RAWR_ENVID_MINGW          3
#define RAWR_ENVID_EMSCRIPTEN     4
#define RAWR_ENVID_NATIVE         5
#define RAWR_ENVID_MAX            5

// --- Detection. ---

#if defined(__EMSCRIPTEN__)
    #define RAWR_DETECTED_ENVID RAWR_ENVID_EMSCRIPTEN
#elif defined(__CYGWIN__)
    #define RAWR_DETECTED_ENVID RAWR_ENVID_CYGWIN
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define RAWR_DETECTED_ENVID RAWR_ENVID_MINGW
#elif RAWR_PLATFORM_LINUX   || RAWR_PLATFORM_MACOS  || RAWR_PLATFORM_IOS || \
      RAWR_PLATFORM_ANDROID || RAWR_PLATFORM_WINDOWS
    #define RAWR_DETECTED_ENVID RAWR_ENVID_NATIVE
#else
    #define RAWR_DETECTED_ENVID RAWR_ENVID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_ENVID=RAWR_ENVID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_ENVID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_ENVID
    #error "rawr: RAWR_ENVID is an output of detection; to override it define RAWR_SET_ENVID"
#endif
#ifdef RAWR_SET_ENVID
    #if RAWR_SET_ENVID < 1 || RAWR_SET_ENVID > RAWR_ENVID_MAX
        #error "rawr: RAWR_SET_ENVID must be one of the RAWR_ENVID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_ENVID           RAWR_SET_ENVID
    #define RAWR_ENVID_DETECTED  0
    #define RAWR_ENVID_OVERRIDEN 1
#else
    #define RAWR_ENVID           RAWR_DETECTED_ENVID
    #define RAWR_ENVID_DETECTED  1
    #define RAWR_ENVID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ENV_CYGWIN     0
#define RAWR_ENV_MINGW      0
#define RAWR_ENV_EMSCRIPTEN 0
#define RAWR_ENV_NATIVE     0
#define RAWR_ENV_UNKNOWN    0
#if RAWR_ENVID == RAWR_ENVID_UNKNOWN
    #undef  RAWR_ENV_UNKNOWN
    #define RAWR_ENV_UNKNOWN 1
#elif RAWR_ENVID == RAWR_ENVID_CYGWIN
    #undef  RAWR_ENV_CYGWIN
    #define RAWR_ENV_CYGWIN 1
#elif RAWR_ENVID == RAWR_ENVID_MINGW
    #undef  RAWR_ENV_MINGW
    #define RAWR_ENV_MINGW 1
#elif RAWR_ENVID == RAWR_ENVID_EMSCRIPTEN
    #undef  RAWR_ENV_EMSCRIPTEN
    #define RAWR_ENV_EMSCRIPTEN 1
#elif RAWR_ENVID == RAWR_ENVID_NATIVE
    #undef  RAWR_ENV_NATIVE
    #define RAWR_ENV_NATIVE 1
#endif
