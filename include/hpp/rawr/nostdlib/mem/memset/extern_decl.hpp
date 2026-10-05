// GENERATED — do not edit
#pragma once

#include "rawr/nostdlib/mem/memset/base.hpp"

#if !RAWR_AMALGAM || (RAWR_AMALGAM && (RAWR_AMALGAM_NOSTDLIB || RAWR_AMALGAM_NOSTDLIB_MEMSET))
    extern "C" {
        auto memset(void* dst, int val, decltype(sizeof(0)) n) -> void*
        { return rawr::nostdlib::memset(dst, val, n); }
    }
#endif
