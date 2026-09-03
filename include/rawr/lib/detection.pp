//// rawr/lib/detection.pp.
//
// We could cheat and ask the build system to sneak this info to us
// ... but whats the fun in that?
// Also, we want this to be somewhat portable, why rely on others?
//
// All macros default to 0 so consumers can write:
//     #if RAWR_COMPILER_CLANG && !RAWR_PLATFORM_WINDOWS
// instead of:
//     #if defined(RAWR_COMPILER_CLANG) && !defined(RAWR_PLATFORM_WINDOWS)
//
// Composite flags are defined in #if blocks to allow things like RAWR_PP_IF(COMPOSITE, ...),
// If we naiively did
//     #define RAWR_IS_64BIT (RAWR_PTR_SIZE == 8)
// The following expansion would happen
//     RAWR_PP_IF(RAWR_IS_64BIT, ...) => RAWR_PP_IF_(4 == 8)
// That doesn't compile, instead we define it inside an #if block and the expansion
// behaves as you'd expect.
// It's a little more work on our end, but thats what we're here for, right?
//
// TODO: maybe allow the user to override somehow, like if one of the values is set
//       then just skip detection?
#pragma once

/// Library constants and library feature detection:

#define RAWR_DETECTION_MIN_GCC_VERSION   11
#define RAWR_DETECTION_MIN_CLANG_VERSION 13
#define RAWR_DETECTION_MIN_MSVC_VERSION  1928

#ifndef RAWR_DETECTION_NO_COMPILER_ERROR
    #define RAWR_DETECTION_NO_COMPILER_ERROR 0
#else
    #if RAWR_DETECTION_NO_COMPILER_ERROR
        #undef  RAWR_DETECTION_NO_COMPILER_ERROR
        #define RAWR_DETECTION_NO_COMPILER_ERROR 1
    #else
        #undef  RAWR_DETECTION_NO_COMPILER_ERROR
        #define RAWR_DETECTION_NO_COMPILER_ERROR 0
    #endif
#endif


/// General detection:

// ============================================================
// Compiler
// ============================================================
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
            #error "rawr/lib/detection.pp: GCC 11+ required (concepts correctness, NTTP structural types)"
        #endif
    #elif RAWR_COMPILER_CLANG
        #if RAWR_COMPILER_VERSION_MAJOR < RAWR_DETECTION_MIN_CLANG_VERSION
            #error "rawr/lib/detection.pp: Clang 13+ required (concepts constraint subsumption correctness)"
        #endif
    #elif RAWR_COMPILER_MSVC
        #if _MSC_VER < RAWR_DETECTION_MIN_MSVC_VERSION
            #error "rawr/lib/detection.pp: MSVC 16.8+ (_MSC_VER >= 1928) required (C++20 mode, concepts)"
        #endif
    #else
        #error "rawr/lib/detection.pp: unrecognized compiler"
    #endif
#endif

// ============================================================
// C++ Standard
// ============================================================
// MSVC doesn't update __cplusplus without /Zc:__cplusplus.
// _MSVC_LANG is always correct on MSVC regardless.
#if defined(_MSVC_LANG)
    #define RAWR_CXX_VERSION _MSVC_LANG
#else
    #define RAWR_CXX_VERSION __cplusplus
#endif
// NOLINTBEGIN(modernize-macro-to-enum)
#define RAWR_CXX_VERSION_98 199711L
#define RAWR_CXX_VERSION_11 201103L
#define RAWR_CXX_VERSION_14 201402L
#define RAWR_CXX_VERSION_17 201703L
#define RAWR_CXX_VERSION_20 202002L
#define RAWR_CXX_VERSION_23 202302L
#define RAWR_CXX_VERSION_26 202603L
// NOLINTEND(modernize-macro-to-enum)


