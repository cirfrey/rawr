//// rawr/san/tsan.hpp.

#ifdef RAWR_MODULE
    export module rawr.san.tsan;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::san::tsan::detail
{
    // NOLINTBEGIN(bugprone-reserved-identifier)
    extern "C" void __tsan_acquire(void*);
    extern "C" void __tsan_release(void*);
    extern "C" void __tsan_ignore_reads_begin();
    extern "C" void __tsan_ignore_reads_end();
    extern "C" void __tsan_ignore_writes_begin();
    extern "C" void __tsan_ignore_writes_end();
    // NOLINTEND(bugprone-reserved-identifier)
}

RAWR_EXPORT namespace rawr::san::tsan
{
    inline constexpr bool compiled = RAWR_SAN_TSAN != 0;

    inline auto acquire([[maybe_unused]] void* address) noexcept -> void
    { if constexpr(compiled) { detail::__tsan_acquire(address); } }

    inline auto release([[maybe_unused]] void* address) noexcept -> void
    { if constexpr(compiled) { detail::__tsan_release(address); } }

    inline auto ignore_reads_begin() noexcept -> void
    { if constexpr(compiled) { detail::__tsan_ignore_reads_begin(); } }

    inline auto ignore_reads_end() noexcept -> void
    { if constexpr(compiled) { detail::__tsan_ignore_reads_end(); } }

    inline auto ignore_writes_begin() noexcept -> void
    { if constexpr(compiled) { detail::__tsan_ignore_writes_begin(); } }

    inline auto ignore_writes_end() noexcept -> void
    { if constexpr(compiled) { detail::__tsan_ignore_writes_end(); } }
}
