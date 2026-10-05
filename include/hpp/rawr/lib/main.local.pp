// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.lib.main.pp
#endif

#include "rawr/detection/abi.local.pp"

#if RAWR_ABI_SYSV
    #include "rawr/abi/sysv/main.local.pp"
    #undef RAWR_MAIN
    #define RAWR_MAIN(...)  RAWR_ABI_SYSV_MAIN(__VA_ARGS__)
    #undef RAWR_MAIN_NOCTX
    #define RAWR_MAIN_NOCTX RAWR_ABI_SYSV_MAIN_NOCTX
#elif RAWR_ABI_WIN64
    #include "rawr/abi/win64/main.local.pp"
    #undef RAWR_MAIN
    #define RAWR_MAIN(...)  RAWR_ABI_WIN64_MAIN_NOCTX
    #undef RAWR_MAIN_NOCTX
    #define RAWR_MAIN_NOCTX RAWR_ABI_WIN64_MAIN_NOCTX
#endif