// ============================================================
// CPU Architecture
// ============================================================
#define RAWR_ARCH_X86     0
#define RAWR_ARCH_X64     0
#define RAWR_ARCH_ARM64   0
#define RAWR_ARCH_ARM32   0
#define RAWR_ARCH_RISCV64 0
#define RAWR_ARCH_RISCV32 0
#define RAWR_ARCH_XTENSA  0
#define RAWR_ARCH_AVR     0
#define RAWR_ARCH_WASM32  0
#define RAWR_ARCH_WASM64  0
#define RAWR_ARCH_PPC32   0
#define RAWR_ARCH_PPC64   0
#define RAWR_ARCH_LOONG64 0
#define RAWR_ARCH_MIPS32  0
#define RAWR_ARCH_MIPS64  0
#define RAWR_ARCH_S390    0
#define RAWR_ARCH_S390X   0
#define RAWR_ARCH_MSP430  0
#define RAWR_ARCH_SPARC32 0
#define RAWR_ARCH_SPARC64 0
#define RAWR_ARCH_SUPERH  0
#define RAWR_ARCH_UNKNOWN 0
// WASM first — Emscripten defines __i386 for legacy reasons
#if defined(__wasm64__)
    #undef  RAWR_ARCH_WASM64
    #define RAWR_ARCH_WASM64 1
#elif defined(__wasm32__) || defined(__wasm__)
    // Fallback to 32-bit if only __wasm__ is defined (legacy)
    #undef  RAWR_ARCH_WASM32
    #define RAWR_ARCH_WASM32 1
#elif defined(__x86_64__) || defined(_M_X64)
    #undef  RAWR_ARCH_X64
    #define RAWR_ARCH_X64 1
#elif defined(__i386__) || defined(__i386) || defined(_M_IX86)
    // __i386 (no trailing __) is the GCC spelling; _M_IX86 is MSVC
    #undef  RAWR_ARCH_X86
    #define RAWR_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #undef  RAWR_ARCH_ARM64
    #define RAWR_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
    #undef  RAWR_ARCH_ARM32
    #define RAWR_ARCH_ARM32 1
#elif defined(__riscv)
    // __riscv_xlen is always defined alongside __riscv
    #if __riscv_xlen == 64
        #undef  RAWR_ARCH_RISCV64
        #define RAWR_ARCH_RISCV64 1
    #else
        #undef  RAWR_ARCH_RISCV32
        #define RAWR_ARCH_RISCV32 1
    #endif
#elif defined(__XTENSA__)
    #undef  RAWR_ARCH_XTENSA
    #define RAWR_ARCH_XTENSA 1
#elif defined(__AVR__)
    #undef  RAWR_ARCH_AVR
    #define RAWR_ARCH_AVR 1
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
    // Check for 64-bit PowerPC (includes ELFv1, ELFv2, and MSVC definitions)
    #if defined(__powerpc64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        #undef  RAWR_ARCH_PPC64
        #define RAWR_ARCH_PPC64 1
    #else
        #undef  RAWR_ARCH_PPC32
        #define RAWR_ARCH_PPC32 1
    #endif
#elif defined(__loongarch64) || (defined(__loongarch__) && __loongarch_grlen == 64)
    #undef  RAWR_ARCH_LOONG64
    #define RAWR_ARCH_LOONG64 1
#elif defined(__mips__) || defined(_M_MRX000)
    // __mips64 or _MIPS_SIM / __mips register width checks
    #if defined(__mips64) || (defined(_MIPS_SIM) && _MIPS_SIM == _ABI64) || (defined(__mips_regsize) && __mips_regsize == 64)
        #undef  RAWR_ARCH_MIPS64
        #define RAWR_ARCH_MIPS64 1
    #else
        #undef  RAWR_ARCH_MIPS32
        #define RAWR_ARCH_MIPS32 1
    #endif
#elif defined(__s390x__) || defined(__s390__)
    // s390x is the 64-bit architecture; s390 is the legacy 32-bit
    #if defined(__s390x__)
        #undef  RAWR_ARCH_S390X
        #define RAWR_ARCH_S390X 1
    #else
        #undef  RAWR_ARCH_S390
        #define RAWR_ARCH_S390 1
    #endif
#elif defined(__MSP430__)
    #undef  RAWR_ARCH_MSP430
    #define RAWR_ARCH_MSP430 1
#elif defined(__sparc__) || defined(__sparc)
    // LEON processors (space-grade SPARC V8) are covered under __sparc__.
    // __sparcv9 and __arch64__ dictate 64-bit.
    #if defined(__sparcv9) || defined(__sparc_v9__) || defined(__arch64__)
        #undef  RAWR_ARCH_SPARC64
        #define RAWR_ARCH_SPARC64 1
    #else
        #undef  RAWR_ARCH_SPARC32
        #define RAWR_ARCH_SPARC32 1
    #endif
