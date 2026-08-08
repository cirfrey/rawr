//// rawr/cxx/itanium.hpp
#pragma region rawr/cxx/itanium.hpp

#ifdef RAWR_UNITY
    #pragma once
    #include "rawr/cxx/intrin.hpp"
#endif

namespace rawr::cxx::itanium
{
    using cxa_atexit_fn = void(*)(void*);

    RAWR_SYMBOL_ALIAS_PRAGMA("cxa_atexit", "__cxa_atexit")
    extern "C" int cxa_atexit(
        cxa_atexit_fn fn,
        void* arg,
        void* dso
    ) noexcept RAWR_ASM_ALIAS("__cxa_atexit");
}

#pragma endregion rawr/cxx/itanium.hpp
