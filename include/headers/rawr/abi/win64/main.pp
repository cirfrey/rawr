#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/abi/win64/main.pp"
#endif
#pragma once

#include "rawr/lib/attributes.pp"

#define RAWR_ABI_WIN64_MAIN_NOCTX                         \
    _Pragma("comment(linker, \"/entry:rawr_user_main\")") \
    extern "C" RAWR_NORETURN void rawr_user_main() noexcept
