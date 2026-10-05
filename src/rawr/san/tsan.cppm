export module rawr.san.tsan;
import rawr.lib.intrin.base;
#include "rawr/detection/san.pp"

namespace rawr::san::tsan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __tsan_acquire            (void*) -> void;
    extern "C" auto __tsan_release            (void*) -> void;
    extern "C" auto __tsan_ignore_reads_begin ()      -> void;
    extern "C" auto __tsan_ignore_reads_end   ()      -> void;
    extern "C" auto __tsan_ignore_writes_begin()      -> void;
    extern "C" auto __tsan_ignore_writes_end  ()      -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

export namespace rawr::san::tsan
{
    inline constexpr bool compiled = RAWR_SAN_TSAN != 0;

    constexpr auto acquire([[maybe_unused]] void* address) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_acquire(address); }
    }

    constexpr auto release([[maybe_unused]] void* address) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_release(address); }
    }

    constexpr auto ignore_reads_begin() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_ignore_reads_begin(); }
    }

    constexpr auto ignore_reads_end() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_ignore_reads_end(); }
    }

    constexpr auto ignore_writes_begin() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_ignore_writes_begin(); }
    }

    constexpr auto ignore_writes_end() noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__tsan_ignore_writes_end(); }
    }
}
