export module rawr.nostdlib.mem.memcpy.extern_decl;
import        rawr.nostdlib.mem.memcpy.base;
#include     "rawr/distribution/amalgam.pp"

export extern "C" {
    auto memcpy(void* dst, void* const src, decltype(sizeof(0)) n) -> void*
        RAWR_AMALGAM_NOSTDLIB_GATED(RAWR_AMALGAM_NOSTDLIB_MEMCPY,
            { return rawr::nostdlib::memcpy(dst, src, n); }
        );
    }