#elif defined(__sh__)
    // Covers SH-1 through SH-4 (SuperH)
    #undef  RAWR_ARCH_SUPERH
    #define RAWR_ARCH_SUPERH 1
#else
    #undef  RAWR_ARCH_UNKNOWN
    #define RAWR_ARCH_UNKNOWN 1
#endif

#define RAWR_ARCH_FAMILY_RISCV 0
#define RAWR_ARCH_FAMILY_X86   0
#define RAWR_ARCH_FAMILY_WASM  0
#define RAWR_ARCH_FAMILY_ARM   0
#define RAWR_ARCH_FAMILY_PPC   0
#define RAWR_ARCH_FAMILY_MIPS  0
#define RAWR_ARCH_FAMILY_S390  0
#define RAWR_ARCH_FAMILY_SPARC 0
#if RAWR_ARCH_RISCV64 || RAWR_ARCH_RISCV32
    #undef  RAWR_ARCH_FAMILY_RISCV
    #define RAWR_ARCH_FAMILY_RISCV 1
#endif
#if RAWR_ARCH_WASM32 || RAWR_ARCH_WASM64
    #undef  RAWR_ARCH_FAMILY_WASM
    #define RAWR_ARCH_FAMILY_WASM 1
#endif
#if RAWR_ARCH_X64 || RAWR_ARCH_X86
    #undef  RAWR_ARCH_FAMILY_X86
    #define RAWR_ARCH_FAMILY_X86 1
#endif
#if RAWR_ARCH_ARM64 || RAWR_ARCH_ARM32
    #undef  RAWR_ARCH_FAMILY_ARM
    #define RAWR_ARCH_FAMILY_ARM 1
#endif
#if RAWR_ARCH_PPC64 || RAWR_ARCH_PPC32
    #undef  RAWR_ARCH_FAMILY_PPC
    #define RAWR_ARCH_FAMILY_PPC 1
#endif
#if RAWR_ARCH_MIPS64 || RAWR_ARCH_MIPS32
    #undef  RAWR_ARCH_FAMILY_MIPS
    #define RAWR_ARCH_FAMILY_MIPS 1
#endif
#if RAWR_ARCH_S390X || RAWR_ARCH_S390
    #undef  RAWR_ARCH_FAMILY_S390
    #define RAWR_ARCH_FAMILY_S390 1
#endif
#if RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #undef  RAWR_ARCH_FAMILY_SPARC
    #define RAWR_ARCH_FAMILY_SPARC 1
#endif

// Defaults
#define RAWR_ARCH_X86_SSE     0
#define RAWR_ARCH_X86_SSE2    0
#define RAWR_ARCH_X86_SSE41   0
#define RAWR_ARCH_X86_SSE42   0
#define RAWR_ARCH_X86_AVX     0
#define RAWR_ARCH_X86_AVX2    0
#define RAWR_ARCH_X86_AVX512F 0
#define RAWR_ARCH_X86_FMA     0
#define RAWR_ARCH_X86_BMI1    0
#define RAWR_ARCH_X86_BMI2    0
#define RAWR_ARCH_X86_POPCNT  0
#define RAWR_ARCH_X86_LZCNT   0
#define RAWR_ARCH_X86_CLWB    0
#if RAWR_ARCH_FAMILY_X86
    // SSE / SSE2 are guaranteed on x64 by ABI, so we use ||, not &&
    #if RAWR_ARCH_X64 || defined(__SSE__)
        #undef  RAWR_ARCH_X86_SSE
        #define RAWR_ARCH_X86_SSE 1
    #endif
    #if RAWR_ARCH_X64 || defined(__SSE2__)
        #undef  RAWR_ARCH_X86_SSE2
        #define RAWR_ARCH_X86_SSE2 1
    #endif
    #if defined(__SSE4_1__)
        #undef  RAWR_ARCH_X86_SSE41
        #define RAWR_ARCH_X86_SSE41 1
    #endif
    #if defined(__SSE4_2__)
        #undef  RAWR_ARCH_X86_SSE42
        #define RAWR_ARCH_X86_SSE42 1
    #endif
    #if defined(__AVX__)
        #undef  RAWR_ARCH_X86_AVX
        #define RAWR_ARCH_X86_AVX 1
    #endif
    #if defined(__AVX2__)
        #undef  RAWR_ARCH_X86_AVX2
        #define RAWR_ARCH_X86_AVX2 1
    #endif
    #if defined(__AVX512F__)
        #undef  RAWR_ARCH_X86_AVX512F
        #define RAWR_ARCH_X86_AVX512F 1
    #endif
    #if defined(__FMA__)
        #undef  RAWR_ARCH_X86_FMA
        #define RAWR_ARCH_X86_FMA 1
    #endif
    #if defined(__BMI__)
        #undef  RAWR_ARCH_X86_BMI1
        #define RAWR_ARCH_X86_BMI1 1
    #endif
    #if defined(__BMI2__)
        #undef  RAWR_ARCH_X86_BMI2
        #define RAWR_ARCH_X86_BMI2 1
    #endif
    #if defined(__POPCNT__)
        #undef  RAWR_ARCH_X86_POPCNT
        #define RAWR_ARCH_X86_POPCNT 1
    #endif
    #if defined(__LZCNT__)
        #undef  RAWR_ARCH_X86_LZCNT
        #define RAWR_ARCH_X86_LZCNT 1
    #endif
    #if defined(__CLWB__)
        #undef  RAWR_ARCH_X86_CLWB
        #define RAWR_ARCH_X86_CLWB 1
    #endif
    // Note: MSVC exposes few independent ISA flags — only AVX/AVX2/AVX512 via /arch:.
    // CLWB, FMA, BMI etc. must be injected via -DRAWR_ARCH_X86_FEATURE_CLWB=1
    // in the build system when targeting those features on MSVC.
