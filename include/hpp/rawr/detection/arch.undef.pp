// GENERATED — do not edit
#if RAWR_ARCH_FAMILY_WASM
    #if defined(__wasm_atomics__)
        #undef RAWR_ARCH_WASM_ATOMICS
    #endif
#endif
#undef RAWR_ARCH_WASM_ATOMICS
#if RAWR_ARCH_FAMILY_RISCV
    #if defined(__riscv_atomic)
        #undef RAWR_ARCH_RISCV_ATOMIC
    #endif
#endif
#undef RAWR_ARCH_RISCV_ATOMIC
#if RAWR_ARCH_FAMILY_ARM
    #if !defined(__ARM_ARCH_6M__)
        #undef RAWR_ARCH_ARM_ATOMIC_CAS
    #endif
    #if defined(__ARM_FEATURE_BF16)
        #undef RAWR_ARCH_ARM_BF16
    #endif
    #if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
        #undef RAWR_ARCH_ARM_FP16
    #endif
    #if defined(__ARM_FEATURE_DOTPROD)
        #undef RAWR_ARCH_ARM_DOTPROD
    #endif
    #if defined(__ARM_FEATURE_SVE2)
        #undef RAWR_ARCH_ARM_SVE2
    #endif
    #if defined(__ARM_FEATURE_SVE)
        #undef RAWR_ARCH_ARM_SVE
    #endif
    #if defined(__ARM_NEON)
        #undef RAWR_ARCH_ARM_NEON
    #endif
#endif
#undef RAWR_ARCH_ARM_ATOMIC_CAS
#undef RAWR_ARCH_ARM_BF16
#undef RAWR_ARCH_ARM_FP16
#undef RAWR_ARCH_ARM_DOTPROD
#undef RAWR_ARCH_ARM_SVE2
#undef RAWR_ARCH_ARM_SVE
#undef RAWR_ARCH_ARM_NEON
#if RAWR_ARCH_FAMILY_X86
    #if defined(__CLWB__)
        #undef RAWR_ARCH_X86_CLWB
    #endif
    #if defined(__LZCNT__)
        #undef RAWR_ARCH_X86_LZCNT
    #endif
    #if defined(__POPCNT__)
        #undef RAWR_ARCH_X86_POPCNT
    #endif
    #if defined(__BMI2__)
        #undef RAWR_ARCH_X86_BMI2
    #endif
    #if defined(__BMI__)
        #undef RAWR_ARCH_X86_BMI1
    #endif
    #if defined(__FMA__)
        #undef RAWR_ARCH_X86_FMA
    #endif
    #if defined(__AVX512F__)
        #undef RAWR_ARCH_X86_AVX512F
    #endif
    #if defined(__AVX2__)
        #undef RAWR_ARCH_X86_AVX2
    #endif
    #if defined(__AVX__)
        #undef RAWR_ARCH_X86_AVX
    #endif
    #if defined(__SSE4_2__)
        #undef RAWR_ARCH_X86_SSE42
    #endif
    #if defined(__SSE4_1__)
        #undef RAWR_ARCH_X86_SSE41
    #endif
    #if RAWR_ARCH_X64 || defined(__SSE2__)
        #undef RAWR_ARCH_X86_SSE2
    #endif
    #if RAWR_ARCH_X64 || defined(__SSE__)
        #undef RAWR_ARCH_X86_SSE
    #endif
#endif
#undef RAWR_ARCH_X86_CLWB
#undef RAWR_ARCH_X86_LZCNT
#undef RAWR_ARCH_X86_POPCNT
#undef RAWR_ARCH_X86_BMI2
#undef RAWR_ARCH_X86_BMI1
#undef RAWR_ARCH_X86_FMA
#undef RAWR_ARCH_X86_AVX512F
#undef RAWR_ARCH_X86_AVX2
#undef RAWR_ARCH_X86_AVX
#undef RAWR_ARCH_X86_SSE42
#undef RAWR_ARCH_X86_SSE41
#undef RAWR_ARCH_X86_SSE2
#undef RAWR_ARCH_X86_SSE
#if RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #undef RAWR_ARCH_FAMILY_SPARC
#endif
#if RAWR_ARCH_S390X || RAWR_ARCH_S390
    #undef RAWR_ARCH_FAMILY_S390
#endif
#if RAWR_ARCH_MIPS64 || RAWR_ARCH_MIPS32
    #undef RAWR_ARCH_FAMILY_MIPS
