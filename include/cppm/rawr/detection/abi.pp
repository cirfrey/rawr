#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.abi.pp
#endif

#include "rawr/detection/arch.pp"
#include "rawr/detection/platform.pp"
#include "rawr/distribution/todo.pp"

RAWR_TODO("Wasm32 and wasm64 may need different abis.")

// Determined by CPU + host. Describes the register usage, stack layout,
// and entry parameter passing convention rawr's trampolines must conform to.
#define RAWR_ABI_SYSV          0  // x86-64 SysV AMD64 — Linux, macOS, BSD
#define RAWR_ABI_WIN64         0  // Microsoft x64 — Windows x86-64
#define RAWR_ABI_AAPCS64       0  // AArch64 PCS — Linux, bare metal AArch64
#define RAWR_ABI_AAPCS64_APPLE 0  // AArch64 Apple variant — Apple Silicon, iOS
#define RAWR_ABI_AAPCS32       0  // ARM 32-bit PCS
#define RAWR_ABI_RISCV_LP64    0  // RISC-V LP64 — 64-bit RISC-V
#define RAWR_ABI_RISCV_ILP32   0  // RISC-V ILP32 — 32-bit RISC-V
#define RAWR_ABI_XTENSA        0  // Xtensa — ESP32 (windowed or call0, toolchain decides)
#define RAWR_ABI_AVR           0  // AVR — 8-bit Atmel/Microchip
#define RAWR_ABI_WASM          0  // WebAssembly
#define RAWR_ABI_UNKNOWN       0
#if RAWR_ARCH_WASM
    #undef  RAWR_ABI_WASM
    #define RAWR_ABI_WASM 1
#elif RAWR_ARCH_X64
    #if RAWR_PLATFORM_WINDOWS
        #undef  RAWR_ABI_WIN64
        #define RAWR_ABI_WIN64 1
    #else
        // x86-64 on Linux, macOS (pre-M1), BSD, Android all use SysV AMD64
        #undef  RAWR_ABI_SYSV
        #define RAWR_ABI_SYSV 1
    #endif
#elif RAWR_ARCH_ARM64
    #if RAWR_PLATFORM_MACOS || RAWR_PLATFORM_IOS
        #undef  RAWR_ABI_AAPCS64_APPLE
        #define RAWR_ABI_AAPCS64_APPLE 1
    #else
        #undef  RAWR_ABI_AAPCS64
        #define RAWR_ABI_AAPCS64 1
    #endif
#elif RAWR_ARCH_ARM32
    #undef  RAWR_ABI_AAPCS32
    #define RAWR_ABI_AAPCS32 1
#elif RAWR_ARCH_RISCV64
    #undef  RAWR_ABI_RISCV_LP64
    #define RAWR_ABI_RISCV_LP64 1
#elif RAWR_ARCH_RISCV32
    #undef  RAWR_ABI_RISCV_ILP32
    #define RAWR_ABI_RISCV_ILP32 1
#elif RAWR_ARCH_XTENSA
    #undef  RAWR_ABI_XTENSA
    #define RAWR_ABI_XTENSA 1
#elif RAWR_ARCH_AVR
    #undef  RAWR_ABI_AVR
    #define RAWR_ABI_AVR 1
#else
    #undef  RAWR_ABI_UNKNOWN
    #define RAWR_ABI_UNKNOWN 1
#endif
