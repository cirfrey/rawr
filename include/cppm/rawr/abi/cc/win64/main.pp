#pragma once

#ifdef RAWR_METADATA
    #define RAWR_PP_MODULE rawr.abi.cc.win64.main.pp
#endif

#include "rawr/compiler/utils.pp"

#define RAWR_ABI_CC_WIN64_MAIN_NOCTX                      \
    _Pragma("comment(linker, \"/entry:rawr_user_main\")") \
    extern "C" RAWR_NORETURN void rawr_user_main() noexcept
