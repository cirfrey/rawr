// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.endian.pp
#endif

#include "rawr/detection/arch.local.pp"

// ============================================================
// Endianness
// ============================================================
#undef RAWR_ENDIAN_LITTLE
#define RAWR_ENDIAN_LITTLE  0
#undef RAWR_ENDIAN_BIG
#define RAWR_ENDIAN_BIG     0
#undef RAWR_ENDIAN_UNKNOWN
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
