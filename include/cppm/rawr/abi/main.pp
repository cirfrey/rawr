#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.abi.main.pp
#endif

#include "rawr/abi/cc/detection.pp"

#if RAWR_ABI_CC_SYSV
    #include "rawr/abi/cc/sysv/main.pp"
    #define RAWR_ABI_MAIN(...)  RAWR_ABI_CC_SYSV_MAIN(__VA_ARGS__)
    #define RAWR_ABI_MAIN_NOCTX RAWR_ABI_CC_SYSV_MAIN_NOCTX
#elif RAWR_ABI_CC_WIN64
    #include "rawr/abi/cc/win64/main.pp"
    #define RAWR_ABI_MAIN(...)  RAWR_ABI_CC_WIN64_MAIN_NOCTX
    #define RAWR_ABI_MAIN_NOCTX RAWR_ABI_CC_WIN64_MAIN_NOCTX
#endif
