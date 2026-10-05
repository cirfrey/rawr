// GENERATED — do not edit
#pragma once

#include "rawr/lib/integer/raw.hpp"
#include "rawr/lib/intrin/base.hpp"
#include "rawr/detection/san.local.pp"

namespace rawr::san::msan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __msan_poison                  (void const volatile*, rst) -> void;
    extern "C" auto __msan_unpoison                (void const volatile*, rst) -> void;
    extern "C" auto __msan_check_mem_is_initialized(void const volatile*, rst) -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

namespace rawr::san::msan
{
    inline constexpr bool compiled = RAWR_SAN_MSAN != 0;

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

// ── end-of-header scope: undo pp fragments and macros (FILO)
#include "rawr/detection/san.undef.pp"
