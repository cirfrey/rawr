
//// rawr/abi/sysv.hpp
#pragma once

#include "rawr/lib/module.pp"
#if RAWR_MODULE
    import rawr.lib.detection;
    import rawr.lib.integer.raw;
#else
    #include "rawr/lib/detection.hpp"
    #include "rawr/lib/integer/raw.hpp"
#endif
#include "rawr/lib/attributes.pp"
#include "rawr/lib/rich_enum.pp"

namespace rawr::abi::sysv
{
    // SysV psABI auxiliary vector tag constants.
    // Values are ABI-fixed — explicit enumerator values are not optional here.
    RAWR_RICH_ENUM(at, ru8, (
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

    #define RAWR_ABI_SYSV_AUXVE_(Name, word_t)                                                                                    \
        struct Name {                                                                                                             \
            word_t a_type;                                                                                                        \
            word_t a_val;                                                                                                         \
                                                                                                                                  \
            [[nodiscard]] constexpr auto tag() const noexcept -> at {                                                             \
                return at::from_underlying_unchecked(                                                                             \
                    static_cast<at::underlying_type>(a_type));                                                                    \
            }                                                                                                                     \
                                                                                                                                  \
            [[nodiscard]] constexpr auto is_null()              const noexcept -> bool { return tag() == at::null;              } \
            [[nodiscard]] constexpr auto is_ignore()            const noexcept -> bool { return tag() == at::ignore;            } \
            [[nodiscard]] constexpr auto is_execfd()            const noexcept -> bool { return tag() == at::execfd;            } \
            [[nodiscard]] constexpr auto is_phdr()              const noexcept -> bool { return tag() == at::phdr;              } \
            [[nodiscard]] constexpr auto is_phent()             const noexcept -> bool { return tag() == at::phent;             } \
            [[nodiscard]] constexpr auto is_phnum()             const noexcept -> bool { return tag() == at::phnum;             } \
            [[nodiscard]] constexpr auto is_pagesz()            const noexcept -> bool { return tag() == at::pagesz;            } \
            [[nodiscard]] constexpr auto is_base()              const noexcept -> bool { return tag() == at::base;              } \
            [[nodiscard]] constexpr auto is_flags()             const noexcept -> bool { return tag() == at::flags;             } \
            [[nodiscard]] constexpr auto is_entry()             const noexcept -> bool { return tag() == at::entry;             } \
            [[nodiscard]] constexpr auto is_notelf()            const noexcept -> bool { return tag() == at::notelf;            } \
            [[nodiscard]] constexpr auto is_uid()               const noexcept -> bool { return tag() == at::uid;               } \
            [[nodiscard]] constexpr auto is_euid()              const noexcept -> bool { return tag() == at::euid;              } \
            [[nodiscard]] constexpr auto is_gid()               const noexcept -> bool { return tag() == at::gid;               } \
            [[nodiscard]] constexpr auto is_egid()              const noexcept -> bool { return tag() == at::egid;              } \
            [[nodiscard]] constexpr auto is_platform()          const noexcept -> bool { return tag() == at::platform;          } \
            [[nodiscard]] constexpr auto is_hwcap()             const noexcept -> bool { return tag() == at::hwcap;             } \
            [[nodiscard]] constexpr auto is_clktck()            const noexcept -> bool { return tag() == at::clktck;            } \
            [[nodiscard]] constexpr auto is_fpucw()             const noexcept -> bool { return tag() == at::fpucw;             } \
            [[nodiscard]] constexpr auto is_dcachebsize()       const noexcept -> bool { return tag() == at::dcachebsize;       } \
            [[nodiscard]] constexpr auto is_icachebsize()       const noexcept -> bool { return tag() == at::icachebsize;       } \
            [[nodiscard]] constexpr auto is_ucachebsize()       const noexcept -> bool { return tag() == at::ucachebsize;       } \
            [[nodiscard]] constexpr auto is_ignoreppc()         const noexcept -> bool { return tag() == at::ignoreppc;         } \
            [[nodiscard]] constexpr auto is_secure()            const noexcept -> bool { return tag() == at::secure;            } \
            [[nodiscard]] constexpr auto is_base_platform()     const noexcept -> bool { return tag() == at::base_platform;     } \
            [[nodiscard]] constexpr auto is_random()            const noexcept -> bool { return tag() == at::random;            } \
            [[nodiscard]] constexpr auto is_hwcap2()            const noexcept -> bool { return tag() == at::hwcap2;            } \
            [[nodiscard]] constexpr auto is_rseq_feature_size() const noexcept -> bool { return tag() == at::rseq_feature_size; } \
            [[nodiscard]] constexpr auto is_rseq_align()        const noexcept -> bool { return tag() == at::rseq_align;        } \
            [[nodiscard]] constexpr auto is_hwcap3()            const noexcept -> bool { return tag() == at::hwcap3;            } \
            [[nodiscard]] constexpr auto is_hwcap4()            const noexcept -> bool { return tag() == at::hwcap4;            } \
            [[nodiscard]] constexpr auto is_execfn()            const noexcept -> bool { return tag() == at::execfn;            } \
            [[nodiscard]] constexpr auto is_sysinfo()           const noexcept -> bool { return tag() == at::sysinfo;           } \
            [[nodiscard]] constexpr auto is_sysinfo_ehdr()      const noexcept -> bool { return tag() == at::sysinfo_ehdr;      } \
            [[nodiscard]] constexpr auto is_minsigstksz()       const noexcept -> bool { return tag() == at::minsigstksz;       } \
                                                                                                                                  \
            [[nodiscard]] constexpr auto pagesz()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto phent()                const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto phnum()                const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto flags()                const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto uid()                  const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto euid()                 const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto gid()                  const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto egid()                 const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto hwcap()                const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto hwcap2()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto hwcap3()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto hwcap4()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto clktck()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto minsigstksz()          const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto rseq_feature_size()    const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto rseq_align()           const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto secure()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto notelf()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto execfd()               const noexcept -> word_t { return a_val; }                        \
            /* FUTURE ptr<>: these become ptr<T, word_policy<word_t>>::from_integer(a_val) */                                     \
            [[nodiscard]] constexpr auto phdr()                 const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto base()                 const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto entry()                const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto platform()             const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto base_platform()        const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto random()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto execfn()               const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto sysinfo()              const noexcept -> word_t { return a_val; }                        \
            [[nodiscard]] constexpr auto sysinfo_ehdr()         const noexcept -> word_t { return a_val; }                        \
        }
    RAWR_ABI_SYSV_AUXVE_(auxve32, ru32);
    RAWR_ABI_SYSV_AUXVE_(auxve64, ru64);
    #undef RAWR_ABI_SYSV_AUXVE_

    // ── auxv ─────────────────────────────────────────────────────────────────
    // Thin wrapper enabling range-for over an auxv vector.
    // Sentinel-based end(): no need to pre-scan for null terminator.
    //
    // Usage:
    //   for (auto& at : ctx.auxv) { switch (at.tag()) { ... } }
    //   for (auto* at = ctx.auxv.first; !at->is_null(); ++at) { ... } // manual
    #define RAWR_ABI_SYSV_AUXV_(Name, Entry)                                                                              \
        struct Name {                                                                                                     \
            Entry* first = nullptr;                                                                                       \
                                                                                                                          \
            struct sentinel {};                                                                                           \
            struct iterator {                                                                                             \
                Entry* ptr;                                                                                               \
                [[nodiscard]] constexpr auto operator*()          const noexcept -> Entry&    { return *ptr;            } \
                [[nodiscard]] constexpr auto operator->()         const noexcept -> Entry*    { return ptr;             } \
                              constexpr auto operator++()               noexcept -> iterator& { ++ptr; return *this;    } \
                [[nodiscard]] constexpr auto operator!=(sentinel) const noexcept -> bool      { return !ptr->is_null(); } \
            };                                                                                                            \
                                                                                                                          \
            [[nodiscard]] constexpr auto begin() const noexcept -> iterator { return { first }; }                         \
            [[nodiscard]] constexpr auto end()   const noexcept -> sentinel { return {};        }                         \
        }
    RAWR_ABI_SYSV_AUXV_(auxv32, auxve32);
    RAWR_ABI_SYSV_AUXV_(auxv64, auxve64);
    #undef RAWR_ABI_SYSV_AUXV_

    // TODO: context32 and selecting the correct one in the MAIN macro.
    struct context64 {
        void* sp    = nullptr;
        rs32  argc  = 0;
        char** argv = nullptr;
        char** envp = nullptr;
        auxv64 auxv  = {};

        // Not constexpr: reinterpret_cast on the auxv pointer is unavoidable here.
        // The entry accessors and span iteration are constexpr; construction from a
        // live stack pointer is inherently runtime.
        static RAWR_ALWAYS_INLINE auto from_stack_pointer(void* stack_pointer) noexcept -> context64
        {
            rs32   argc = *static_cast<rs32*>(stack_pointer);
            char** argv = static_cast<char**>(stack_pointer) + 1;
            char** envp = argv + argc + 1;

            char** cur = envp;
            while (*cur != nullptr) { ++cur; }

            return {
                .sp = stack_pointer,
                .argc = argc,
                .argv = argv,
                .envp = envp,
                .auxv = { reinterpret_cast<auxve64*>(cur + 1) }
            };
        }
    };

    struct ctx_trampoline
    {
        // Defaulted .error guards against misuse, even if you don't check .is_set.
        static constexpr char missing_error[] = ".error \"Missing trampoline\"";
        char const* data_  = missing_error;
        ru64        size_  = sizeof(missing_error);
        bool        is_set = false;

        [[nodiscard]] constexpr auto data() const { return data_; }
        [[nodiscard]] constexpr auto size() const { return size_; }

        template <ru64 Size>
        constexpr ctx_trampoline(char const (&str)[Size]) : data_{str}, size_{Size}, is_set{true} {}
        constexpr ctx_trampoline() = default;
    };
    struct ctx_trampolines_t
    {
        ctx_trampoline arr[archs::enum_count];
        constexpr auto operator[](archs arch)       -> ctx_trampoline&       { return arr[arch.to_underlying()]; }
        constexpr auto operator[](archs arch) const -> ctx_trampoline const& { return arr[arch.to_underlying()]; }
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
