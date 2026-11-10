export module rawr.san.lsan;
import        rawr.intrin.base;
#include     "rawr/san/detection.pp"

namespace rawr::san::lsan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __lsan_ignore_object(void const*) -> void;
    extern "C" auto __lsan_do_leak_check()            -> void;
    extern "C" auto __lsan_disable      ()            -> void;
    extern "C" auto __lsan_enable       ()            -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

export namespace rawr::san::lsan
{
    inline constexpr bool compiled = RAWR_SANFEATURE_LSAN != 0;

    constexpr auto ignore_object([[maybe_unused]] void const* address) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__lsan_ignore_object(address); }
    }

    constexpr auto do_leak_check() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__lsan_do_leak_check(); }
    }

    constexpr auto disable() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__lsan_disable(); }
    }

    constexpr auto enable() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__lsan_enable(); }
    }
}
