#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.detection.bin.pp
#endif

#include "rawr/detection/platform.pp"

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
