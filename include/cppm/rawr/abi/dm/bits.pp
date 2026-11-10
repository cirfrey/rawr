#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.abi.dm.bits.pp
#endif

#if defined(__CHAR_BIT__)
    #define RAWR_ABI_DM_BITS_IN_BYTE __CHAR_BIT__
#elif defined(_MSC_VER)
    #define RAWR_ABI_DM_BITS_IN_BYTE 8
#else
    #ifndef RAWR_ABI_DM_BITS_IN_BYTE
        #error How many bits in byte?
    #endif
#endif
