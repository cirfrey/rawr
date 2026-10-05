// GENERATED — do not edit
#pragma once

#include "rawr/lib/compiler.local.pp"

namespace rawr::cxx_abi::itanium
{
    using cxa_atexit_fn = void(*)(void*);

    RAWR_ALTERNATENAME("cxa_atexit", "__cxa_atexit")
    extern "C" auto cxa_atexit(
        cxa_atexit_fn callback,
        void* arg,
        void* dso
    ) noexcept -> int
    RAWR_ASM("__cxa_atexit");
}

// ── end-of-header scope: undo pp fragments and macros (FILO)
#include "rawr/lib/compiler.undef.pp"
