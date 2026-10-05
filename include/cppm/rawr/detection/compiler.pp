#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.compiler.pp
#endif

#define RAWR_DETECTION_MIN_GCC_VERSION   11
#define RAWR_DETECTION_MIN_CLANG_VERSION 12
#define RAWR_DETECTION_MIN_MSVC_VERSION  1928

#define RAWR_COMPILER_CLANG   0
#define RAWR_COMPILER_GCC     0
#define RAWR_COMPILER_MSVC    0
#define RAWR_COMPILER_UNKNOWN 0
// Clang must come before GCC — clang-cl defines both __clang__ and _MSC_VER,
// and Apple Clang defines __GNUC__ for compatibility.
#if defined(__clang__)
    #undef  RAWR_COMPILER_CLANG
    #define RAWR_COMPILER_CLANG        1
    #define RAWR_COMPILER_VERSION      (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #define RAWR_COMPILER_VERSION_MAJOR __clang_major__
    #define RAWR_COMPILER_VERSION_MINOR __clang_minor__
    #define RAWR_COMPILER_VERSION_PATCH __clang_patchlevel__
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif defined(__GNUC__) || defined(__GNUG__)
    #undef  RAWR_COMPILER_GCC
    #define RAWR_COMPILER_GCC           1
    #define RAWR_COMPILER_VERSION       (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #define RAWR_COMPILER_VERSION_MAJOR __GNUC__
    #define RAWR_COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define RAWR_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif defined(_MSC_VER)
    #undef  RAWR_COMPILER_MSVC
    #define RAWR_COMPILER_MSVC          1
    #define RAWR_COMPILER_VERSION       _MSC_FULL_VER
    #define RAWR_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
    #define RAWR_COMPILER_VERSION_MINOR (_MSC_VER % 100)
    #define RAWR_COMPILER_VERSION_PATCH 0
    #define RAWR_COMPILER_VERSION_BUILD (_MSC_FULL_VER % 100000)
#else
    #undef  RAWR_COMPILER_UNKNOWN
    #define RAWR_COMPILER_UNKNOWN       1
    #define RAWR_COMPILER_VERSION       0
    #define RAWR_COMPILER_VERSION_MAJOR 0
    #define RAWR_COMPILER_VERSION_MINOR 0
    #define RAWR_COMPILER_VERSION_PATCH 0
    #define RAWR_COMPILER_VERSION_BUILD 0
#endif

#define RAWR_COMPILER_FAMILY_GNU 0
#if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
    #undef  RAWR_COMPILER_FAMILY_GNU
    #define RAWR_COMPILER_FAMILY_GNU 1
#endif

#if !RAWR_DETECTION_NO_COMPILER_ERROR
    #if RAWR_COMPILER_GCC
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_DETECTION_MIN_GCC_VERSION
            #error "rawr/detection/compiler.pp: GCC 11+ required (concepts correctness, NTTP structural types)"
        #endif
    #elif RAWR_COMPILER_CLANG
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_DETECTION_MIN_CLANG_VERSION
            #error "rawr/detection/compiler.pp: Clang 12+ required (concepts constraint subsumption correctness)"
        #endif
    #elif RAWR_COMPILER_MSVC
        #if _MSC_VER < RAWR_DETECTION_MIN_MSVC_VERSION
            #error "rawr/detection/compiler.pp: MSVC 16.8+ (_MSC_VER >= 1928) required (C++20 mode, concepts)"
        #endif
    #else
        #error "rawr/detection/compiler.pp: unrecognized compiler"
    #endif
#endif
