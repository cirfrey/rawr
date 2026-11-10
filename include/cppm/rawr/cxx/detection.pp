#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.cxx.detection.pp
#endif

#include "rawr/compiler/detection.pp"

// --- Ids. ---

#define RAWR_CXXID_UNKNOWN        1
#define RAWR_CXXID_ITANIUM        2
#define RAWR_CXXID_MSVC           3
#define RAWR_CXXID_MAX            3

// --- Detection. ---

// _MSC_VER is defined by both MSVC and clang-cl (Clang targeting MSVC ABI).
// That is the correct discriminator — it's about the target ABI, not the compiler.
#if defined(_MSC_VER)
    #define RAWR_DETECTED_CXXID RAWR_CXXID_MSVC
#elif RAWR_COMPILER_GCC || RAWR_COMPILER_CLANG
    #define RAWR_DETECTED_CXXID RAWR_CXXID_ITANIUM
#else
    #define RAWR_DETECTED_CXXID RAWR_CXXID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_CXXID=RAWR_CXXID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_CXXID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_CXXID
    #error "rawr: RAWR_CXXID is an output of detection; to override it define RAWR_SET_CXXID"
#endif
#ifdef RAWR_SET_CXXID
    #if RAWR_SET_CXXID < 1 || RAWR_SET_CXXID > RAWR_CXXID_MAX
        #error "rawr: RAWR_SET_CXXID must be one of the RAWR_CXXID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_CXXID           RAWR_SET_CXXID
    #define RAWR_CXXID_DETECTED  0
    #define RAWR_CXXID_OVERRIDEN 1
#else
    #define RAWR_CXXID           RAWR_DETECTED_CXXID
    #define RAWR_CXXID_DETECTED  1
    #define RAWR_CXXID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#if RAWR_CXXID == RAWR_CXXID_UNKNOWN
    #undef  RAWR_CXX_UNKNOWN
    #define RAWR_CXX_UNKNOWN 1
#elif RAWR_CXXID == RAWR_CXXID_ITANIUM
    #undef  RAWR_CXX_ITANIUM
    #define RAWR_CXX_ITANIUM 1
#elif RAWR_CXXID == RAWR_CXXID_MSVC
    #undef  RAWR_CXX_MSVC
    #define RAWR_CXX_MSVC 1
#endif
