//// rawr/san/msan.hpp.

#ifdef RAWR_MODULE
    export module rawr.san.msan;
    import rawr.lib.integer.raw;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/raw.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::san::msan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" void __msan_poison(void const volatile*, rst);
    extern "C" void __msan_unpoison(void const volatile*, rst);
    extern "C" void __msan_check_mem_is_initialized(void const volatile*, rst);
    // NOLINTEND(bugprone-reserved-identifier)
}

RAWR_EXPORT namespace rawr::san::msan
{
    inline constexpr bool compiled = RAWR_SAN_MSAN != 0;

    inline auto poison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    { if constexpr(compiled) { detail::__msan_poison(address, size); } }

    inline auto unpoison(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    { if constexpr(compiled) { detail::__msan_unpoison(address, size); } }

    inline auto check_initialized(
        [[maybe_unused]] void const* address,
        [[maybe_unused]] rst size
    ) noexcept -> void
    { if constexpr(compiled) { detail::__msan_check_mem_is_initialized(address, size); } }
}
