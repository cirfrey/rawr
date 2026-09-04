#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/intrin/mem.hpp"
#endif

#include "rawr/lib/dist/todo.pp"

#ifdef RAWR_MODULE
    export module rawr.lib.intrin.mem;
    import rawr.lib.intrin.base;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.bits;
    import rawr.lib.detection;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/bits.hpp"
    #include "rawr/lib/detection.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/compiler.pp"
#include "rawr/lib/attributes.pp"

RAWR_EXPORT namespace rawr::inline lib::intrin::inline mem::msvc
{
    RAWR_MSVC_INTRIN(1, memcpy,  (void*,       void const*, rst) -> void*);
    RAWR_MSVC_INTRIN(1, memset,  (void*,       int, rst)         -> void*);
    RAWR_MSVC_INTRIN(1, memmove, (void*,       void const*, rst) -> void*);
    RAWR_MSVC_INTRIN(1, memcmp,  (void const*, void const*, rst) -> int);
}

RAWR_EXPORT namespace rawr::inline lib::intrin::inline mem::gnu
{
                  RAWR_ALWAYS_INLINE constexpr auto memcpy (void* d,       void const* s, rst n) noexcept -> void* RAWR_GNU({ return ::__builtin_memcpy (d, s, n); });
                  RAWR_ALWAYS_INLINE           auto memset (void* d,       int v,         rst n) noexcept -> void* RAWR_GNU({ return ::__builtin_memset (d, v, n); });
                  RAWR_ALWAYS_INLINE constexpr auto memmove(void* d,       void const* s, rst n) noexcept -> void* RAWR_GNU({ return ::__builtin_memmove(d, s, n); });
    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto memcmp (void const* a, void const* b, rst n) noexcept -> int   RAWR_GNU({ return ::__builtin_memcmp (a, b, n); });
}

// Memory intrinsics.
RAWR_EXPORT namespace rawr::inline lib::intrin::inline mem
{
    namespace soft
    {
        template <typename Dst, typename Src>
        constexpr auto memcpy(Dst* dst, Src const* src, rst bytes) noexcept -> Dst*
        {
            // Fast path: if element sizes match and total bytes align, do direct element casting
            if constexpr (sizeof(Dst) == sizeof(Src)) {
                if (bytes % sizeof(Dst) == 0) {
                    rst const count = bytes / sizeof(Dst);
                    for (rst i = 0; i < count; ++i) {
                        dst[i] = bit_cast<Dst>(src[i]);
                    }
                    return dst;
                }
            }

            RAWR_TODO("Unimplemented. Implement")
            return dst;
        }

        constexpr auto memset(void* dst, int val, rst n) noexcept -> void*
        {
            auto* d = static_cast<unsigned char*>(dst);
            for (rst i = 0; i != n; ++i) { d[i] = static_cast<unsigned char>(val); }
            return dst;
        }

        constexpr auto memmove(void* dst, void const* src, rst n) noexcept -> void*
        {
            auto*       d = static_cast<unsigned char*>(dst);
            auto const* s = static_cast<unsigned char const*>(src);
            if (d < s || d >= s + n) { for (rst i = 0; i   != n; ++i) { d[i] = s[i]; } }
            else                     { for (rst i = n; i-- != 0;    ) { d[i] = s[i]; } }
            return dst;
        }

        [[nodiscard]] constexpr auto memcmp(void const* lhs, void const* rhs, rst n) noexcept -> int
        {
            auto const* l = static_cast<unsigned char const*>(lhs);
            auto const* r = static_cast<unsigned char const*>(rhs);
            for (rst i = 0; i != n; ++i) {
                if (l[i] < r[i]) { return -1; }
                if (l[i] > r[i]) { return  1; }
            }
            return 0;
        }
    }

    RAWR_ALWAYS_INLINE constexpr auto memcpy(auto* dst, auto const* src, rst n) noexcept -> void*
    {
        if (intrin::is_consteval())                      { return soft::memcpy(dst, src, n); }
             if constexpr(this_compiler.is_family_gnu()) { return  gnu::memcpy(dst, src, n); }
        else if constexpr(this_compiler.is_msvc())       { return msvc::memcpy(dst, src, n); }
        else                                             { return soft::memcpy(dst, src, n); }
    }

    RAWR_ALWAYS_INLINE constexpr auto memset(void* dst, int val, rst n) noexcept -> void*
    {
        if (intrin::is_consteval())                      { return soft::memset(dst, val, n); }
             if constexpr(this_compiler.is_family_gnu()) { return  gnu::memset(dst, val, n); }
        else if constexpr(this_compiler.is_msvc())       { return msvc::memset(dst, val, n); }
        else                                             { return soft::memset(dst, val, n); }
    }

    RAWR_ALWAYS_INLINE constexpr auto memmove(void* dst, void const* src, rst n) noexcept -> void*
    {
        if (intrin::is_consteval())                      { return soft::memmove(dst, src, n); }
             if constexpr(this_compiler.is_family_gnu()) { return  gnu::memmove(dst, src, n); }
        else if constexpr(this_compiler.is_msvc())       { return msvc::memmove(dst, src, n); }
        else                                             { return soft::memmove(dst, src, n); }
    }

    [[nodiscard]] RAWR_ALWAYS_INLINE constexpr auto memcmp(void const* lhs, void const* rhs, rst n) noexcept -> int
    {
        if (intrin::is_consteval())                      { return soft::memcmp(lhs, rhs, n); }
             if constexpr(this_compiler.is_family_gnu()) { return  gnu::memcmp(lhs, rhs, n); }
        else if constexpr(this_compiler.is_msvc())       { return msvc::memcmp(lhs, rhs, n); }
        else                                             { return soft::memcmp(lhs, rhs, n); }
    }
}