#endif

#define RAWR_ARCH_ARM_NEON       0
#define RAWR_ARCH_ARM_SVE        0
#define RAWR_ARCH_ARM_SVE2       0
#define RAWR_ARCH_ARM_DOTPROD    0
#define RAWR_ARCH_ARM_FP16       0
#define RAWR_ARCH_ARM_BF16       0
#define RAWR_ARCH_ARM_ATOMIC_CAS 0
#if RAWR_ARCH_FAMILY_ARM
    #if defined(__ARM_NEON)
        #undef  RAWR_ARCH_ARM_NEON
        #define RAWR_ARCH_ARM_NEON 1
    #endif
    #if defined(__ARM_FEATURE_SVE)
        #undef  RAWR_ARCH_ARM_SVE
        #define RAWR_ARCH_ARM_SVE 1
    #endif
    #if defined(__ARM_FEATURE_SVE2)
        #undef  RAWR_ARCH_ARM_SVE2
        #define RAWR_ARCH_ARM_SVE2 1
    #endif
    #if defined(__ARM_FEATURE_DOTPROD)
        #undef  RAWR_ARCH_ARM_DOTPROD
        #define RAWR_ARCH_ARM_DOTPROD 1
    #endif
    #if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
        #undef  RAWR_ARCH_ARM_FP16
        #define RAWR_ARCH_ARM_FP16 1
    #endif
    #if defined(__ARM_FEATURE_BF16)
        #undef  RAWR_ARCH_ARM_BF16
        #define RAWR_ARCH_ARM_BF16 1
    #endif
    // ARMv6-M (Cortex-M0/M0+/M1) is the one ARM32 profile that excludes
    // LDREX/STREX entirely. GCC/Clang define __ARM_ARCH_6M__ specifically
    // for it. Every other ARM32 profile (ARMv6T2+, ARMv7-M and up, all
    // ARMv7-A/R and later) has LDREX/STREX. AArch64 (RAWR_ARCH_ARM64) has
    // no equivalent gap — every AArch64 profile is A-class and always has
    // at least LDXR/STXR — so it's unconditionally capable and isn't
    // gated by this macro at all; it's handled as always-1 at point of use.
    #if !defined(__ARM_ARCH_6M__)
        #undef  RAWR_ARCH_ARM_ATOMIC_CAS
        #define RAWR_ARCH_ARM_ATOMIC_CAS 1
    #endif
#endif

#define RAWR_ARCH_RISCV_ATOMIC 0
#if RAWR_ARCH_FAMILY_RISCV
    // Set by GCC/Clang when the 'A' (atomic) extension is targeted
    // (-matomic, or an "a"-suffixed -march, e.g. rv32ima). Many minimal
    // embedded RV32I cores omit the A extension deliberately to save area.
    #if defined(__riscv_atomic)
        #undef  RAWR_ARCH_RISCV_ATOMIC
        #define RAWR_ARCH_RISCV_ATOMIC 1
    #endif
