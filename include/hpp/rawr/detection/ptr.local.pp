// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.detection.ptr.pp
#endif

#include "rawr/detection/platform.local.pp"
#include "rawr/detection/compiler.local.pp"

#if defined(__SIZEOF_POINTER__)
    #undef RAWR_PTR_SIZE
    #define RAWR_PTR_SIZE __SIZEOF_POINTER__
#elif RAWR_PLATFORM_WINDOWS && defined(_WIN64)
    #undef RAWR_PTR_SIZE
    #define RAWR_PTR_SIZE 8
#else
    #undef RAWR_PTR_SIZE
    #define RAWR_PTR_SIZE 4
#endif

#undef RAWR_IS_64BIT
#define RAWR_IS_64BIT 0
#undef RAWR_IS_32BIT
#define RAWR_IS_32BIT 0
#if RAWR_PTR_SIZE == 8
    #undef  RAWR_IS_64BIT
    #define RAWR_IS_64BIT 1
#endif
#if RAWR_PTR_SIZE == 4
    #undef  RAWR_IS_32BIT
    #define RAWR_IS_32BIT 1
#endif

#if defined(__CHAR_BIT__)
    #undef RAWR_BITS_IN_BYTE
    #define RAWR_BITS_IN_BYTE __CHAR_BIT__
#elif RAWR_COMPILER_MSVC
    #undef RAWR_BITS_IN_BYTE
    #define RAWR_BITS_IN_BYTE 8
#else
    #ifndef RAWR_BITS_IN_BYTE
        #error How many bits in byte?
    #endif
#endif