#endif
#if RAWR_ARCH_PPC64 || RAWR_ARCH_PPC32
    #undef RAWR_ARCH_FAMILY_PPC
#endif
#if RAWR_ARCH_ARM64 || RAWR_ARCH_ARM32
    #undef RAWR_ARCH_FAMILY_ARM
#endif
#if RAWR_ARCH_X64 || RAWR_ARCH_X86
    #undef RAWR_ARCH_FAMILY_X86
#endif
#if RAWR_ARCH_WASM32 || RAWR_ARCH_WASM64
    #undef RAWR_ARCH_FAMILY_WASM
#endif
#if RAWR_ARCH_RISCV64 || RAWR_ARCH_RISCV32
    #undef RAWR_ARCH_FAMILY_RISCV
#endif
#undef RAWR_ARCH_FAMILY_SPARC
#undef RAWR_ARCH_FAMILY_S390
#undef RAWR_ARCH_FAMILY_MIPS
#undef RAWR_ARCH_FAMILY_PPC
#undef RAWR_ARCH_FAMILY_ARM
#undef RAWR_ARCH_FAMILY_WASM
#undef RAWR_ARCH_FAMILY_X86
#undef RAWR_ARCH_FAMILY_RISCV
#if defined(__wasm64__)
    #undef RAWR_ARCH_WASM64
#elif defined(__wasm32__) || defined(__wasm__)
    #undef RAWR_ARCH_WASM32
#elif defined(__x86_64__) || defined(_M_X64)
    #undef RAWR_ARCH_X64
#elif defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #undef RAWR_ARCH_X86
#elif defined(__aarch64__) || defined(_M_ARM64)
    #undef RAWR_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
    #undef RAWR_ARCH_ARM32
#elif defined(__riscv)
    #if __riscv_xlen == 64
        #undef RAWR_ARCH_RISCV64
    #else
        #undef RAWR_ARCH_RISCV32
    #endif
#elif defined(__XTENSA__)
    #undef RAWR_ARCH_XTENSA
#elif defined(__AVR__)
    #undef RAWR_ARCH_AVR
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
    #if defined(__powerpc64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        #undef RAWR_ARCH_PPC64
    #else
        #undef RAWR_ARCH_PPC32
    #endif
#elif defined(__loongarch64) || (defined(__loongarch__) && __loongarch_grlen == 64)
    #undef RAWR_ARCH_LOONG64
#elif defined(__mips__) || defined(_M_MRX000)
    #if defined(__mips64) || (defined(_MIPS_SIM) && _MIPS_SIM == _ABI64) || (defined(__mips_regsize) && __mips_regsize == 64)
        #undef RAWR_ARCH_MIPS64
    #else
        #undef RAWR_ARCH_MIPS32
    #endif
#elif defined(__s390x__) || defined(__s390__)
    #if defined(__s390x__)
        #undef RAWR_ARCH_S390X
    #else
        #undef RAWR_ARCH_S390
    #endif
#elif defined(__MSP430__)
    #undef RAWR_ARCH_MSP430
#elif defined(__sparc__) || defined(__sparc)
    #if defined(__sparcv9) || defined(__sparc_v9__) || defined(__arch64__)
        #undef RAWR_ARCH_SPARC64
    #else
        #undef RAWR_ARCH_SPARC32
    #endif
#elif defined(__sh__)
    #undef RAWR_ARCH_SUPERH
#else
    #undef RAWR_ARCH_UNKNOWN
#endif
#undef RAWR_ARCH_UNKNOWN
#undef RAWR_ARCH_SUPERH
#undef RAWR_ARCH_SPARC64
#undef RAWR_ARCH_SPARC32
#undef RAWR_ARCH_MSP430
#undef RAWR_ARCH_S390X
#undef RAWR_ARCH_S390
#undef RAWR_ARCH_MIPS64
#undef RAWR_ARCH_MIPS32
#undef RAWR_ARCH_LOONG64
#undef RAWR_ARCH_PPC64
#undef RAWR_ARCH_PPC32
#undef RAWR_ARCH_WASM64
#undef RAWR_ARCH_WASM32
#undef RAWR_ARCH_AVR
#undef RAWR_ARCH_XTENSA
#undef RAWR_ARCH_RISCV32
#undef RAWR_ARCH_RISCV64
#undef RAWR_ARCH_ARM32
#undef RAWR_ARCH_ARM64
#undef RAWR_ARCH_X64
#undef RAWR_ARCH_X86
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