#endif

#define RAWR_ARCH_WASM_ATOMICS 0
#if RAWR_ARCH_FAMILY_WASM
    // Set by Clang when compiled with -matomics (the threads/atomics
    // proposal). Without it, a wasm module has no shared memory across
    // execution agents — see RAWR_ARCH_HAS_CAS below.
    #if defined(__wasm_atomics__)
        #undef  RAWR_ARCH_WASM_ATOMICS
        #define RAWR_ARCH_WASM_ATOMICS 1
    #endif
#endif

// ============================================================
// Host OS / Hardware Platform
// ============================================================
// TODO: detect wasi. #define RAWR_PLATFORM_WASI 0
#define RAWR_PLATFORM_LINUX   0
#define RAWR_PLATFORM_WINDOWS 0
#define RAWR_PLATFORM_FREEBSD 0
#define RAWR_PLATFORM_OPENBSD 0
#define RAWR_PLATFORM_NETBSD  0
#define RAWR_PLATFORM_MACOS   0
#define RAWR_PLATFORM_IOS     0
#define RAWR_PLATFORM_ANDROID 0
#define RAWR_PLATFORM_WASM    0
#define RAWR_PLATFORM_ESP32   0
#define RAWR_PLATFORM_ESP8266 0
#define RAWR_PLATFORM_STM32   0
#define RAWR_PLATFORM_NORDIC  0
#define RAWR_PLATFORM_PICO    0
#define RAWR_PLATFORM_TEENSY  0
#define RAWR_PLATFORM_AVR     0
#define RAWR_PLATFORM_UNKNOWN 0
// WASM: both Emscripten and standalone WASM runtimes
#if defined(__EMSCRIPTEN__) || defined(__wasm__)
    #undef  RAWR_PLATFORM_WASM
    #define RAWR_PLATFORM_WASM 1
// ESP32: ESP-IDF defines ESP_PLATFORM; Arduino framework defines ARDUINO_ARCH_ESP32
#elif defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
    #undef  RAWR_PLATFORM_ESP32
    #define RAWR_PLATFORM_ESP32 1
// ESP8266: Arduino framework
#elif defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
    #undef  RAWR_PLATFORM_ESP8266
    #define RAWR_PLATFORM_ESP8266 1
// Apple: requires TargetConditionals.h to distinguish iOS from macOS
#elif defined(__APPLE__)
    #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
        #undef  RAWR_PLATFORM_MACOS
        #define RAWR_PLATFORM_MACOS 1
    #elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || \
          defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)       || \
          defined(__ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__)
        #undef  RAWR_PLATFORM_IOS
        #define RAWR_PLATFORM_IOS 1
    #else
        #undef  RAWR_PLATFORM_UNKNOWN
        #define RAWR_PLATFORM_UNKNOWN 1
    #endif
// Android defines both __ANDROID__ and __linux__ — must come before Linux
#elif defined(__ANDROID__)
    #undef  RAWR_PLATFORM_ANDROID
    #define RAWR_PLATFORM_ANDROID 1
#elif defined(__linux__)
    #undef  linux // Some compilers define 'linux' as a bare macro alongside '__linux__',
                  // truly god has failed us.
    #undef  RAWR_PLATFORM_LINUX
    #define RAWR_PLATFORM_LINUX 1
// Windows: _WIN32 is defined on both 32 and 64-bit Windows including under MinGW and Cygwin.
// Cygwin and MinGW are distinguished at the ENV layer, not the HOST layer.
// HOST_WINDOWS means "running on Windows hardware", regardless of compatibility layer.
#elif defined(_WIN32) || defined(_WIN64)
    #undef  RAWR_PLATFORM_WINDOWS
    #define RAWR_PLATFORM_WINDOWS 1
// STM32: ST HAL or CubeMX defines USE_HAL_DRIVER; bare CMSIS defines STM32 family macros.
// STM32F0..STM32WL cover the main families.
#elif defined(USE_HAL_DRIVER)           || \
      defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || \
      defined(STM32F3) || defined(STM32F4) || defined(STM32F7) || \
      defined(STM32G0) || defined(STM32G4) || \
      defined(STM32H7) || defined(STM32L0) || defined(STM32L1) || \
      defined(STM32L4) || defined(STM32L5) || \
      defined(STM32U5) || defined(STM32WB) || defined(STM32WL)
    #undef  RAWR_PLATFORM_STM32
    #define RAWR_PLATFORM_STM32 1
