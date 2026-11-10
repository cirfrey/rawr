#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.arch.endian.pp
#endif

#include "rawr/arch/detection.pp"

// --- Ids. ---

#define RAWR_ARCH_ENDIANID_UNKNOWN 1
#define RAWR_ARCH_ENDIANID_LE      2
#define RAWR_ARCH_ENDIANID_BE      3
#define RAWR_ARCH_ENDIANID_MAX     3

// --- Detection. ---

// __BYTE_ORDER__ is the authoritative source — covers bi-endian ARM correctly
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_LE
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_BE
    #else
        #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_UNKNOWN
    #endif
#elif defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
    #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_LE
#elif defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB)
    #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_BE
// Fallback for toolchains without __BYTE_ORDER__ or not otherwise detectable:
#elif RAWR_ARCH_S390X || RAWR_ARCH_S390 || RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_BE
#elif                    \
    RAWR_ARCH_X86     || \
    RAWR_ARCH_ARM     || \
    RAWR_ARCH_RISCV   || \
    RAWR_ARCH_WASM    || \
    RAWR_ARCH_XTENSA  || \
    RAWR_ARCH_LOONG64 || \
    RAWR_ARCH_AVR     || \
    RAWR_ARCH_MSP430
    #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_LE
#else
    #define RAWR_DETECTED_ARCH_ENDIANID RAWR_ARCH_ENDIANID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_ARCH_ENDIANID=RAWR_ARCH_ENDIANID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_ARCH_ENDIANID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_ARCH_ENDIANID
    #error "rawr: RAWR_ARCH_ENDIANID is an output of detection; to override it define RAWR_SET_ARCH_ENDIANID"
#endif
#ifdef RAWR_SET_ARCH_ENDIANID
    #if RAWR_SET_ARCH_ENDIANID < 1 || RAWR_SET_ARCH_ENDIANID > RAWR_ARCH_ENDIANID_MAX
        #error "rawr: RAWR_SET_ARCH_ENDIANID must be one of the RAWR_ARCH_ENDIANID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_ARCH_ENDIANID           RAWR_SET_ARCH_ENDIANID
    #define RAWR_ARCH_ENDIANID_DETECTED  0
    #define RAWR_ARCH_ENDIANID_OVERRIDEN 1
#else
    #define RAWR_ARCH_ENDIANID           RAWR_DETECTED_ARCH_ENDIANID
    #define RAWR_ARCH_ENDIANID_DETECTED  1
    #define RAWR_ARCH_ENDIANID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ARCH_LE             0
#define RAWR_ARCH_BE             0
#define RAWR_ARCH_ENDIAN_UNKNOWN 0
#if RAWR_ARCH_ENDIANID == RAWR_ARCH_ENDIANID_UNKNOWN
    #undef  RAWR_ARCH_ENDIAN_UNKNOWN
    #define RAWR_ARCH_ENDIAN_UNKNOWN 1
#elif RAWR_ARCH_ENDIANID == RAWR_ARCH_ENDIANID_LE
    #undef  RAWR_ARCH_LE
    #define RAWR_ARCH_LE 1
#elif RAWR_ARCH_ENDIANID == RAWR_ARCH_ENDIANID_BE
    #undef  RAWR_ARCH_BE
    #define RAWR_ARCH_BE 1
#endif
