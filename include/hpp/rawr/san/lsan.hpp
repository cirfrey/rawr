// GENERATED — do not edit
#pragma once

#include "rawr/lib/intrin/base.hpp"
#include "rawr/detection/san.local.pp"

namespace rawr::san::lsan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" auto __lsan_ignore_object(void const*) -> void;
    extern "C" auto __lsan_do_leak_check()            -> void;
    extern "C" auto __lsan_disable      ()            -> void;
    extern "C" auto __lsan_enable       ()            -> void;
    // NOLINTEND(bugprone-reserved-identifier)
}

namespace rawr::san::lsan
{
    inline constexpr bool compiled = RAWR_SAN_LSAN != 0;

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

// ── end-of-header scope: undo pp fragments and macros (FILO)
#include "rawr/detection/san.undef.pp"