// Nordic nRF: family macros cover nRF51 and nRF52 series including all variants.
// nRF9160 is a separate SiP but same SDK — include NRF9160 if needed.
#elif defined(NRF51) || defined(NRF52) || defined(NRF9160)
    #undef  RAWR_PLATFORM_NORDIC
    #define RAWR_PLATFORM_NORDIC 1
// Raspberry Pi Pico: Pico SDK defines PICO_BOARD; some configs define RASPBERRYPI_PICO
#elif defined(PICO_BOARD) || defined(RASPBERRYPI_PICO)
    #undef  RAWR_PLATFORM_PICO
    #define RAWR_PLATFORM_PICO 1
// Teensy: TEENSYDUINO covers all Teensy boards (3.x, 4.x) when using Teensyduino.
// Bare metal Teensy 4.x: __IMXRT1062__. Bare metal Teensy 3.x: __MK*__ variants.
#elif defined(TEENSYDUINO) || defined(__IMXRT1062__) || \
      defined(__MK20DX128__) || defined(__MK20DX256__) || \
      defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #undef  RAWR_PLATFORM_TEENSY
    #define RAWR_PLATFORM_TEENSY 1
#elif defined(__AVR__)
    #undef  RAWR_PLATFORM_AVR
    #define RAWR_PLATFORM_AVR 1
#elif defined(__FreeBSD__)
    #undef  RAWR_PLATFORM_FREEBSD
    #define RAWR_PLATFORM_FREEBSD 1
#elif defined(__OpenBSD__)
    #undef  RAWR_PLATFORM_OPENBSD
    #define RAWR_PLATFORM_OPENBSD 1
#elif defined(__NetBSD__)
    #undef  RAWR_PLATFORM_NETBSD
    #define RAWR_PLATFORM_NETBSD 1
#else
    #undef  RAWR_PLATFORM_UNKNOWN
    #define RAWR_PLATFORM_UNKNOWN 1
#endif

// ============================================================
// Environment Layer
// ============================================================
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

// ============================================================
// Calling Convention ABI
// ============================================================
// TODO: Wasm32 and wasm64 may need different abis.
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

// ============================================================
// C++ ABI
// ============================================================
// Determined by compiler and target. Describes what the compiler
// emits for __cxa_atexit, guards, vtables, RTTI, exception tables.
#define RAWR_CXX_ABI_ITANIUM 0  // GCC, Clang (non-MSVC target)
#define RAWR_CXX_ABI_MSVC    0  // MSVC, clang-cl
#define RAWR_CXX_ABI_UNKNOWN 0
// _MSC_VER is defined by both MSVC and clang-cl (Clang targeting MSVC ABI).
// That is the correct discriminator — it's about the target ABI, not the compiler.
#if defined(_MSC_VER)
    #undef  RAWR_CXX_ABI_MSVC
    #define RAWR_CXX_ABI_MSVC 1
#elif RAWR_COMPILER_GCC || RAWR_COMPILER_CLANG
    #undef  RAWR_CXX_ABI_ITANIUM
    #define RAWR_CXX_ABI_ITANIUM 1
#else
    #undef  RAWR_CXX_ABI_UNKNOWN
    #define RAWR_CXX_ABI_UNKNOWN 1
#endif

// ============================================================
// Binary Format
// ============================================================
#define RAWR_BIN_ELF     0
#define RAWR_BIN_MACHO   0
#define RAWR_BIN_PE      0
#define RAWR_BIN_WASM    0
#define RAWR_BIN_UNKNOWN 0
#if defined(__ELF__)
    #undef  RAWR_BIN_ELF
    #define RAWR_BIN_ELF 1
#elif defined(__MACH__)
    #undef  RAWR_BIN_MACHO
    #define RAWR_BIN_MACHO 1
#elif defined(_WIN32)
    #undef  RAWR_BIN_PE
    #define RAWR_BIN_PE 1
#elif RAWR_PLATFORM_WASM
    #undef  RAWR_BIN_WASM
    #define RAWR_BIN_WASM 1
