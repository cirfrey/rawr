
//// rawr/abi/sysv.pp.
#pragma once

#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.abi.sysv;
    import rawr.lib.detection;
    import rawr.lib.diag.dwarf;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/abi/sysv.hpp"
    #include "rawr/lib/detection.hpp"
    #include "rawr/lib/diag/dwarf.hpp"
#endif

#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

#if RAWR_COMPILER_GCC
    // GCC injects a ud2 in _start, the RAWR_UNREACHABLE supresses it.
    // If you want a ud2 you should do it yourself, the trampoline is guaranteed
    // to not hit it.
    #define RAWR_ABI_SYSV_MAIN(...)                                              \
        static_assert(                                                           \
            ::rawr::abi::sysv::ctx_trampolines[::rawr::this_arch].is_set,        \
            "SysV trampoline not defined for this architecture"                  \
        );                                                                       \
        extern "C" {                                                             \
            [[gnu::naked]] RAWR_NORETURN void _start() noexcept {                \
                asm((::rawr::abi::sysv::ctx_trampolines[::rawr::this_arch]));    \
                RAWR_UNREACHABLE;                                                \
            }                                                                    \
            [[gnu::flatten]] RAWR_NORETURN void rawr_main(void* sp) noexcept {   \
                ::rawr::lib::diag::dwarf::mark_unwind_root();                    \
                auto ctx = ::rawr::abi::sysv::context64::from_stack_pointer(sp); \
                [](auto& ctx) RAWR_NORETURN { __VA_ARGS__; }(ctx);               \
            }                                                                    \
        }
#elif RAWR_COMPILER_CLANG
    // Clang complains about non-asm in naked functions. It also doesn't
    // inject a ud2 in _start, so we don't need to suppress it.
    #define RAWR_ABI_SYSV_MAIN(...)                                              \
        static_assert(                                                           \
            ::rawr::abi::sysv::ctx_trampolines[::rawr::this_arch].is_set,        \
            "SysV ctx trampoline not defined for this architecture"              \
        );                                                                       \
        extern "C" {                                                             \
            [[gnu::naked]] RAWR_NORETURN void _start() noexcept {                \
                asm((::rawr::abi::sysv::ctx_trampolines[::rawr::this_arch]));    \
            }                                                                    \
            [[gnu::flatten]] RAWR_NORETURN void rawr_main(void* sp) noexcept {   \
                ::rawr::lib::diag::dwarf::mark_unwind_root();                    \
                auto ctx = ::rawr::abi::sysv::context64::from_stack_pointer(sp); \
                [](auto& ctx) RAWR_NORETURN { __VA_ARGS__; }(ctx);               \
            }                                                                    \
        }
#endif

// _start is aliased to rawr_main, this makes it so conceptually rawr_main means "user code"
// and _start means "abi trampoline stuff". In NOCTX mode you'll only see rawr_main in the
// generated assembly which is cleaner and more sematically consistent. The linker figures
// everything out correctly.
#define RAWR_ABI_SYSV_MAIN_NOCTX(...)                                     \
    extern "C" {                                                          \
        RAWR_NORETURN void rawr_main() noexcept {                         \
            ::rawr::lib::diag::dwarf::mark_unwind_root();                 \
            __VA_ARGS__;                                                  \
        }                                                                 \
        RAWR_NORETURN void _start() noexcept RAWR_ASM_ALIAS("rawr_main"); \
    }
