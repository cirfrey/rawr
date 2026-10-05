// GENERATED — do not edit
#pragma once

#include "rawr/lib/intrin/mem.hpp"
#include "rawr/lib/compiler.local.pp"

namespace rawr::nostdlib::inline mem
{
    RAWR_ALWAYS_INLINE
    auto memcpy(void* dst, void const* src, decltype(sizeof(0)) n) -> void*
    {
        return rawr::intrin::mem::soft::memcpy(
            reinterpret_cast<unsigned char*>(dst),
            reinterpret_cast<unsigned char const*>(src),
            n
        );
    }
}

// ── end-of-header scope: undo pp fragments and macros (FILO)
#include "rawr/lib/compiler.undef.pp"
