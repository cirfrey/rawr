#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.bin.detection.pp
#endif

#include "rawr/platform/detection.pp"

// --- Ids. ---

#define RAWR_BINID_UNKNOWN 1
#define RAWR_BINID_ELF     2
#define RAWR_BINID_MACHO   3
#define RAWR_BINID_PE      4
#define RAWR_BINID_WASM    5
#define RAWR_BINID_MAX     5

// --- Detection. ---

#if defined(__ELF__)
    #define RAWR_DETECTED_BINID RAWR_BINID_ELF
#elif defined(__MACH__)
    #define RAWR_DETECTED_BINID RAWR_BINID_MACHO
#elif defined(_WIN32) || defined(__CYGWIN__)  // Cygwin is PE/COFF but defines no _WIN32
    #define RAWR_DETECTED_BINID RAWR_BINID_PE
#elif RAWR_PLATFORM_WASM
    #define RAWR_DETECTED_BINID RAWR_BINID_WASM
#else
    #define RAWR_DETECTED_BINID RAWR_BINID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_BINID=RAWR_BINID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_BINID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_BINID
    #error "rawr: RAWR_BINID is an output of detection; to override it define RAWR_SET_BINID"
#endif
#ifdef RAWR_SET_BINID
    #if RAWR_SET_BINID < 1 || RAWR_SET_BINID > RAWR_BINID_MAX
        #error "rawr: RAWR_SET_BINID must be one of the RAWR_BINID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_BINID           RAWR_SET_BINID
    #define RAWR_BINID_DETECTED  0
    #define RAWR_BINID_OVERRIDEN 1
#else
    #define RAWR_BINID           RAWR_DETECTED_BINID
    #define RAWR_BINID_DETECTED  1
    #define RAWR_BINID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_BIN_UNKNOWN 0
#define RAWR_BIN_ELF     0
#define RAWR_BIN_MACHO   0
#define RAWR_BIN_PE      0
#define RAWR_BIN_WASM    0
#if RAWR_BINID == RAWR_BINID_UNKNOWN
    #undef  RAWR_BIN_UNKNOWN
    #define RAWR_BIN_UNKNOWN 1
#elif RAWR_BINID == RAWR_BINID_ELF
    #undef  RAWR_BIN_ELF
    #define RAWR_BIN_ELF 1
#elif RAWR_BINID == RAWR_BINID_MACHO
    #undef  RAWR_BIN_MACHO
    #define RAWR_BIN_MACHO 1
#elif RAWR_BINID == RAWR_BINID_PE
    #undef  RAWR_BIN_PE
    #define RAWR_BIN_PE 1
#elif RAWR_BINID == RAWR_BINID_WASM
    #undef  RAWR_BIN_WASM
    #define RAWR_BIN_WASM 1
#endif