#else
    #undef  RAWR_BIN_UNKNOWN
    #define RAWR_BIN_UNKNOWN 1
#endif

// ============================================================
// Pointer Width
// ============================================================
#if defined(__SIZEOF_POINTER__)
    #define RAWR_PTR_SIZE __SIZEOF_POINTER__
#elif RAWR_PLATFORM_WINDOWS && defined(_WIN64)
    #define RAWR_PTR_SIZE 8
#else
    #define RAWR_PTR_SIZE 4
#endif

#define RAWR_IS_64BIT 0
#define RAWR_IS_32BIT 0
#if RAWR_PTR_SIZE == 8
    #undef  RAWR_IS_64BIT
    #define RAWR_IS_64BIT 1
#endif
#if RAWR_PTR_SIZE == 4
    #undef  RAWR_IS_32BIT
    #define RAWR_IS_32BIT 1
#endif

// ============================================================
// Endianness
// ============================================================
#define RAWR_ENDIAN_LITTLE  0
#define RAWR_ENDIAN_BIG     0
#define RAWR_ENDIAN_UNKNOWN 0
// __BYTE_ORDER__ is the authoritative source — covers bi-endian ARM correctly
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #undef  RAWR_ENDIAN_LITTLE
        #define RAWR_ENDIAN_LITTLE 1
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #undef  RAWR_ENDIAN_BIG
        #define RAWR_ENDIAN_BIG 1
    #else
        #undef  RAWR_ENDIAN_UNKNOWN
        #define RAWR_ENDIAN_UNKNOWN 1
    #endif
#elif defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
    #undef  RAWR_ENDIAN_LITTLE
    #define RAWR_ENDIAN_LITTLE 1
#elif defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB)
    #undef  RAWR_ENDIAN_BIG
    #define RAWR_ENDIAN_BIG 1
// Fallback for toolchains without __BYTE_ORDER__ or not otherwise detectable:
#elif RAWR_ARCH_S390X || RAWR_ARCH_S390 || RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #undef  RAWR_ENDIAN_BIG
    #define RAWR_ENDIAN_BIG 1
#elif RAWR_ARCH_X64    || RAWR_ARCH_X86       || RAWR_ARCH_ARM64  || \
      RAWR_ARCH_ARM32  || RAWR_ARCH_RISCV64   || RAWR_ARCH_RISCV32|| \
      RAWR_ARCH_XTENSA || RAWR_ARCH_LOONG64   || RAWR_ARCH_AVR    || \
      RAWR_ARCH_WASM   || RAWR_ARCH_MSP430
    #undef  RAWR_ENDIAN_LITTLE
    #define RAWR_ENDIAN_LITTLE 1
#else
    #undef  RAWR_ENDIAN_UNKNOWN
    #define RAWR_ENDIAN_UNKNOWN 1
#endif

// ============================================================
// Utility/build Flags
// ============================================================

#ifdef __SIZEOF_INT128__
    #define RAWR_HAS_INT128 1
#else
    #define RAWR_HAS_INT128 0
#endif

// POSIX: meaningful syscall-level POSIX APIs exist.
// WASM deliberately excluded — Emscripten emulates POSIX in userspace,
// standalone WASM/WASI has a completely different interface.
// Cygwin provides POSIX over Windows, hence included.
#define RAWR_IS_POSIX 0
#if RAWR_PLATFORM_LINUX   || \
    RAWR_PLATFORM_MACOS   || \
    RAWR_PLATFORM_IOS     || \
    RAWR_PLATFORM_ANDROID || \
    RAWR_PLATFORM_OPENBSD || \
    RAWR_PLATFORM_FREEBSD || \
    RAWR_PLATFORM_NETBSD  || \
    RAWR_ENV_CYGWIN
    #undef  RAWR_IS_POSIX
    #define RAWR_IS_POSIX 1
#endif

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
    #define RAWR_HAS_EXCEPTIONS 1
#else
    #define RAWR_HAS_EXCEPTIONS 0
#endif

#if RAWR_COMPILER_FAMILY_GNU && defined(__GCC_HAVE_DWARF2_CFI_ASM)
    #define RAWR_HAS_CFI_ASM 1
#else
    #define RAWR_HAS_CFI_ASM 0
#endif


#if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
    #define RAWR_HAS_RTTI 1
