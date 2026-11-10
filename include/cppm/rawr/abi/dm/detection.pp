// An ABI is two independent facts:
//   cc : calling convention - register usage, stack layout, entry parameter passing (what rawr's trampolines must obey)
//   dm : data model         - the widths of int / long / pointer (what struct layouts and size_t-like types depend on)
// They vary independently: x32 is (SysV, ILP32), Cygwin x64 is (Win64, LP64), MinGW x64 is (Win64, LLP64),
// watchOS arm64_32 is (AAPCS64 Apple, ILP32), RISC-V is (RISCV, ILP32|LP64).
#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.abi.dm.detection.pp
#endif

// --- Ids. ---

#define RAWR_ABI_DMID_UNKNOWN 1
#define RAWR_ABI_DMID_ILP32   2
#define RAWR_ABI_DMID_LP64    3
#define RAWR_ABI_DMID_LLP64   4
#define RAWR_ABI_DMID_ILP64   5
#define RAWR_ABI_DMID_LP32    6
#define RAWR_ABI_DMID_IP16    7
#define RAWR_ABI_DMID_MAX     7

// --- Detection. ---

#if defined(__SIZEOF_INT__) && defined(__SIZEOF_LONG__) && defined(__SIZEOF_POINTER__)
    #if   __SIZEOF_INT__ == 4 && __SIZEOF_LONG__ == 4 && __SIZEOF_POINTER__ == 4
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_ILP32
    #elif __SIZEOF_INT__ == 4 && __SIZEOF_LONG__ == 8 && __SIZEOF_POINTER__ == 8
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_LP64
    #elif __SIZEOF_INT__ == 4 && __SIZEOF_LONG__ == 4 && __SIZEOF_POINTER__ == 8
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_LLP64
    #elif __SIZEOF_INT__ == 8 && __SIZEOF_LONG__ == 8 && __SIZEOF_POINTER__ == 8
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_ILP64
    #elif __SIZEOF_INT__ == 2 && __SIZEOF_LONG__ == 4 && __SIZEOF_POINTER__ == 4
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_LP32
    #elif __SIZEOF_INT__ == 2 && __SIZEOF_LONG__ == 4 && __SIZEOF_POINTER__ == 2
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_IP16
    #else
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_UNKNOWN
    #endif
#elif defined(_MSC_VER)
    // MSVC defines no __SIZEOF_*: int and long are 32-bit on every MSVC target, only the pointer varies.
    #if defined(_WIN64)
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_LLP64
    #else
        #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_ILP32
    #endif
#else
    #define RAWR_DETECTED_ABI_DMID RAWR_ABI_DMID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_DMID=RAWR_DMID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_DMID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_ABI_DMID
    #error "rawr: RAWR_ABI_DMID is an output of detection; to override it define RAWR_SET_ABI_DMID"
#endif
#ifdef RAWR_SET_ABI_DMID
    #if RAWR_SET_ABI_DMID < 1 || RAWR_SET_ABI_DMID > RAWR_ABI_DMID_MAX
        #error "rawr: RAWR_SET_ABI_DMID must be one of the RAWR_ABI_DMID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_ABI_DMID           RAWR_SET_ABI_DMID
    #define RAWR_ABI_DMID_DETECTED  0
    #define RAWR_ABI_DMID_OVERRIDEN 1
#else
    #define RAWR_ABI_DMID           RAWR_DETECTED_ABI_DMID
    #define RAWR_ABI_DMID_DETECTED  1
    #define RAWR_ABI_DMID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ABI_DM_ILP32   0  // int 32, long 32, ptr 32 - 32-bit Linux/Windows/ARM/RISC-V/wasm32, x32, arm64_32
#define RAWR_ABI_DM_LP64    0  // int 32, long 64, ptr 64 - 64-bit Linux/macOS/BSD/Android, Cygwin
#define RAWR_ABI_DM_LLP64   0  // int 32, long 32, ptr 64 - 64-bit Windows (MSVC, MinGW)
#define RAWR_ABI_DM_ILP64   0  // int 64, long 64, ptr 64 - rare
#define RAWR_ABI_DM_LP32    0  // int 16, long 32, ptr 32 - rare
#define RAWR_ABI_DM_IP16    0  // int 16, long 32, ptr 16 - AVR, small-model MSP430
#define RAWR_ABI_DM_UNKNOWN 0
#if RAWR_ABI_DMID == RAWR_ABI_DMID_UNKNOWN
    #undef  RAWR_ABI_DM_UNKNOWN
    #define RAWR_ABI_DM_UNKNOWN 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_ILP32
    #undef  RAWR_ABI_DM_ILP32
    #define RAWR_ABI_DM_ILP32 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_LP64
    #undef  RAWR_ABI_DM_LP64
    #define RAWR_ABI_DM_LP64 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_LLP64
    #undef  RAWR_ABI_DM_LLP64
    #define RAWR_ABI_DM_LLP64 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_ILP64
    #undef  RAWR_ABI_DM_ILP64
    #define RAWR_ABI_DM_ILP64 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_LP32
    #undef  RAWR_ABI_DM_LP32
    #define RAWR_ABI_DM_LP32 1
#elif RAWR_ABI_DMID == RAWR_ABI_DMID_IP16
    #undef  RAWR_ABI_DM_IP16
    #define RAWR_ABI_DM_IP16 1
#endif

// --- Families. ---

#define RAWR_ABI_DM_PTR64 0
#define RAWR_ABI_DM_PTR32 0
#if RAWR_ABI_DM_LP64 || RAWR_ABI_DM_LLP64 || RAWR_ABI_DM_ILP64
    #undef  RAWR_ABI_DM_PTR64
    #define RAWR_ABI_DM_PTR64 1
#elif RAWR_ABI_DM_ILP32 || RAWR_ABI_DM_LP32
    #undef  RAWR_ABI_DM_PTR32
    #define RAWR_ABI_DM_PTR32 1
#endif
