//// rawr/cxx_abi/itanium.hpp.

#ifdef RAWR_MODULE
    export module rawr.cxx_abi.itanium;
    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"

RAWR_EXPORT namespace rawr::cxx_abi::itanium
{
    using cxa_atexit_fn = void(*)(void*);

    RAWR_SYMBOL_ALIAS_PRAGMA("cxa_atexit", "__cxa_atexit")
    extern "C" auto cxa_atexit(
        cxa_atexit_fn callback,
        void* arg,
        void* dso
    ) noexcept -> int
    RAWR_ASM_ALIAS("__cxa_atexit");
}
