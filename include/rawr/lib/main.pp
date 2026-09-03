#pragma once

#include "rawr/lib/detection.pp"

#if RAWR_ABI_SYSV
    #include "rawr/abi/sysv.pp"
    #define RAWR_MAIN(...)       RAWR_ABI_SYSV_MAIN(__VA_ARGS__)
    #define RAWR_MAIN_NOCTX(...) RAWR_ABI_SYSV_MAIN_NOCTX(__VA_ARGS__)
#elif RAWR_ABI_WIN64
    #include "rawr/abi/win64.pp"
    #define RAWR_MAIN(...)       RAWR_ABI_WIN64_MAIN(__VA_ARGS__)
    #define RAWR_MAIN_NOCTX(...) RAWR_ABI_WIN64_MAIN_NOCTX(__VA_ARGS__)
#endif
