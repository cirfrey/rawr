export module rawr.san.msan;
import        rawr.lib.integer.raw;
import        rawr.intrin.base;
#include     "rawr/san/detection.pp"

namespace rawr::san::msan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __msan_poison                  (void const volatile*, rst) -> void;
    extern "C" auto __msan_unpoison                (void const volatile*, rst) -> void;
    extern "C" auto __msan_check_mem_is_initialized(void const volatile*, rst) -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

export namespace rawr::san::msan
{
    inline constexpr bool compiled = RAWR_SANFEATURE_MSAN != 0;

    constexpr auto poison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__msan_poison(address, size); }
    }

    constexpr auto unpoison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__msan_unpoison(address, size); }
    }

    constexpr auto check_initialized(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    {
        if (intrin::is_consteval()) { return; }
        if constexpr(compiled) { detail::__msan_check_mem_is_initialized(address, size); }
    }
}
