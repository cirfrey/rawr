export module rawr.nostdlib.mem.memset.extern_decl;
import        rawr.nostdlib.mem.memset.base;

#if !RAWR_AMALGAM || (RAWR_AMALGAM && (RAWR_AMALGAM_NOSTDLIB || RAWR_AMALGAM_NOSTDLIB_MEMSET))
    export extern "C" {
        auto memset(void* dst, int val, decltype(sizeof(0)) n) -> void*
        { return rawr::nostdlib::memset(dst, val, n); }
    }
#endif
