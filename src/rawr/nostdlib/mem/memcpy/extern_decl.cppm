export module rawr.nostdlib.mem.memcpy.extern_decl;
import        rawr.nostdlib.mem.memcpy.base;

#if !RAWR_AMALGAM || (RAWR_AMALGAM && (RAWR_AMALGAM_NOSTDLIB || RAWR_AMALGAM_NOSTDLIB_MEMCPY))
    export extern "C" {
        auto memcpy(void* dst, void* const src, decltype(sizeof(0)) n) -> void*
        { return rawr::nostdlib::memcpy(dst, src, n); }
    }
#endif
