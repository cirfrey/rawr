export module rawr.nostdlib.mem.memset.extern_decl;
import        rawr.nostdlib.mem.memset.base;
#include     "rawr/distribution/amalgam.pp"

export extern "C" {
    auto memset(void* dst, int val, decltype(sizeof(0)) n) -> void*
    RAWR_AMALGAM_NOSTDLIB_GATED(RAWR_AMALGAM_NOSTDLIB_MEMSET,
        { return rawr::nostdlib::memset(dst, val, n); }
    );
}
