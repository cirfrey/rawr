// GENERATED — do not edit
#pragma once

#include "rawr/nostdlib/mem/memcpy/base.hpp"

#if !RAWR_AMALGAM || (RAWR_AMALGAM && (RAWR_AMALGAM_NOSTDLIB || RAWR_AMALGAM_NOSTDLIB_MEMCPY))
    extern "C" {
        auto memcpy(void* dst, void* const src, decltype(sizeof(0)) n) -> void*
        { return rawr::nostdlib::memcpy(dst, src, n); }
    }
#endif
