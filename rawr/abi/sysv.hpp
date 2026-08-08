
//// rawr/abi/sysv.hpp
#pragma region rawr/abi/sysv.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/cxx/detection_macros.hpp"
    #include "rawr/cxx/intrin/macros.hpp"
    #include "rawr/data/rich_enum.hpp"
    #include "rawr/diag/dwarf.hpp"
#endif

namespace rawr::abi::sysv
{
    // SysV psABI auxiliary vector tag constants.
    // Values are ABI-fixed — explicit enumerator values are not optional here.
    // u16: AT_MINSIGSTKSZ = 51 is the current high watermark; u8 may be too narrow;
    // u16 absorbs future additions without a type change.
    RAWR_RICH_ENUM(at, u16, (
        (null,              0),  // end-of-vector sentinel; no value field is meaningful
        (ignore,            1),  // ignored entry; skip
        (execfd,            2),  // fd of executable (only present when phdr absent)
        (phdr,              3),  // ▸ pointer: ELF program header table in memory
        (phent,             4),  // sizeof(Elf_Phdr)
        (phnum,             5),  // count of program headers
        (pagesz,            6),  // system page size in bytes
        (base,              7),  // ▸ pointer: interpreter (ld.so) load base
        (flags,             8),  // reserved; always 0 on Linux
        (entry,             9),  // ▸ pointer: executable entry point
        (notelf,            10), // nonzero when binary is not ELF
        (uid,               11), // real user ID
        (euid,              12), // effective user ID
        (gid,               13), // real group ID
        (egid,              14), // effective group ID
        (platform,          15), // ▸ pointer: arch name string ("x86_64", "aarch64", …)
        (hwcap,             16), // hardware capability bitmask (arch-defined bits)
        (clktck,            17), // clock ticks/sec; denominator for times(2)
        (fpucw,             18), // i386 FPU control word; ignore on all other arches
        (dcachebsize,       19), // data cache block size; PowerPC/MIPS only
        (icachebsize,       20), // instruction cache block size; PowerPC/MIPS only
        (ucachebsize,       21), // unified cache block size; PowerPC/MIPS only
        (ignoreppc,         22), // PowerPC-specific; ignore
        (secure,            23), // 1 if executing with elevated privilege (setuid/cap)
        (base_platform,     24), // ▸ pointer: base platform string ("v8l" on 32-bit ARM)
        (random,            25), // ▸ pointer: 16 kernel-supplied random bytes
        (hwcap2,            26), // secondary hardware capability bitmask
        (rseq_feature_size, 27), // size of rseq features kernel supports (Linux 5.13+)
        (rseq_align,        28), // required rseq area alignment  (Linux 5.13+)
        (hwcap3,            29), // third hwcap bitmask; arch-specific, rarely present
        (hwcap4,            30), // fourth hwcap bitmask; arch-specific, rarely present
        (execfn,            31), // ▸ pointer: pathname of executed binary
        (sysinfo,           32), // ▸ pointer: vsyscall entry point (x86-32 only)
        (sysinfo_ehdr,      33), // ▸ pointer: vDSO ELF image base
        (minsigstksz,       51)  // minimum alternate signal stack size in bytes
    ), ());

