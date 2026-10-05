// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.arch.pp
#endif

#undef RAWR_ARCH_X86
#define RAWR_ARCH_X86     0
#undef RAWR_ARCH_X64
#define RAWR_ARCH_X64     0
#undef RAWR_ARCH_ARM64
#define RAWR_ARCH_ARM64   0
#undef RAWR_ARCH_ARM32
#define RAWR_ARCH_ARM32   0
#undef RAWR_ARCH_RISCV64
#define RAWR_ARCH_RISCV64 0
#undef RAWR_ARCH_RISCV32
#define RAWR_ARCH_RISCV32 0
#undef RAWR_ARCH_XTENSA
#define RAWR_ARCH_XTENSA  0
#undef RAWR_ARCH_AVR
#define RAWR_ARCH_AVR     0
#undef RAWR_ARCH_WASM32
#define RAWR_ARCH_WASM32  0
#undef RAWR_ARCH_WASM64
#define RAWR_ARCH_WASM64  0
#undef RAWR_ARCH_PPC32
#define RAWR_ARCH_PPC32   0
#undef RAWR_ARCH_PPC64
#define RAWR_ARCH_PPC64   0
#undef RAWR_ARCH_LOONG64
#define RAWR_ARCH_LOONG64 0
#undef RAWR_ARCH_MIPS32
#define RAWR_ARCH_MIPS32  0
#undef RAWR_ARCH_MIPS64
#define RAWR_ARCH_MIPS64  0
#undef RAWR_ARCH_S390
#define RAWR_ARCH_S390    0
#undef RAWR_ARCH_S390X
#define RAWR_ARCH_S390X   0
#undef RAWR_ARCH_MSP430
#define RAWR_ARCH_MSP430  0
#undef RAWR_ARCH_SPARC32
#define RAWR_ARCH_SPARC32 0
#undef RAWR_ARCH_SPARC64
#define RAWR_ARCH_SPARC64 0
#undef RAWR_ARCH_SUPERH
#define RAWR_ARCH_SUPERH  0
#undef RAWR_ARCH_UNKNOWN
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

#undef RAWR_ARCH_FAMILY_RISCV
#define RAWR_ARCH_FAMILY_RISCV 0
#undef RAWR_ARCH_FAMILY_X86
#define RAWR_ARCH_FAMILY_X86   0
#undef RAWR_ARCH_FAMILY_WASM
#define RAWR_ARCH_FAMILY_WASM  0
#undef RAWR_ARCH_FAMILY_ARM
#define RAWR_ARCH_FAMILY_ARM   0
#undef RAWR_ARCH_FAMILY_PPC
#define RAWR_ARCH_FAMILY_PPC   0
#undef RAWR_ARCH_FAMILY_MIPS
#define RAWR_ARCH_FAMILY_MIPS  0
#undef RAWR_ARCH_FAMILY_S390
#define RAWR_ARCH_FAMILY_S390  0
#undef RAWR_ARCH_FAMILY_SPARC
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
#undef RAWR_ARCH_X86_SSE
#define RAWR_ARCH_X86_SSE     0
#undef RAWR_ARCH_X86_SSE2
#define RAWR_ARCH_X86_SSE2    0
#undef RAWR_ARCH_X86_SSE41
#define RAWR_ARCH_X86_SSE41   0
#undef RAWR_ARCH_X86_SSE42
#define RAWR_ARCH_X86_SSE42   0
#undef RAWR_ARCH_X86_AVX
#define RAWR_ARCH_X86_AVX     0
#undef RAWR_ARCH_X86_AVX2
#define RAWR_ARCH_X86_AVX2    0
#undef RAWR_ARCH_X86_AVX512F
#define RAWR_ARCH_X86_AVX512F 0
#undef RAWR_ARCH_X86_FMA
#define RAWR_ARCH_X86_FMA     0
#undef RAWR_ARCH_X86_BMI1
#define RAWR_ARCH_X86_BMI1    0
#undef RAWR_ARCH_X86_BMI2
#define RAWR_ARCH_X86_BMI2    0
#undef RAWR_ARCH_X86_POPCNT
#define RAWR_ARCH_X86_POPCNT  0
#undef RAWR_ARCH_X86_LZCNT
#define RAWR_ARCH_X86_LZCNT   0
#undef RAWR_ARCH_X86_CLWB
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

#undef RAWR_ARCH_ARM_NEON
#define RAWR_ARCH_ARM_NEON       0
#undef RAWR_ARCH_ARM_SVE
#define RAWR_ARCH_ARM_SVE        0
#undef RAWR_ARCH_ARM_SVE2
#define RAWR_ARCH_ARM_SVE2       0
#undef RAWR_ARCH_ARM_DOTPROD
#define RAWR_ARCH_ARM_DOTPROD    0
#undef RAWR_ARCH_ARM_FP16
#define RAWR_ARCH_ARM_FP16       0
#undef RAWR_ARCH_ARM_BF16
#define RAWR_ARCH_ARM_BF16       0
#undef RAWR_ARCH_ARM_ATOMIC_CAS
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

#undef RAWR_ARCH_RISCV_ATOMIC
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

#undef RAWR_ARCH_WASM_ATOMICS
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
