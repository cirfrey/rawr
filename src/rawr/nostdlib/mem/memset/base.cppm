export module rawr.nostdlib.mem.memset.base;
import        rawr.intrin.mem;
#include     "rawr/compiler/utils.pp"

export namespace rawr::nostdlib::inline mem
{
    RAWR_ALWAYS_INLINE
    auto memset(void* dst, int val, decltype(sizeof(0)) n) -> void*
    { return rawr::intrin::mem::soft::memset(dst, val, n); }
}
