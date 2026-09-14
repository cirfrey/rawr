#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/abi/sysv/main.pp"
#endif
#pragma once

#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.abi.sysv.ctx;
    import rawr.lib.diag.dwarf;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/abi/sysv/ctx.hpp"
    #include "rawr/lib/diag/dwarf.hpp"
#endif
#include "rawr/abi/sysv/trampolines.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

#if RAWR_COMPILER_GCC
    // GCC injects a ud2 in _start, the RAWR_UNREACHABLE supresses it.
    // If you want a ud2 you should do it yourself, the trampoline is guaranteed
    // to not hit it.
    #define RAWR_ABI_SYSV_MAIN(...)                                                                            \
        static_assert(RAWR_ABI_SYSV_HAS_TRAMPOLINE, "SysV ctx trampoline not defined for this architecture" ); \
                                                                                                               \
        [[gnu::flatten]] RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main(__VA_ARGS__) noexcept -> void;   \
        extern "C" {                                                                                           \
            [[gnu::naked]] RAWR_NORETURN auto _start() noexcept -> void                                        \
            {                                                                                                  \
                __asm__(RAWR_ABI_SYSV_TRAMPOLINE);                                                             \
                RAWR_UNREACHABLE;                                                                              \
            }                                                                                                  \
                                                                                                               \
            [[gnu::flatten]] RAWR_NORETURN auto rawr_main(void* sp) noexcept -> void                           \
            {                                                                                                  \
                ::rawr::lib::diag::dwarf::mark_unwind_root();                                                  \
                auto ctx = ::rawr::abi::sysv::context64::from_stack_pointer(sp);                               \
                rawr_user_main(ctx);                                                                           \
            }                                                                                                  \
        }                                                                                                      \
        [[gnu::flatten]] RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main(__VA_ARGS__) noexcept -> void

#elif RAWR_COMPILER_CLANG
    // Clang complains about non-asm in naked functions. It also doesn't
    // inject a ud2 in _start, so we don't need to suppress it.
    #define RAWR_ABI_SYSV_MAIN(...)                                                                            \
        static_assert(RAWR_ABI_SYSV_HAS_TRAMPOLINE, "SysV ctx trampoline not defined for this architecture" ); \
                                                                                                               \
        [[gnu::flatten]] RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main(__VA_ARGS__) noexcept -> void;   \
        extern "C" {                                                                                           \
            [[gnu::naked]] RAWR_NORETURN auto _start() noexcept -> void                                        \
            { __asm__(RAWR_ABI_SYSV_TRAMPOLINE); }                                                             \
                                                                                                               \
            [[gnu::flatten]] RAWR_NORETURN auto rawr_main(void* sp) noexcept -> void                           \
            {                                                                                                  \
                ::rawr::lib::diag::dwarf::mark_unwind_root();                                                  \
                auto ctx = ::rawr::abi::sysv::context64::from_stack_pointer(sp);                               \
                rawr_user_main(ctx);                                                                           \
            }                                                                                                  \
        }                                                                                                      \
        [[gnu::flatten]] RAWR_NORETURN auto rawr_user_main(__VA_ARGS__) noexcept -> void

#endif

#define RAWR_ABI_SYSV_MAIN_NOCTX                                             \
    RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main() noexcept -> void; \
    extern "C" {                                                             \
        RAWR_NORETURN auto rawr_main() noexcept -> void                      \
        {                                                                    \
            ::rawr::lib::diag::dwarf::mark_unwind_root();                    \
            rawr_user_main();                                                \
        }                                                                    \
        RAWR_ATTRIBUTE(alias("rawr_main"))                                   \
        RAWR_NORETURN auto _start() noexcept -> void;                        \
    }                                                                        \
    RAWR_NORETURN RAWR_ALWAYS_INLINE auto rawr_user_main() noexcept -> void
