// GENERATED — do not edit

#if RAWRSCAN_METADATA
    #undef RAWR_PP_MODULE
    #define RAWR_PP_MODULE rawr.abi.win64.main.pp
#endif

#include "rawr/lib/compiler.local.pp"

#undef RAWR_ABI_WIN64_MAIN_NOCTX
#define RAWR_ABI_WIN64_MAIN_NOCTX                         \
    _Pragma("comment(linker, \"/entry:rawr_user_main\")") \
    extern "C" RAWR_NORETURN void rawr_user_main() noexcept
