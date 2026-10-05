#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.abi.win64.main.pp
#endif

#include "rawr/lib/compiler.pp"

#define RAWR_ABI_WIN64_MAIN_NOCTX                         \
    _Pragma("comment(linker, \"/entry:rawr_user_main\")") \
    extern "C" RAWR_NORETURN void rawr_user_main() noexcept