    #define RAWR_ABI_SYSV_AUXVE_(Name, word_t)                                                                                \
        struct Name {                                                                                                     \
            word_t a_type;                                                                                                \
            word_t a_val;                                                                                                 \
                                                                                                                          \
            [[nodiscard]] constexpr at tag() const noexcept {                                                             \
                return at::from_underlying_unchecked(                                                                     \
                    static_cast<at::underlying_type>(a_type));                                                            \
            }                                                                                                             \
                                                                                                                          \
            /* Direct compare — loop terminator, must be fast even at O0. */                                              \
            [[nodiscard]] constexpr bool is_null() const noexcept { return a_type == static_cast<word_t>(0); }            \
                                                                                                                          \
            [[nodiscard]] constexpr bool is_ignore()            const noexcept { return tag() == at::ignore;            } \
            [[nodiscard]] constexpr bool is_execfd()            const noexcept { return tag() == at::execfd;            } \
            [[nodiscard]] constexpr bool is_phdr()              const noexcept { return tag() == at::phdr;              } \
            [[nodiscard]] constexpr bool is_phent()             const noexcept { return tag() == at::phent;             } \
            [[nodiscard]] constexpr bool is_phnum()             const noexcept { return tag() == at::phnum;             } \
            [[nodiscard]] constexpr bool is_pagesz()            const noexcept { return tag() == at::pagesz;            } \
            [[nodiscard]] constexpr bool is_base()              const noexcept { return tag() == at::base;              } \
            [[nodiscard]] constexpr bool is_flags()             const noexcept { return tag() == at::flags;             } \
            [[nodiscard]] constexpr bool is_entry()             const noexcept { return tag() == at::entry;             } \
            [[nodiscard]] constexpr bool is_notelf()            const noexcept { return tag() == at::notelf;            } \
            [[nodiscard]] constexpr bool is_uid()               const noexcept { return tag() == at::uid;               } \
            [[nodiscard]] constexpr bool is_euid()              const noexcept { return tag() == at::euid;              } \
            [[nodiscard]] constexpr bool is_gid()               const noexcept { return tag() == at::gid;               } \
            [[nodiscard]] constexpr bool is_egid()              const noexcept { return tag() == at::egid;              } \
            [[nodiscard]] constexpr bool is_platform()          const noexcept { return tag() == at::platform;          } \
            [[nodiscard]] constexpr bool is_hwcap()             const noexcept { return tag() == at::hwcap;             } \
            [[nodiscard]] constexpr bool is_clktck()            const noexcept { return tag() == at::clktck;            } \
            [[nodiscard]] constexpr bool is_fpucw()             const noexcept { return tag() == at::fpucw;             } \
            [[nodiscard]] constexpr bool is_dcachebsize()       const noexcept { return tag() == at::dcachebsize;       } \
            [[nodiscard]] constexpr bool is_icachebsize()       const noexcept { return tag() == at::icachebsize;       } \
            [[nodiscard]] constexpr bool is_ucachebsize()       const noexcept { return tag() == at::ucachebsize;       } \
            [[nodiscard]] constexpr bool is_ignoreppc()         const noexcept { return tag() == at::ignoreppc;         } \
            [[nodiscard]] constexpr bool is_secure()            const noexcept { return tag() == at::secure;            } \
            [[nodiscard]] constexpr bool is_base_platform()     const noexcept { return tag() == at::base_platform;     } \
            [[nodiscard]] constexpr bool is_random()            const noexcept { return tag() == at::random;            } \
            [[nodiscard]] constexpr bool is_hwcap2()            const noexcept { return tag() == at::hwcap2;            } \
            [[nodiscard]] constexpr bool is_rseq_feature_size() const noexcept { return tag() == at::rseq_feature_size; } \
            [[nodiscard]] constexpr bool is_rseq_align()        const noexcept { return tag() == at::rseq_align;        } \
            [[nodiscard]] constexpr bool is_hwcap3()            const noexcept { return tag() == at::hwcap3;            } \
            [[nodiscard]] constexpr bool is_hwcap4()            const noexcept { return tag() == at::hwcap4;            } \
            [[nodiscard]] constexpr bool is_execfn()            const noexcept { return tag() == at::execfn;            } \
            [[nodiscard]] constexpr bool is_sysinfo()           const noexcept { return tag() == at::sysinfo;           } \
            [[nodiscard]] constexpr bool is_sysinfo_ehdr()      const noexcept { return tag() == at::sysinfo_ehdr;      } \
            [[nodiscard]] constexpr bool is_minsigstksz()       const noexcept { return tag() == at::minsigstksz;       } \
                                                                                                                          \
            [[nodiscard]] constexpr word_t pagesz()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t phent()             const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t phnum()             const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t flags()             const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t uid()               const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t euid()              const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t gid()               const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t egid()              const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t hwcap()             const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t hwcap2()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t hwcap3()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t hwcap4()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t clktck()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t minsigstksz()       const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t rseq_feature_size() const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t rseq_align()        const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t secure()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t notelf()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t execfd()            const noexcept { return a_val; }                           \
            /* FUTURE ptr<>: these become ptr<T, word_policy<word_t>>::from_integer(a_val) */                             \
            [[nodiscard]] constexpr word_t phdr()              const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t base()              const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t entry()             const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t platform()          const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t base_platform()     const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t random()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t execfn()            const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t sysinfo()           const noexcept { return a_val; }                           \
            [[nodiscard]] constexpr word_t sysinfo_ehdr()      const noexcept { return a_val; }                           \
        }
    RAWR_ABI_SYSV_AUXVE_(auxve32, u32);
    RAWR_ABI_SYSV_AUXVE_(auxve64, u64);
    #undef RAWR_ABI_SYSV_AUXVE_

