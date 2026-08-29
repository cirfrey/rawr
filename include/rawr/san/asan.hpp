//// rawr/san/asan.hpp.
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.integer.raw;
#else
    #include "rawr/lib/integer/raw.hpp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::san::asan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __asan_poison_memory_region(void const volatile*, rst) -> void;
    extern "C" auto __asan_unpoison_memory_region(void const volatile*, rst) -> void;
    extern "C" auto __asan_address_is_poisoned(void const volatile*) -> int;
    extern "C" auto __asan_region_is_poisoned(void*, rst) -> void*;
    extern "C" auto __asan_handle_no_return() -> void;
    extern "C" auto __asan_set_error_report_callback(void (*)(const char*)) -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

RAWR_EXPORT namespace rawr::san::asan
{
    inline constexpr bool compiled = RAWR_SAN_ASAN != 0;

    inline auto poison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    { if constexpr(compiled) { detail::__asan_poison_memory_region(address, size); } }

    inline auto unpoison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    { if constexpr(compiled) { detail::__asan_unpoison_memory_region(address, size); } }


    inline auto poisoned([[maybe_unused]] void const* address) noexcept -> bool
    {
        if constexpr(compiled) { return detail::__asan_address_is_poisoned(address) != 0; }
        else                   { return false; }
    }

    inline auto first_poisoned(
        [[maybe_unused]] void* address,
        [[maybe_unused]] rst size
    ) noexcept -> void*
    {
        if constexpr(compiled) { return detail::__asan_region_is_poisoned(address, size); }
        else                   { return nullptr; }
    }

    inline auto handle_no_return() noexcept -> void
    { if constexpr(compiled) { detail::__asan_handle_no_return(); } }

    using report_callback = void(*)(const char*);
    inline auto set_error_report_callback([[maybe_unused]] report_callback callback) noexcept -> void
    { if constexpr(compiled) { detail::__asan_set_error_report_callback(callback); } }
}
