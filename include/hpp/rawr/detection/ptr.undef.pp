// GENERATED — do not edit
#if defined(__CHAR_BIT__)
    #undef RAWR_BITS_IN_BYTE
#elif RAWR_COMPILER_MSVC
    #undef RAWR_BITS_IN_BYTE
#endif
#if RAWR_PTR_SIZE == 4
    #undef RAWR_IS_32BIT
#endif
#if RAWR_PTR_SIZE == 8
    #undef RAWR_IS_64BIT
#endif
#undef RAWR_IS_32BIT
#undef RAWR_IS_64BIT
#if defined(__SIZEOF_POINTER__)
    #undef RAWR_PTR_SIZE
#elif RAWR_PLATFORM_WINDOWS && defined(_WIN64)
    #undef RAWR_PTR_SIZE
#else
    #undef RAWR_PTR_SIZE
#endif
#include "rawr/detection/compiler.undef.pp"
#include "rawr/detection/platform.undef.pp"
#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
#endif
