// GENERATED — do not edit
#pragma once

#include "rawr/lib/intrin/mem.hpp"
#include "rawr/lib/compiler.local.pp"

namespace rawr::nostdlib::inline mem
{
    RAWR_ALWAYS_INLINE
    auto memset(void* dst, int val, decltype(sizeof(0)) n) -> void*
    { return rawr::intrin::mem::soft::memset(dst, val, n); }
}

// ── end-of-header scope: undo pp fragments and macros (FILO)
#include "rawr/lib/compiler.undef.pp"
