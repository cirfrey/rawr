//// rawr/abi/win64.pp.
#pragma once

#include "rawr/lib/attributes.pp"

// rawr_main_ctx only exists to have ctx be a dependent name in the user's main.
// Its a template since MSVC is bad at inlining lambdas.
#define RAWR_ABI_WIN64_MAIN(...)                                                             \
    _Pragma("comment(linker, \"/entry:rawr_main\")")                                         \
    RAWR_ALWAYS_INLINE RAWR_NORETURN void rawr_main_ctx(auto& ctx) noexcept { __VA_ARGS__; } \
    extern "C" RAWR_NORETURN void rawr_main() noexcept { auto c = 0; rawr_main_ctx(c); }

#define RAWR_ABI_WIN64_MAIN_NOCTX(...)               \
    _Pragma("comment(linker, \"/entry:rawr_main\")") \
    extern "C" RAWR_NORETURN void rawr_main() noexcept { __VA_ARGS__; }
