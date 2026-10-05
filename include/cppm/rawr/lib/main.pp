#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.lib.main.pp
#endif

#include "rawr/detection/abi.pp"

#if RAWR_ABI_SYSV
    #include "rawr/abi/sysv/main.pp"
    #define RAWR_MAIN(...)  RAWR_ABI_SYSV_MAIN(__VA_ARGS__)
    #define RAWR_MAIN_NOCTX RAWR_ABI_SYSV_MAIN_NOCTX
#elif RAWR_ABI_WIN64
    #include "rawr/abi/win64/main.pp"
    #define RAWR_MAIN(...)  RAWR_ABI_WIN64_MAIN_NOCTX
    #define RAWR_MAIN_NOCTX RAWR_ABI_WIN64_MAIN_NOCTX
#endif
