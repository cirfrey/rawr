export module rawr.nostdlib.mem.memcpy.base;
import        rawr.intrin.mem;
#include     "rawr/compiler/utils.pp"

export namespace rawr::nostdlib::inline mem
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
