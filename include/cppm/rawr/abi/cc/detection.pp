// An ABI is two independent facts:
//   cc : calling convention - register usage, stack layout, entry parameter passing (what rawr's trampolines must obey)
//   dm : data model         - the widths of int / long / pointer (what struct layouts and size_t-like types depend on)
// They vary independently: x32 is (SysV, ILP32), Cygwin x64 is (Win64, LP64), MinGW x64 is (Win64, LLP64),
// watchOS arm64_32 is (AAPCS64 Apple, ILP32), RISC-V is (RISCV, ILP32|LP64).
#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.abi.cc.detection.pp
#endif

#include "rawr/arch/detection.pp"
#include "rawr/platform/detection.pp"
#include "rawr/distribution/todo.pp"

RAWR_TODO("Wasm32 and wasm64 may need different abis.")
RAWR_TODO("IA32 has several calling conventions (cdecl/stdcall/fastcall/...) and no id yet.")
RAWR_TODO("Float ABI (soft/softfp/hard) is part of the calling convention on ARM32 and RISC-V and is not modeled yet.")

// --- Ids. ---

#define RAWR_ABI_CCID_UNKNOWN        1
#define RAWR_ABI_CCID_SYSV           2
#define RAWR_ABI_CCID_WIN64          3
#define RAWR_ABI_CCID_AAPCS64        4
#define RAWR_ABI_CCID_AAPCS64_APPLE  5
#define RAWR_ABI_CCID_AAPCS64_WIN    6
#define RAWR_ABI_CCID_ARM64EC        7
#define RAWR_ABI_CCID_AAPCS32        8
#define RAWR_ABI_CCID_RISCV          9
#define RAWR_ABI_CCID_XTENSA         10
#define RAWR_ABI_CCID_AVR            11
#define RAWR_ABI_CCID_WASM           12
#define RAWR_ABI_CCID_MAX            12

// --- Detection. ---

#if RAWR_ARCH_WASM
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_WASM
#elif RAWR_ARCH_X64
    #if RAWR_PLATFORM_WINDOWS
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_WIN64
    #else
        // x86-64 on Linux, macOS (pre-M1), BSD, Android all use SysV AMD64
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_SYSV
    #endif
#elif RAWR_ARCH_ARM64
    #if defined(__arm64ec__) || defined(_M_ARM64EC)
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_ARM64EC
    #elif RAWR_PLATFORM_WINDOWS
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_AAPCS64_WIN
    #elif RAWR_PLATFORM_MACOS || RAWR_PLATFORM_IOS
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_AAPCS64_APPLE
    #else
        #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_AAPCS64
    #endif
#elif RAWR_ARCH_ARM32
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_AAPCS32
#elif RAWR_ARCH_RISCV
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_RISCV
#elif RAWR_ARCH_XTENSA
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_XTENSA
#elif RAWR_ARCH_AVR
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_AVR
#else
    #define RAWR_DETECTED_ABI_CCID RAWR_ABI_CCID_UNKNOWN
#endif

// --- User override. ---

// -DRAWR_SET_ABI_CCID=RAWR_ABI_CCID_<VALUE> replaces detection (UNKNOWN is a legal value). RAWR_DETECTED_ABI_CCID always keeps what
// detection found. Id 0 is reserved as "malformed": a misspelled name evaluates to 0.
#ifdef RAWR_ABI_CCID
    #error "rawr: RAWR_ABI_CCID is an output of detection; to override it define RAWR_SET_ABI_CCID"
#endif
#ifdef RAWR_SET_ABI_CCID
    #if RAWR_SET_ABI_CCID < 1 || RAWR_SET_ABI_CCID > RAWR_ABI_CCID_MAX
        #error "rawr: RAWR_SET_ABI_CCID must be one of the RAWR_ABI_CCID_* values (a misspelled name evaluates to 0, which is never valid)"
    #endif
    #define RAWR_ABI_CCID           RAWR_SET_ABI_CCID
    #define RAWR_ABI_CCID_DETECTED  0
    #define RAWR_ABI_CCID_OVERRIDEN 1
#else
    #define RAWR_ABI_CCID           RAWR_DETECTED_ABI_CCID
    #define RAWR_ABI_CCID_DETECTED  1
    #define RAWR_ABI_CCID_OVERRIDEN 0
#endif

// --- Canonical macros. ---

#define RAWR_ABI_CC_SYSV          0  // x86-64 SysV AMD64 - Linux, macOS, BSD
#define RAWR_ABI_CC_WIN64         0  // Microsoft x64 - Windows x86-64 (also Cygwin/MinGW)
#define RAWR_ABI_CC_AAPCS64       0  // AArch64 PCS - Linux, bare metal AArch64
#define RAWR_ABI_CC_AAPCS64_APPLE 0  // AArch64 Apple variant - Apple Silicon, iOS
#define RAWR_ABI_CC_AAPCS64_WIN   0  // AArch64 Windows variant (x18 = TEB, own variadic rules)
#define RAWR_ABI_CC_ARM64EC       0  // Arm64EC: ARM64 code using the x64-compatible ABI
#define RAWR_ABI_CC_AAPCS32       0  // ARM 32-bit PCS
#define RAWR_ABI_CC_RISCV         0  // RISC-V psABI (the data model is RAWR_DM_*)
#define RAWR_ABI_CC_XTENSA        0  // Xtensa - ESP32 (windowed or call0, toolchain decides)
#define RAWR_ABI_CC_AVR           0  // AVR - 8-bit Atmel/Microchip
#define RAWR_ABI_CC_WASM          0  // WebAssembly
#define RAWR_ABI_CC_UNKNOWN       0
#if RAWR_ABI_CCID == RAWR_ABI_CCID_UNKNOWN
    #undef  RAWR_ABI_CC_UNKNOWN
    #define RAWR_ABI_CC_UNKNOWN 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_SYSV
    #undef  RAWR_ABI_CC_SYSV
    #define RAWR_ABI_CC_SYSV 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_WIN64
    #undef  RAWR_ABI_CC_WIN64
    #define RAWR_ABI_CC_WIN64 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_AAPCS64
    #undef  RAWR_ABI_CC_AAPCS64
    #define RAWR_ABI_CC_AAPCS64 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_AAPCS64_APPLE
    #undef  RAWR_ABI_CC_AAPCS64_APPLE
    #define RAWR_ABI_CC_AAPCS64_APPLE 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_AAPCS64_WIN
    #undef  RAWR_ABI_CC_AAPCS64_WIN
    #define RAWR_ABI_CC_AAPCS64_WIN 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_ARM64EC
    #undef  RAWR_ABI_CC_ARM64EC
    #define RAWR_ABI_CC_ARM64EC 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_AAPCS32
    #undef  RAWR_ABI_CC_AAPCS32
    #define RAWR_ABI_CC_AAPCS32 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_RISCV
    #undef  RAWR_ABI_CC_RISCV
    #define RAWR_ABI_CC_RISCV 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_XTENSA
    #undef  RAWR_ABI_CC_XTENSA
    #define RAWR_ABI_CC_XTENSA 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_AVR
    #undef  RAWR_ABI_CC_AVR
    #define RAWR_ABI_CC_AVR 1
#elif RAWR_ABI_CCID == RAWR_ABI_CCID_WASM
    #undef  RAWR_ABI_CC_WASM
    #define RAWR_ABI_CC_WASM 1
#endif