    // ── auxv ─────────────────────────────────────────────────────────────────
    // Thin wrapper enabling range-for over an auxv vector.
    // Sentinel-based end(): no need to pre-scan for null terminator.
    //
    // Usage:
    //   for (auto& at : ctx.auxv) { switch (at.tag()) { ... } }
    //   for (auto* at = ctx.auxv.first; !at->is_null(); ++at) { ... } // manual
    #define RAWR_ABI_SYSV_AUXV_(Name, Entry)                                                                               \
        struct Name {                                                                                             \
            Entry* first = nullptr;                                                                               \
                                                                                                                  \
            struct sentinel {};                                                                                   \
            struct iterator {                                                                                     \
                Entry* ptr;                                                                                       \
                [[nodiscard]] constexpr Entry&    operator*()          const noexcept { return *ptr;            } \
                [[nodiscard]] constexpr Entry*    operator->()         const noexcept { return ptr;             } \
                              constexpr iterator& operator++()               noexcept { ++ptr; return *this;    } \
                [[nodiscard]] constexpr bool      operator!=(sentinel) const noexcept { return !ptr->is_null(); } \
            };                                                                                                    \
                                                                                                                  \
            [[nodiscard]] constexpr iterator begin() const noexcept { return { first }; }                         \
            [[nodiscard]] constexpr sentinel end()   const noexcept { return {};        }                         \
        }
    RAWR_ABI_SYSV_AUXV_(auxv32, auxve32);
    RAWR_ABI_SYSV_AUXV_(auxv64, auxve64);
    #undef RAWR_ABI_SYSV_AUXV_

    // TODO: context32 and selecting the correct one in the MAIN macro.
    struct context64 {
        void* sp    = nullptr;
        s32  argc    = 0;
        char** argv = nullptr;
        char** envp = nullptr;
        auxv64 auxv  = {};

        // Not constexpr: reinterpret_cast on the auxv pointer is unavoidable here.
        // The entry accessors and span iteration are constexpr; construction from a
        // live stack pointer is inherently runtime.
        static RAWR_ALWAYS_INLINE auto from_stack_pointer(void* sp) noexcept -> context64
        {
            s32    argc = *static_cast<s32*>(sp);
            char** argv = static_cast<char**>(sp) + 1;
            char** envp = argv + argc + 1;

            char** cur = envp;
            while (*cur) { ++cur; }

            return { sp, argc, argv, envp, { reinterpret_cast<auxve64*>(cur + 1) } };
        }
    };

    struct ctx_trampoline
    {
        // Defaulted .error guards against misuse, even if you don't check .is_set.
        char const* data_  = ".error \"Missing trampoline\"";
        u64         size_  = 27;
        bool        is_set = false;

        constexpr auto data() const { return data_; }
        constexpr auto size() const { return size_; }

        template <u64 Size>
        constexpr ctx_trampoline(char const (&str)[Size]) : data_{str}, size_{Size}, is_set{true} {}
        constexpr ctx_trampoline() = default;
    };
    struct ctx_trampolines_t
    {
        ctx_trampoline arr[archs::enum_count];
        constexpr auto operator[](archs a)       -> ctx_trampoline&       { return arr[a.to_underlying()]; }
        constexpr auto operator[](archs a) const -> ctx_trampoline const& { return arr[a.to_underlying()]; }
    };
    constexpr auto ctx_trampolines = []() constexpr {
        ctx_trampolines_t ret;

        // Most of these get a Tail‑call (jmp/b/j) – preserves kernel‑provided
        // 16‑byte alignment, no stack push, no alignment fixup needed.
        ret[rawr::archs::x64]     = "mov %rsp, %rdi\n\tjmp rawr_main";
        ret[rawr::archs::arm32]   = "mov r0, sp\n\tb rawr_main";
        ret[rawr::archs::arm64]   = "mov x0, sp\n\tb rawr_main";
        ret[rawr::archs::riscv32] = "mv a0, sp\n\tj rawr_main";
        ret[rawr::archs::riscv64] = "mv a0, sp\n\tj rawr_main";
        // Except x86-32: esp points to argc at entry. Pass original esp as void* sp argument.
        // Push before align — otherwise the push itself breaks the alignment we just set.
        // "and $-16, %esp" first would lose the original esp value.
        ret[rawr::archs::x86] =
            "movl %esp, %eax\n\t"    // save original sp
            "andl $-16, %esp\n\t"    // align (kernel may deliver esp-4 ≡ 0 mod 16; verify per ABI)
            "pushl %eax\n\t"         // push sp as argument
            "call rawr_main";        // must be call, not jmp — arg is on stack

        return ret;
    }();
}

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
                ::rawr::diag::dwarf::mark_unwind_root();                         \
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
                ::rawr::diag::dwarf::mark_unwind_root();                         \
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
            ::rawr::diag::dwarf::mark_unwind_root();                      \
            __VA_ARGS__;                                                  \
        }                                                                 \
        RAWR_NORETURN void _start() noexcept RAWR_ASM_ALIAS("rawr_main"); \
    }

#pragma endregion rawr/abi/sysv.hpp