#else
    #define RAWR_HAS_RTTI 0
#endif

// ============================================================
// Sanitizers & Mitigations
// ============================================================
// Guarding __has_feature inside an #elif chain prevents MSVC from
// aggressively expanding it and throwing C1012.

// --- Address Sanitizer (ASAN) ---
#if defined(__SANITIZE_ADDRESS__)
    #define RAWR_SAN_ASAN 1
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #define RAWR_SAN_ASAN 1
    #else
        #define RAWR_SAN_ASAN 0
    #endif
#else
    #define RAWR_SAN_ASAN 0
#endif

// --- Hardware-Assisted Address Sanitizer (HWASAN) ---
#if defined(__SANITIZE_HWADDRESS__)
    #define RAWR_SAN_HWASAN 1
#elif defined(__has_feature)
    #if __has_feature(hwaddress_sanitizer)
        #define RAWR_SAN_HWASAN 1
    #else
        #define RAWR_SAN_HWASAN 0
    #endif
#else
    #define RAWR_SAN_HWASAN 0
#endif

// --- Thread Sanitizer (TSAN) ---
#if defined(__SANITIZE_THREAD__)
    #define RAWR_SAN_TSAN 1
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define RAWR_SAN_TSAN 1
    #else
        #define RAWR_SAN_TSAN 0
    #endif
#else
    #define RAWR_SAN_TSAN 0
#endif

// --- Memory Sanitizer (MSAN) ---
#if defined(__SANITIZE_MEMORY__)
    #define RAWR_SAN_MSAN 1
#elif defined(__has_feature)
    #if __has_feature(memory_sanitizer)
        #define RAWR_SAN_MSAN 1
    #else
        #define RAWR_SAN_MSAN 0
    #endif
#else
    #define RAWR_SAN_MSAN 0
#endif

// --- Leak Sanitizer (LSAN) ---
#if defined(__SANITIZE_LEAK__)
    #define RAWR_SAN_LSAN 1
#elif defined(__has_feature)
    #if __has_feature(leak_sanitizer)
        #define RAWR_SAN_LSAN 1
    #else
        #define RAWR_SAN_LSAN 0
    #endif
#else
    #define RAWR_SAN_LSAN 0
#endif

// --- Undefined Behavior Sanitizer (UBSAN) ---
#if defined(__SANITIZE_UNDEFINED__)
    #define RAWR_SAN_UBSAN 1
#elif defined(__has_feature)
    #if __has_feature(undefined_behavior_sanitizer)
        #define RAWR_SAN_UBSAN 1
    #else
        #define RAWR_SAN_UBSAN 0
    #endif
#else
    #define RAWR_SAN_UBSAN 0
#endif

// --- Control Flow Guard / Integrity (CFG / CFI) ---
#if defined(_CONTROL_FLOW_GUARD)
    #define RAWR_SAN_CFI 1 // MSVC CFG
#elif defined(__has_feature)
    #if __has_feature(control_flow_integrity)
        #define RAWR_SAN_CFI 1 // Clang CFI
    #else
        #define RAWR_SAN_CFI 0
    #endif
#else
    #define RAWR_SAN_CFI 0
#endif

// --- SafeStack ---
#if defined(__has_feature)
    #if __has_feature(safe_stack)
        #define RAWR_SAN_SAFESTACK 1
    #else
        #define RAWR_SAN_SAFESTACK 0
    #endif
#else
    #define RAWR_SAN_SAFESTACK 0
#endif

// --- MSVC Runtime Checks (/RTC) ---
#if defined(__MSVC_RUNTIME_CHECKS)
    #define RAWR_SAN_RTC 1
#else
    #define RAWR_SAN_RTC 0
#endif

// Helper for "any sanitizer is active" (useful for tweaking timeouts or disabling optimizations)
// Note: Excludes CFI, SafeStack, and RTC as they usually don't dictate timeout adjustments.
#define RAWR_SAN_ANY_SANITIZER 0
#if RAWR_SAN_ASAN   || \
    RAWR_SAN_HWASAN || \
    RAWR_SAN_TSAN   || \
    RAWR_SAN_MSAN   || \
    RAWR_SAN_LSAN   || \
    RAWR_SAN_UBSAN
    #undef  RAWR_SAN_ANY_SANITIZER
    #define RAWR_SAN_ANY_SANITIZER 1
#endif
