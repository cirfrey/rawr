// GENERATED — do not edit
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #undef RAWR_ENDIAN_LITTLE
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #undef RAWR_ENDIAN_BIG
    #else
        #undef RAWR_ENDIAN_UNKNOWN
    #endif
#elif defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
    #undef RAWR_ENDIAN_LITTLE
#elif defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB)
    #undef RAWR_ENDIAN_BIG
#elif RAWR_ARCH_S390X || RAWR_ARCH_S390 || RAWR_ARCH_SPARC64 || RAWR_ARCH_SPARC32
    #undef RAWR_ENDIAN_BIG
#elif RAWR_ARCH_X64    || RAWR_ARCH_X86       || RAWR_ARCH_ARM64  || RAWR_ARCH_ARM32  || RAWR_ARCH_RISCV64   || RAWR_ARCH_RISCV32|| RAWR_ARCH_XTENSA || RAWR_ARCH_LOONG64   || RAWR_ARCH_AVR    || RAWR_ARCH_WASM   || RAWR_ARCH_MSP430
    #undef RAWR_ENDIAN_LITTLE
#else
    #undef RAWR_ENDIAN_UNKNOWN
#endif
#undef RAWR_ENDIAN_UNKNOWN
#undef RAWR_ENDIAN_BIG
#undef RAWR_ENDIAN_LITTLE
#include "rawr/detection/arch.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
