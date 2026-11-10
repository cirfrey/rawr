#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.compiler.detection.pp
#endif

// --- Ids. ---

#define RAWR_COMPILERID_UNKNOWN 1
#define RAWR_COMPILERID_CLANG   2
#define RAWR_COMPILERID_GCC     3
#define RAWR_COMPILERID_MSVC    4
#define RAWR_COMPILERID_MAX     4

// --- Detection. ---

// Clang must come before GCC — clang-cl defines both __clang__ and _MSC_VER,
// and Apple Clang defines __GNUC__ for compatibility.
#if defined(__clang__)
    #define RAWR_DETECTED_COMPILERID RAWR_COMPILERID_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define RAWR_DETECTED_COMPILERID RAWR_COMPILERID_GCC
#elif defined(_MSC_VER)
    #define RAWR_DETECTED_COMPILERID RAWR_COMPILERID_MSVC
#else
    #define RAWR_DETECTED_COMPILERID RAWR_COMPILERID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_COMPILERID=RAWR_COMPILERID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_COMPILERID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_COMPILERID
    #error "rawr: RAWR_COMPILERID is an output of detection; to override it define RAWR_SET_COMPILERID"
#endif
#ifdef RAWR_SET_COMPILERID
    #if RAWR_SET_COMPILERID < 1 || RAWR_SET_COMPILERID > RAWR_COMPILERID_MAX
        #error "rawr: RAWR_SET_COMPILERID must be one of the RAWR_COMPILERID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_COMPILERID           RAWR_SET_COMPILERID
    #define RAWR_COMPILERID_DETECTED  0
    #define RAWR_COMPILERID_OVERRIDEN 1
#else
    #define RAWR_COMPILERID           RAWR_DETECTED_COMPILERID
    #define RAWR_COMPILERID_DETECTED  1
    #define RAWR_COMPILERID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_COMPILER_CLANG   0
#define RAWR_COMPILER_GCC     0
#define RAWR_COMPILER_MSVC    0
#define RAWR_COMPILER_UNKNOWN 0
#if RAWR_COMPILERID == RAWR_COMPILERID_CLANG
    #undef  RAWR_COMPILER_CLANG
    #define RAWR_COMPILER_CLANG        1
    #define RAWR_COMPILER_VERSION      (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #define RAWR_COMPILER_VERSION_MAJOR __clang_major__
    #define RAWR_COMPILER_VERSION_MINOR __clang_minor__
    #define RAWR_COMPILER_VERSION_PATCH __clang_patchlevel__
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif RAWR_COMPILERID == RAWR_COMPILERID_GCC
    #undef  RAWR_COMPILER_GCC
    #define RAWR_COMPILER_GCC           1
    #define RAWR_COMPILER_VERSION       (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #define RAWR_COMPILER_VERSION_MAJOR __GNUC__
    #define RAWR_COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define RAWR_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
    #define RAWR_COMPILER_VERSION_BUILD 0
#elif RAWR_COMPILERID == RAWR_COMPILERID_MSVC
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

// --- Families. ---

#define RAWR_COMPILER_GNU 0
#if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
    #undef  RAWR_COMPILER_GNU
    #define RAWR_COMPILER_GNU 1
#endif

// --- Version checking. ---

#define RAWR_COMPILER_MIN_GCC_VERSION   11
#define RAWR_COMPILER_MIN_CLANG_VERSION 12
#define RAWR_COMPILER_MIN_MSVC_VERSION  1928
#ifndef RAWR_COMPILER_NO_VERSION_CHECKING
    #define RAWR_COMPILER_NO_VERSION_CHECKING 0
#endif

#if !RAWR_COMPILER_NO_VERSION_CHECKING
    #if RAWR_COMPILER_GCC
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_COMPILER_MIN_GCC_VERSION
            #error "rawr/compiler/detection.pp: GCC 11+ required (concepts correctness, NTTP structural types)"
        #endif
    #elif RAWR_COMPILER_CLANG
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_COMPILER_MIN_CLANG_VERSION
            #error "rawr/compiler/detection.pp: Clang 12+ required (concepts constraint subsumption correctness)"
        #endif
    #elif RAWR_COMPILER_MSVC
        #if _MSC_VER < RAWR_COMPILER_MIN_MSVC_VERSION
            #error "rawr/compiler/detection.pp: MSVC 16.8+ (_MSC_VER >= 1928) required (C++20 mode, concepts)"
        #endif
    #else
        #error "rawr/compiler/detection.pp: unrecognized compiler"
    #endif
#endif
