//// rawr/san/lsan.hpp.

#ifdef RAWR_MODULE
    export module rawr.san.lsan;
    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::san::lsan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" void __lsan_ignore_object(void const*);
    extern "C" void __lsan_do_leak_check();
    extern "C" void __lsan_disable();
    extern "C" void __lsan_enable();
    // NOLINTEND(bugprone-reserved-identifier)
}

RAWR_EXPORT namespace rawr::san::lsan
{
    inline constexpr bool compiled = RAWR_SAN_LSAN != 0;

    inline auto ignore_object([[maybe_unused]] void const* address) noexcept -> void
    { if constexpr(compiled) { detail::__lsan_ignore_object(address); } }

    inline auto do_leak_check() noexcept -> void
    { if constexpr(compiled) { detail::__lsan_do_leak_check(); } }

    inline auto disable() noexcept -> void
    { if constexpr(compiled) { detail::__lsan_disable(); } }

    inline auto enable() noexcept -> void
    { if constexpr(compiled) { detail::__lsan_enable(); } }
}
