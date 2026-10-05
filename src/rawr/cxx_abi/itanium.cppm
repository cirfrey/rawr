export module rawr.cxx_abi.itanium;
#include     "rawr/lib/compiler.pp"

export namespace rawr::cxx_abi::itanium
{
    using cxa_atexit_fn = void(*)(void*);

    RAWR_ALTERNATENAME("cxa_atexit", "__cxa_atexit")
    extern "C" auto cxa_atexit(
        cxa_atexit_fn callback,
        void* arg,
        void* dso
    ) noexcept -> int
    RAWR_ASM("__cxa_atexit");
}
