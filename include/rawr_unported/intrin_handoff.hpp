// rawr — scratch implementation
// Detection → L1 backends → L2 dispatch → SIMD types
//
// Each #pragma region maps to one real header in the final layout.
// Compiler headers (<immintrin.h>, <arm_neon.h>) are included here
// because they define the register types and intrinsic signatures.
// They are compiler-provided, not libc — compatible with freestanding.

// ═══════════════════════════════════════════════════════════════════
#pragma region rawr/lib/detection.hpp
// ═══════════════════════════════════════════════════════════════════

// ── compiler ─────────────────────────────────────────────────────────
#if defined(__clang__)
#  define RAWR_COMPILER_CLANG   1
#  define RAWR_COMPILER_GCC     0
#  define RAWR_COMPILER_MSVC    0
#  define RAWR_COMPILER_VERSION_MAJOR __clang_major__
#  define RAWR_COMPILER_VERSION_MINOR __clang_minor__
#elif defined(__GNUC__)
#  define RAWR_COMPILER_CLANG   0
#  define RAWR_COMPILER_GCC     1
#  define RAWR_COMPILER_MSVC    0
#  define RAWR_COMPILER_VERSION_MAJOR __GNUC__
#  define RAWR_COMPILER_VERSION_MINOR __GNUC_MINOR__
#elif defined(_MSC_VER)
#  define RAWR_COMPILER_CLANG   0
#  define RAWR_COMPILER_GCC     0
#  define RAWR_COMPILER_MSVC    1
#  define RAWR_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#  define RAWR_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#else
#  define RAWR_COMPILER_CLANG   0
#  define RAWR_COMPILER_GCC     0
#  define RAWR_COMPILER_MSVC    0
#  define RAWR_COMPILER_VERSION_MAJOR 0
#  define RAWR_COMPILER_VERSION_MINOR 0
#endif
#define RAWR_COMPILER_GNU (RAWR_COMPILER_GCC || RAWR_COMPILER_CLANG)

// ── arch ─────────────────────────────────────────────────────────────
#if defined(__x86_64__) || defined(_M_X64)
#  define RAWR_ARCH_X64    1
#  define RAWR_ARCH_X86    0
#  define RAWR_ARCH_ARM64  0
#  define RAWR_ARCH_ARM32  0
#elif defined(__i386__) || defined(_M_IX86)
#  define RAWR_ARCH_X64    0
#  define RAWR_ARCH_X86    1
#  define RAWR_ARCH_ARM64  0
#  define RAWR_ARCH_ARM32  0
#elif defined(__aarch64__) || defined(_M_ARM64)
#  define RAWR_ARCH_X64    0
#  define RAWR_ARCH_X86    0
#  define RAWR_ARCH_ARM64  1
#  define RAWR_ARCH_ARM32  0
#elif defined(__arm__) || defined(_M_ARM)
#  define RAWR_ARCH_X64    0
#  define RAWR_ARCH_X86    0
#  define RAWR_ARCH_ARM64  0
#  define RAWR_ARCH_ARM32  1
#else
#  define RAWR_ARCH_X64    0
#  define RAWR_ARCH_X86    0
#  define RAWR_ARCH_ARM64  0
#  define RAWR_ARCH_ARM32  0
#endif
#define RAWR_ARCH_X86_FAMILY (RAWR_ARCH_X64 || RAWR_ARCH_X86)
#define RAWR_ARCH_ARM_FAMILY (RAWR_ARCH_ARM64 || RAWR_ARCH_ARM32)

// ── platform ──────────────────────────────────────────────────────────
#if defined(__linux__)
#  define RAWR_PLATFORM_LINUX   1
#  define RAWR_PLATFORM_WINDOWS 0
#  define RAWR_PLATFORM_MACOS   0
#elif defined(_WIN32) || defined(_WIN64)
#  define RAWR_PLATFORM_LINUX   0
#  define RAWR_PLATFORM_WINDOWS 1
#  define RAWR_PLATFORM_MACOS   0
#elif defined(__APPLE__)
#  define RAWR_PLATFORM_LINUX   0
#  define RAWR_PLATFORM_WINDOWS 0
#  define RAWR_PLATFORM_MACOS   1
#else
#  define RAWR_PLATFORM_LINUX   0
#  define RAWR_PLATFORM_WINDOWS 0
#  define RAWR_PLATFORM_MACOS   0
#endif

// ── ABI ───────────────────────────────────────────────────────────────
#if RAWR_ARCH_X64 && !RAWR_PLATFORM_WINDOWS
#  define RAWR_ABI_SYSV    1
#  define RAWR_ABI_WIN64   0
#  define RAWR_ABI_AAPCS64 0
#elif RAWR_ARCH_X64 && RAWR_PLATFORM_WINDOWS
#  define RAWR_ABI_SYSV    0
#  define RAWR_ABI_WIN64   1
#  define RAWR_ABI_AAPCS64 0
#elif RAWR_ARCH_ARM64
#  define RAWR_ABI_SYSV    0
#  define RAWR_ABI_WIN64   0
#  define RAWR_ABI_AAPCS64 1
#else
#  define RAWR_ABI_SYSV    0
#  define RAWR_ABI_WIN64   0
#  define RAWR_ABI_AAPCS64 0
#endif

// ── binary format ─────────────────────────────────────────────────────
#if RAWR_PLATFORM_LINUX || RAWR_PLATFORM_MACOS
#  define RAWR_BIN_ELF    RAWR_PLATFORM_LINUX
#  define RAWR_BIN_MACHO  RAWR_PLATFORM_MACOS
#  define RAWR_BIN_PE     0
#elif RAWR_PLATFORM_WINDOWS
#  define RAWR_BIN_ELF    0
#  define RAWR_BIN_MACHO  0
#  define RAWR_BIN_PE     1
#else
#  define RAWR_BIN_ELF    0
#  define RAWR_BIN_MACHO  0
#  define RAWR_BIN_PE     0
#endif

// ── x86 ISA features — always defined 0 or 1 ─────────────────────────
// SSE2 is mandatory on x64 by ABI contract.
#define RAWR_ARCH_X86_FEATURE_SSE    (RAWR_ARCH_X64 || defined(__SSE__))
#define RAWR_ARCH_X86_FEATURE_SSE2   (RAWR_ARCH_X64 || defined(__SSE2__))
#define RAWR_ARCH_X86_FEATURE_SSE41  (defined(__SSE4_1__))
#define RAWR_ARCH_X86_FEATURE_SSE42  (defined(__SSE4_2__))
#define RAWR_ARCH_X86_FEATURE_AVX    (defined(__AVX__))
#define RAWR_ARCH_X86_FEATURE_AVX2   (defined(__AVX2__))
#define RAWR_ARCH_X86_FEATURE_AVX512F (defined(__AVX512F__))
#define RAWR_ARCH_X86_FEATURE_FMA    (defined(__FMA__))
#define RAWR_ARCH_X86_FEATURE_BMI1   (defined(__BMI__))
#define RAWR_ARCH_X86_FEATURE_BMI2   (defined(__BMI2__))
#define RAWR_ARCH_X86_FEATURE_POPCNT (defined(__POPCNT__))
#define RAWR_ARCH_X86_FEATURE_LZCNT  (defined(__LZCNT__))
#define RAWR_ARCH_X86_FEATURE_CLWB   (defined(__CLWB__))
// MSVC exposes few independent ISA flags — only AVX/AVX2/AVX512 via /arch:.
// CLWB, FMA, BMI etc. must be injected via -DRAWR_ARCH_X86_FEATURE_CLWB=1
// in the build system when targeting those features on MSVC.

// ── ARM ISA features ──────────────────────────────────────────────────
#define RAWR_ARCH_ARM_FEATURE_NEON    (RAWR_ARCH_ARM_FAMILY && defined(__ARM_NEON))
#define RAWR_ARCH_ARM_FEATURE_SVE     (defined(__ARM_FEATURE_SVE))
#define RAWR_ARCH_ARM_FEATURE_SVE2    (defined(__ARM_FEATURE_SVE2))
#define RAWR_ARCH_ARM_FEATURE_DOTPROD (defined(__ARM_FEATURE_DOTPROD))
#define RAWR_ARCH_ARM_FEATURE_FP16    (defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC))
#define RAWR_ARCH_ARM_FEATURE_BF16    (defined(__ARM_FEATURE_BF16))

// ── build flags ───────────────────────────────────────────────────────
#if (defined(__GNUC__) && !defined(__OPTIMIZE__)) || (defined(_MSC_VER) && !defined(NDEBUG))
#  define RAWR_BUILD_OPTIMIZED 0
#else
#  define RAWR_BUILD_OPTIMIZED 1
#endif
#if defined(__SANITIZE_ADDRESS__) || (defined(__has_feature) && __has_feature(address_sanitizer))
#  define RAWR_BUILD_ASAN 1
#else
#  define RAWR_BUILD_ASAN 0
#endif
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
#  define RAWR_BUILD_EXCEPTIONS 1
#else
#  define RAWR_BUILD_EXCEPTIONS 0
#endif
#define RAWR_BUILD_RTTI (defined(__cpp_rtti) || defined(_CPPRTTI))

// ── primitive types ───────────────────────────────────────────────────
namespace rawr {
    using u8  = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    using u64 = unsigned long long;
    using s8  = signed char;
    using s16 = signed short;
    using s32 = signed int;
    using s64 = signed long long;
    using f32 = float;
    using f64 = double;
    using upt = decltype(sizeof(0));
    using spt = decltype((char*)0 - (char*)0);
} // namespace rawr

// ── compiler/arch structural types ────────────────────────────────────
namespace rawr {
    struct compilers {
        enum class id_t : u32 { unknown=0, gcc, clang, msvc };
        id_t id = id_t::unknown;

        [[nodiscard]] constexpr bool is_gcc()   const noexcept { return id==id_t::gcc;            }
        [[nodiscard]] constexpr bool is_clang() const noexcept { return id==id_t::clang;          }
        [[nodiscard]] constexpr bool is_msvc()  const noexcept { return id==id_t::msvc;           }
        [[nodiscard]] constexpr bool is_gnu()   const noexcept { return is_gcc()||is_clang();     }
        [[nodiscard]] constexpr bool operator==(compilers o) const noexcept { return id==o.id; }
        [[nodiscard]] constexpr bool operator!=(compilers o) const noexcept { return id!=o.id; }

        static constexpr compilers gcc     = {id_t::gcc    };
        static constexpr compilers clang   = {id_t::clang  };
        static constexpr compilers msvc    = {id_t::msvc   };
        static constexpr compilers unknown = {id_t::unknown};
    };

    struct archs {
        enum class id_t : u32 { unknown=0, x64, x86, arm64, arm32, riscv64, riscv32 };
        id_t id = id_t::unknown;

        [[nodiscard]] constexpr bool is_x64()        const noexcept { return id==id_t::x64;              }
        [[nodiscard]] constexpr bool is_x86()        const noexcept { return id==id_t::x86;              }
        [[nodiscard]] constexpr bool is_arm64()      const noexcept { return id==id_t::arm64;            }
        [[nodiscard]] constexpr bool is_arm32()      const noexcept { return id==id_t::arm32;            }
        [[nodiscard]] constexpr bool is_x86_family() const noexcept { return is_x64()||is_x86();        }
        [[nodiscard]] constexpr bool is_arm_family() const noexcept { return is_arm64()||is_arm32();    }
        [[nodiscard]] constexpr bool operator==(archs o) const noexcept { return id==o.id; }
        [[nodiscard]] constexpr bool operator!=(archs o) const noexcept { return id!=o.id; }

        static constexpr archs x64     = {id_t::x64    };
        static constexpr archs x86     = {id_t::x86    };
        static constexpr archs arm64   = {id_t::arm64  };
        static constexpr archs arm32   = {id_t::arm32  };
        static constexpr archs unknown = {id_t::unknown};
    };

    struct build_flags {
        bool optimized  = RAWR_BUILD_OPTIMIZED;
        bool exceptions = RAWR_BUILD_EXCEPTIONS;
        bool rtti       = RAWR_BUILD_RTTI;
        bool asan       = RAWR_BUILD_ASAN;
    };

    inline constexpr compilers this_compiler =
#if   RAWR_COMPILER_CLANG
        compilers::clang;
#elif RAWR_COMPILER_GCC
        compilers::gcc;
#elif RAWR_COMPILER_MSVC
        compilers::msvc;
#else
        compilers::unknown;
#endif

    inline constexpr archs this_arch =
#if   RAWR_ARCH_X64
        archs::x64;
#elif RAWR_ARCH_X86
        archs::x86;
#elif RAWR_ARCH_ARM64
        archs::arm64;
#elif RAWR_ARCH_ARM32
        archs::arm32;
#else
        archs::unknown;
#endif

    inline constexpr build_flags this_build_flags{};

    // ── ISA feature structs ───────────────────────────────────────────
    struct x86_features {
        bool sse=0, sse2=0, sse41=0, sse42=0;
        bool avx=0, avx2=0, avx512f=0;
        bool fma=0, bmi1=0, bmi2=0, popcnt=0, lzcnt=0, clwb=0;
    };
    struct arm_features {
        bool neon=0, sve=0, sve2=0, dotprod=0, fp16=0, bf16=0;
    };

    // compile_features: what this BINARY targets (macro-derived, zero cost)
    // cpu_features:     what the RUNNING CPU supports (call detect_cpu_features())
    inline constexpr x86_features this_x86_features = {
        .sse    = RAWR_ARCH_X86_FEATURE_SSE,
        .sse2   = RAWR_ARCH_X86_FEATURE_SSE2,
        .sse41  = RAWR_ARCH_X86_FEATURE_SSE41,
        .sse42  = RAWR_ARCH_X86_FEATURE_SSE42,
        .avx    = RAWR_ARCH_X86_FEATURE_AVX,
        .avx2   = RAWR_ARCH_X86_FEATURE_AVX2,
        .avx512f= RAWR_ARCH_X86_FEATURE_AVX512F,
        .fma    = RAWR_ARCH_X86_FEATURE_FMA,
        .bmi1   = RAWR_ARCH_X86_FEATURE_BMI1,
        .bmi2   = RAWR_ARCH_X86_FEATURE_BMI2,
        .popcnt = RAWR_ARCH_X86_FEATURE_POPCNT,
        .lzcnt  = RAWR_ARCH_X86_FEATURE_LZCNT,
        .clwb   = RAWR_ARCH_X86_FEATURE_CLWB,
    };
    inline constexpr arm_features this_arm_features = {
        .neon   = RAWR_ARCH_ARM_FEATURE_NEON,
        .sve    = RAWR_ARCH_ARM_FEATURE_SVE,
        .sve2   = RAWR_ARCH_ARM_FEATURE_SVE2,
        .dotprod= RAWR_ARCH_ARM_FEATURE_DOTPROD,
        .fp16   = RAWR_ARCH_ARM_FEATURE_FP16,
        .bf16   = RAWR_ARCH_ARM_FEATURE_BF16,
    };
    // detect_cpu_features() — defined in a .cpp, calls CPUID / getauxval
    x86_features detect_x86_cpu_features() noexcept;
    arm_features detect_arm_cpu_features() noexcept;
} // namespace rawr

#pragma endregion


// ═══════════════════════════════════════════════════════════════════
#pragma region rawr/cxx/intrin/gcc_clang.hpp
// L1: GCC/Clang raw wrappers.
// CONTRACT: always declared. Definitions inside #if — only for
// identifiers/syntax that don't exist on other compilers.
// Not overridable here. Override at layer 2 via <C,A,T> specialization.
// ═══════════════════════════════════════════════════════════════════
namespace rawr::cxx::intrin::gcc_clang {

// ── byteswap — all arches ──────────────────────────────────────────
[[nodiscard]] constexpr auto bswap16(u16 v) noexcept -> u16;
[[nodiscard]] constexpr auto bswap32(u32 v) noexcept -> u32;
[[nodiscard]] constexpr auto bswap64(u64 v) noexcept -> u64;
#if RAWR_COMPILER_GNU
[[nodiscard]] constexpr auto bswap16(u16 v) noexcept -> u16 { return __builtin_bswap16(v); }
[[nodiscard]] constexpr auto bswap32(u32 v) noexcept -> u32 { return __builtin_bswap32(v); }
[[nodiscard]] constexpr auto bswap64(u64 v) noexcept -> u64 { return __builtin_bswap64(v); }
#endif

// ── popcount ──────────────────────────────────────────────────────
[[nodiscard]] constexpr auto popcount16(u16 v) noexcept -> u32;
[[nodiscard]] constexpr auto popcount32(u32 v) noexcept -> u32;
[[nodiscard]] constexpr auto popcount64(u64 v) noexcept -> u32;
#if RAWR_COMPILER_GNU
[[nodiscard]] constexpr auto popcount16(u16 v) noexcept -> u32 { return (u32)__builtin_popcount((u32)v);  }
[[nodiscard]] constexpr auto popcount32(u32 v) noexcept -> u32 { return (u32)__builtin_popcount(v);       }
[[nodiscard]] constexpr auto popcount64(u64 v) noexcept -> u32 { return (u32)__builtin_popcountll(v);     }
#endif

// ── clz / ctz — undefined on zero input (matches hardware UB) ─────
[[nodiscard]] auto clz32(u32 v) noexcept -> u32;
[[nodiscard]] auto clz64(u64 v) noexcept -> u32;
[[nodiscard]] auto ctz32(u32 v) noexcept -> u32;
[[nodiscard]] auto ctz64(u64 v) noexcept -> u32;
#if RAWR_COMPILER_GNU
[[nodiscard]] inline auto clz32(u32 v) noexcept -> u32 { return (u32)__builtin_clz(v);   }
[[nodiscard]] inline auto clz64(u64 v) noexcept -> u32 { return (u32)__builtin_clzll(v); }
[[nodiscard]] inline auto ctz32(u32 v) noexcept -> u32 { return (u32)__builtin_ctz(v);   }
[[nodiscard]] inline auto ctz64(u64 v) noexcept -> u32 { return (u32)__builtin_ctzll(v); }
#endif

// ── non-temporal store (x86-64) ───────────────────────────────────
void movnti32(u32* addr, u32 val) noexcept;
void movnti64(u64* addr, u64 val) noexcept;
#if RAWR_COMPILER_GNU && RAWR_ARCH_X64
inline void movnti32(u32* addr, u32 val) noexcept {
    __builtin_ia32_movnti(reinterpret_cast<int*>(addr), static_cast<int>(val));
}
inline void movnti64(u64* addr, u64 val) noexcept {
    __builtin_ia32_movnti64(reinterpret_cast<long long*>(addr), static_cast<long long>(val));
}
#endif

// ── fences ────────────────────────────────────────────────────────
void sfence() noexcept;
void mfence() noexcept;
void lfence() noexcept;
#if RAWR_COMPILER_GNU && RAWR_ARCH_X86_FEATURE_SSE
inline void sfence() noexcept { __builtin_ia32_sfence(); }
inline void mfence() noexcept { __builtin_ia32_mfence(); }
inline void lfence() noexcept { __builtin_ia32_lfence(); }
#endif

// ── clwb — ISA feature gate, not arch-width gate ──────────────────
void clwb(void const* addr) noexcept;
#if RAWR_COMPILER_GNU && RAWR_ARCH_X86_FEATURE_CLWB
inline void clwb(void const* addr) noexcept { __builtin_ia32_clwb(addr); }
#endif

// ── pause ─────────────────────────────────────────────────────────
void pause() noexcept;
#if RAWR_COMPILER_GNU && RAWR_ARCH_X86_FAMILY
inline void pause() noexcept { __builtin_ia32_pause(); }
#elif RAWR_COMPILER_GNU && RAWR_ARCH_ARM_FAMILY
inline void pause() noexcept { asm volatile("yield"); }
#endif

// ── GS/FS segment reads/writes (x86-64) ──────────────────────────
// volatile is load-bearing — GS/FS memory can change between reads.
// u32 offset zero-extended to upt before use as 64-bit segment offset.
[[nodiscard]] auto gs_read8 (u32 o) noexcept -> u8;
[[nodiscard]] auto gs_read16(u32 o) noexcept -> u16;
[[nodiscard]] auto gs_read32(u32 o) noexcept -> u32;
[[nodiscard]] auto gs_read64(u32 o) noexcept -> u64;
void               gs_write8 (u32 o, u8  v) noexcept;
void               gs_write16(u32 o, u16 v) noexcept;
void               gs_write32(u32 o, u32 v) noexcept;
void               gs_write64(u32 o, u64 v) noexcept;
[[nodiscard]] auto fs_read8 (u32 o) noexcept -> u8;
[[nodiscard]] auto fs_read16(u32 o) noexcept -> u16;
[[nodiscard]] auto fs_read32(u32 o) noexcept -> u32;
[[nodiscard]] auto fs_read64(u32 o) noexcept -> u64;
void               fs_write8 (u32 o, u8  v) noexcept;
void               fs_write16(u32 o, u16 v) noexcept;
void               fs_write32(u32 o, u32 v) noexcept;
void               fs_write64(u32 o, u64 v) noexcept;

#if RAWR_COMPILER_GNU && RAWR_ARCH_X64
#define RAWR_SEG_R_(seg, sz, insn)                                          \
    inline auto seg##_read##sz(u32 o) noexcept -> u##sz {                   \
        u##sz v;                                                             \
        asm volatile(insn " %%" #seg ":(%1),%0"                             \
            : "=r"(v) : "r"(static_cast<upt>(o))); return v;               \
    }
#define RAWR_SEG_W_(seg, sz, insn)                                          \
    inline void seg##_write##sz(u32 o, u##sz v) noexcept {                  \
        asm volatile(insn " %0,%%" #seg ":(%1)"                             \
            :: "r"(v), "r"(static_cast<upt>(o)));                           \
    }
RAWR_SEG_R_(gs, 8,"movb") RAWR_SEG_W_(gs, 8,"movb")
RAWR_SEG_R_(gs,16,"movw") RAWR_SEG_W_(gs,16,"movw")
RAWR_SEG_R_(gs,32,"movl") RAWR_SEG_W_(gs,32,"movl")
RAWR_SEG_R_(gs,64,"movq") RAWR_SEG_W_(gs,64,"movq")
RAWR_SEG_R_(fs, 8,"movb") RAWR_SEG_W_(fs, 8,"movb")
RAWR_SEG_R_(fs,16,"movw") RAWR_SEG_W_(fs,16,"movw")
RAWR_SEG_R_(fs,32,"movl") RAWR_SEG_W_(fs,32,"movl")
RAWR_SEG_R_(fs,64,"movq") RAWR_SEG_W_(fs,64,"movq")
#undef RAWR_SEG_R_
#undef RAWR_SEG_W_
#endif

} // namespace rawr::cxx::intrin::gcc_clang
#pragma endregion


// ═══════════════════════════════════════════════════════════════════
#pragma region rawr/cxx/intrin/msvc.hpp
// L1: MSVC raw wrappers.
// #pragma intrinsic types must EXACTLY match MSVC's internal table.
// unsigned long != unsigned int even at the same size (LLP64).
// __readgs* / __readfs* are implicitly intrinsic — no pragma needed.
// _mm_clwb is the correct MSVC name. __builtin_ia32_clwb does not exist on MSVC.
// ═══════════════════════════════════════════════════════════════════
namespace rawr::cxx::intrin::msvc {

[[nodiscard]] constexpr auto bswap16(u16 v) noexcept -> u16;
[[nodiscard]] constexpr auto bswap32(u32 v) noexcept -> u32;
[[nodiscard]] constexpr auto bswap64(u64 v) noexcept -> u64;
[[nodiscard]] auto clz32(u32 v) noexcept -> u32;
[[nodiscard]] auto clz64(u64 v) noexcept -> u32;
[[nodiscard]] auto ctz32(u32 v) noexcept -> u32;
[[nodiscard]] auto ctz64(u64 v) noexcept -> u32;
[[nodiscard]] auto popcount32(u32 v) noexcept -> u32;
[[nodiscard]] auto popcount64(u64 v) noexcept -> u32;
void stream_si32(u32* addr, u32 val) noexcept;
void stream_si64(u64* addr, u64 val) noexcept;
void sfence() noexcept;
void mfence() noexcept;
void lfence() noexcept;
void clwb(void const* addr) noexcept;
void pause() noexcept;
[[nodiscard]] auto gs_read8 (u32 o) noexcept -> u8;
[[nodiscard]] auto gs_read16(u32 o) noexcept -> u16;
[[nodiscard]] auto gs_read32(u32 o) noexcept -> u32;
[[nodiscard]] auto gs_read64(u32 o) noexcept -> u64;
void               gs_write8 (u32 o, u8  v) noexcept;
void               gs_write16(u32 o, u16 v) noexcept;
void               gs_write32(u32 o, u32 v) noexcept;
void               gs_write64(u32 o, u64 v) noexcept;
[[nodiscard]] auto fs_read8 (u32 o) noexcept -> u8;
[[nodiscard]] auto fs_read16(u32 o) noexcept -> u16;
[[nodiscard]] auto fs_read32(u32 o) noexcept -> u32;
void               fs_write8 (u32 o, u8  v) noexcept;
void               fs_write16(u32 o, u16 v) noexcept;
void               fs_write32(u32 o, u32 v) noexcept;

#if RAWR_COMPILER_MSVC
extern "C" {
    auto _byteswap_ushort(unsigned short)     noexcept -> unsigned short;
    auto _byteswap_ulong (unsigned long)      noexcept -> unsigned long;
    auto _byteswap_uint64(unsigned long long) noexcept -> unsigned long long;
    unsigned int  __popcnt  (unsigned int)          noexcept;
    unsigned long long __popcnt64(unsigned long long) noexcept;
    void _mm_stream_si32(int*,     int)      noexcept;
    void _mm_stream_si64(__int64*, __int64)  noexcept;
    void _mm_sfence()                        noexcept;
    void _mm_mfence()                        noexcept;
    void _mm_lfence()                        noexcept;
    void _mm_pause()                         noexcept;
}
__pragma(intrinsic(_byteswap_ushort))
__pragma(intrinsic(_byteswap_ulong))
__pragma(intrinsic(_byteswap_uint64))
__pragma(intrinsic(_mm_sfence))
__pragma(intrinsic(_mm_mfence))
__pragma(intrinsic(_mm_lfence))
__pragma(intrinsic(_mm_pause))
__pragma(intrinsic(_mm_stream_si32))

[[nodiscard]] constexpr auto bswap16(u16 v) noexcept -> u16 { return (u16)_byteswap_ushort((unsigned short)v); }
[[nodiscard]] constexpr auto bswap32(u32 v) noexcept -> u32 { return (u32)_byteswap_ulong ((unsigned long) v); }
[[nodiscard]] constexpr auto bswap64(u64 v) noexcept -> u64 { return (u64)_byteswap_uint64((unsigned long long)v); }
[[nodiscard]] inline auto popcount32(u32 v) noexcept -> u32 { return (u32)__popcnt(v);   }
[[nodiscard]] inline auto popcount64(u64 v) noexcept -> u32 { return (u32)__popcnt64(v); }
inline void sfence() noexcept { _mm_sfence(); }
inline void mfence() noexcept { _mm_mfence(); }
inline void lfence() noexcept { _mm_lfence(); }
inline void pause()  noexcept { _mm_pause();  }
inline void stream_si32(u32* a, u32 v) noexcept { _mm_stream_si32((int*)a, (int)v); }
inline void stream_si64(u64* a, u64 v) noexcept { _mm_stream_si64((__int64*)a, (__int64)v); }

// clz/ctz: _BitScanReverse output param + return-bool model.
// Undefined on v=0, matching hardware and __builtin_clz contract.
[[nodiscard]] inline auto clz32(u32 v) noexcept -> u32 {
    unsigned long idx; _BitScanReverse(&idx, (unsigned long)v); return 31u - (u32)idx;
}
[[nodiscard]] inline auto clz64(u64 v) noexcept -> u32 {
    unsigned long idx; _BitScanReverse64(&idx, v); return 63u - (u32)idx;
}
[[nodiscard]] inline auto ctz32(u32 v) noexcept -> u32 {
    unsigned long idx; _BitScanForward(&idx, (unsigned long)v); return (u32)idx;
}
[[nodiscard]] inline auto ctz64(u64 v) noexcept -> u32 {
    unsigned long idx; _BitScanForward64(&idx, v); return (u32)idx;
}

#if RAWR_ARCH_X64 && RAWR_ARCH_X86_FEATURE_CLWB
extern "C" void _mm_clwb(void const*) noexcept;
__pragma(intrinsic(_mm_clwb))
inline void clwb(void const* addr) noexcept { _mm_clwb(addr); }
#endif

extern "C" {
    unsigned char      __readgsbyte  (unsigned long o) noexcept;
    unsigned short     __readgsword  (unsigned long o) noexcept;
    unsigned long      __readgsdword (unsigned long o) noexcept;
    unsigned long long __readgsqword (unsigned long o) noexcept;
    void __writegsbyte (unsigned long o, unsigned char      v) noexcept;
    void __writegsword (unsigned long o, unsigned short     v) noexcept;
    void __writegsdword(unsigned long o, unsigned long      v) noexcept;
    void __writegsqword(unsigned long o, unsigned long long v) noexcept;
}
#if RAWR_ARCH_X64
inline auto gs_read8 (u32 o) noexcept->u8  { return (u8) __readgsbyte ((unsigned long)o); }
inline auto gs_read16(u32 o) noexcept->u16 { return (u16)__readgsword ((unsigned long)o); }
inline auto gs_read32(u32 o) noexcept->u32 { return (u32)__readgsdword((unsigned long)o); }
inline auto gs_read64(u32 o) noexcept->u64 { return (u64)__readgsqword((unsigned long)o); }
inline void gs_write8 (u32 o,u8  v) noexcept { __writegsbyte ((unsigned long)o,(unsigned char)     v); }
inline void gs_write16(u32 o,u16 v) noexcept { __writegsword ((unsigned long)o,(unsigned short)    v); }
inline void gs_write32(u32 o,u32 v) noexcept { __writegsdword((unsigned long)o,(unsigned long)     v); }
inline void gs_write64(u32 o,u64 v) noexcept { __writegsqword((unsigned long)o,(unsigned long long)v); }
#endif
#if RAWR_ARCH_X86
extern "C" {
    unsigned char  __readfsbyte (unsigned long o) noexcept;
    unsigned short __readfsword (unsigned long o) noexcept;
    unsigned long  __readfsdword(unsigned long o) noexcept;
    void __writefsbyte (unsigned long o, unsigned char  v) noexcept;
    void __writefsword (unsigned long o, unsigned short v) noexcept;
    void __writefsdword(unsigned long o, unsigned long  v) noexcept;
}
inline auto fs_read8 (u32 o) noexcept->u8  { return (u8) __readfsbyte ((unsigned long)o); }
inline auto fs_read16(u32 o) noexcept->u16 { return (u16)__readfsword ((unsigned long)o); }
inline auto fs_read32(u32 o) noexcept->u32 { return (u32)__readfsdword((unsigned long)o); }
inline void fs_write8 (u32 o,u8  v) noexcept { __writefsbyte ((unsigned long)o,(unsigned char) v); }
inline void fs_write16(u32 o,u16 v) noexcept { __writefsword ((unsigned long)o,(unsigned short)v); }
inline void fs_write32(u32 o,u32 v) noexcept { __writefsdword((unsigned long)o,(unsigned long) v); }
#endif
#endif // RAWR_COMPILER_MSVC
} // namespace rawr::cxx::intrin::msvc
#pragma endregion


// ═══════════════════════════════════════════════════════════════════
#pragma region rawr/lib/attributes.pp
// L2: public dispatch + overridable API.
// Override any op for a specific <C,A,T> via explicit specialization
// before any use that would implicitly instantiate that combination.
// ═══════════════════════════════════════════════════════════════════
namespace rawr::cxx::intrin {

namespace detail { template <auto> inline constexpr bool unimplemented = false; }
enum class prefetch_locality : int { none=0, low=1, medium=2, high=3 };

// ── supports_* — single source of truth ──────────────────────────
template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_byteswap = C.is_gnu() || C.is_msvc();

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_popcount = C.is_gnu() || C.is_msvc();

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_clz = C.is_gnu() || C.is_msvc();

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_ctz = C.is_gnu() || C.is_msvc();

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_nt_store = (C.is_gnu()||C.is_msvc()) && A.is_x64();

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_sfence =
    (C.is_gnu()||C.is_msvc()) && A.is_x86_family() && RAWR_ARCH_X86_FEATURE_SSE;

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_mfence = supports_sfence<C,A>;

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_lfence = supports_sfence<C,A>;

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_clwb =
    (C.is_gnu()||C.is_msvc()) && A.is_x86_family() && RAWR_ARCH_X86_FEATURE_CLWB;

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_gs_read =
    C.is_gnu() ? A.is_x86_family() : (C.is_msvc() && A.is_x64());

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_gs_write = supports_gs_read<C,A>;

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_fs_read =
    C.is_gnu() ? A.is_x86_family() : (C.is_msvc() && A.is_x86());

template <compilers C=this_compiler, archs A=this_arch>
inline constexpr bool supports_fs_write = supports_fs_read<C,A>;

// ── is_constant_evaluated — Category C ───────────────────────────
// false is always honest: runtime=correct, compile-time=gated ops
// fail at their own sites with their own diagnostic.
// defined(__has_builtin) guard is mandatory: older GCC lacks __has_builtin
// entirely; the bare form is a preprocessor syntax error, not 0.
[[nodiscard]] constexpr auto is_constant_evaluated() noexcept -> bool {
#if defined(__has_builtin) && __has_builtin(__builtin_is_constant_evaluated)
    return __builtin_is_constant_evaluated();
#elif RAWR_COMPILER_MSVC && RAWR_COMPILER_VERSION_MAJOR >= 19 && RAWR_COMPILER_VERSION_MINOR >= 25
    return __builtin_is_constant_evaluated();
#else
    return false;
#endif
}

// ── prefetch — degenerate (no-op is always honest) ───────────────
template <prefetch_locality L=prefetch_locality::high,
          compilers C=this_compiler, archs A=this_arch, typename T>
void prefetch_read(T const* addr) noexcept {
#if RAWR_COMPILER_GNU
    __builtin_prefetch(addr, 0, static_cast<int>(L));
#else
    (void)addr;
#endif
}
template <prefetch_locality L=prefetch_locality::high,
          compilers C=this_compiler, archs A=this_arch, typename T>
void prefetch_write(T* addr) noexcept {
#if RAWR_COMPILER_GNU
    __builtin_prefetch(addr, 1, static_cast<int>(L));
#else
    (void)addr;
#endif
}

// ── byteswap — Category A ────────────────────────────────────────
template <compilers C=this_compiler, archs A=this_arch, typename T>
[[nodiscard]] constexpr auto byteswap(T v) noexcept -> T {
    static_assert(sizeof(T)==1||sizeof(T)==2||sizeof(T)==4||sizeof(T)==8,
        "byteswap: T must be 1, 2, 4, or 8 bytes");
    if constexpr (sizeof(T)==1) { return v; }
    else if constexpr (supports_byteswap<C,A>) {
        if constexpr (C.is_msvc()) {
            if constexpr (sizeof(T)==2) return (T)msvc::bswap16((u16)v);
            else if constexpr (sizeof(T)==4) return (T)msvc::bswap32((u32)v);
            else                             return (T)msvc::bswap64((u64)v);
        } else {
            if constexpr (sizeof(T)==2) return (T)gcc_clang::bswap16((u16)v);
            else if constexpr (sizeof(T)==4) return (T)gcc_clang::bswap32((u32)v);
            else                             return (T)gcc_clang::bswap64((u64)v);
        }
    } else {
        // Portable fallback — optimizers will pattern-match to bswap/rev
        if constexpr (sizeof(T)==2) {
            u16 u=(u16)v; return (T)(u16)((u<<8)|(u>>8));
        } else if constexpr (sizeof(T)==4) {
            u32 u=(u32)v;
            u=((u&0x00FF00FFu)<<8)|((u>>8)&0x00FF00FFu);
            return (T)((u<<16)|(u>>16));
        } else {
            u64 u=(u64)v;
            u=((u&0x00FF00FF00FF00FFull)<<8) |((u>>8) &0x00FF00FF00FF00FFull);
            u=((u&0x0000FFFF0000FFFFull)<<16)|((u>>16)&0x0000FFFF0000FFFFull);
            return (T)((u<<32)|(u>>32));
        }
    }
}

// ── popcount / clz / ctz — Category A ────────────────────────────
template <compilers C=this_compiler, archs A=this_arch, typename T>
[[nodiscard]] auto popcount(T v) noexcept -> u32 {
    static_assert(sizeof(T)==2||sizeof(T)==4||sizeof(T)==8);
    if constexpr (supports_popcount<C,A>) {
        if constexpr (C.is_msvc()) {
            if constexpr (sizeof(T)<=4) return msvc::popcount32((u32)v);
            else                        return msvc::popcount64((u64)v);
        } else {
            if constexpr (sizeof(T)==2) return gcc_clang::popcount16((u16)v);
            else if constexpr (sizeof(T)==4) return gcc_clang::popcount32((u32)v);
            else                             return gcc_clang::popcount64((u64)v);
        }
    } else {
        // Portable Hamming weight
        u64 u=(u64)v;
        u = u - ((u>>1)&0x5555555555555555ull);
        u = (u&0x3333333333333333ull)+((u>>2)&0x3333333333333333ull);
        return (u32)(((u+(u>>4))&0x0F0F0F0F0F0F0F0Full)*0x0101010101010101ull>>56);
    }
}

// clz/ctz: undefined on zero — same contract as hardware instruction
template <compilers C=this_compiler, archs A=this_arch, typename T>
[[nodiscard]] auto clz(T v) noexcept -> u32 {
    static_assert(sizeof(T)==4||sizeof(T)==8);
    if constexpr (supports_clz<C,A>) {
        if constexpr (C.is_msvc()) {
            if constexpr (sizeof(T)==4) return msvc::clz32((u32)v);
            else                        return msvc::clz64((u64)v);
        } else {
            if constexpr (sizeof(T)==4) return gcc_clang::clz32((u32)v);
            else                        return gcc_clang::clz64((u64)v);
        }
    } else {
        static_assert(detail::unimplemented<C>, "clz: no backend");
        return 0;
    }
}

template <compilers C=this_compiler, archs A=this_arch, typename T>
[[nodiscard]] auto ctz(T v) noexcept -> u32 {
    static_assert(sizeof(T)==4||sizeof(T)==8);
    if constexpr (supports_ctz<C,A>) {
        if constexpr (C.is_msvc()) {
            if constexpr (sizeof(T)==4) return msvc::ctz32((u32)v);
            else                        return msvc::ctz64((u64)v);
        } else {
            if constexpr (sizeof(T)==4) return gcc_clang::ctz32((u32)v);
            else                        return gcc_clang::ctz64((u64)v);
        }
    } else {
        static_assert(detail::unimplemented<C>, "ctz: no backend");
        return 0;
    }
}

// ── nt_store / fences / clwb_sfence — Category B ─────────────────
template <compilers C=this_compiler, archs A=this_arch, typename T>
void nt_store(T* addr, T val) noexcept {
    static_assert(sizeof(T)==4||sizeof(T)==8,
        "nt_store: MOVNTI is 32/64-bit only; no 8 or 16-bit NT scalar store in x86");
    if constexpr (supports_nt_store<C,A>) {
        if constexpr (C.is_msvc()) {
            if constexpr (sizeof(T)==4) msvc::stream_si32((u32*)addr,(u32)val);
            else                        msvc::stream_si64((u64*)addr,(u64)val);
        } else {
            if constexpr (sizeof(T)==4) gcc_clang::movnti32((u32*)addr,(u32)val);
            else                        gcc_clang::movnti64((u64*)addr,(u64)val);
        }
    } else {
        static_assert(detail::unimplemented<C>,
            "nt_store: no backend — a plain store is not a substitute");
    }
}

template <compilers C=this_compiler, archs A=this_arch>
void sfence() noexcept {
    if constexpr (supports_sfence<C,A>) {
        if constexpr (C.is_msvc()) msvc::sfence(); else gcc_clang::sfence();
    } else { static_assert(detail::unimplemented<C>, "sfence: no backend"); }
}

template <compilers C=this_compiler, archs A=this_arch>
void mfence() noexcept {
    if constexpr (supports_mfence<C,A>) {
        if constexpr (C.is_msvc()) msvc::mfence(); else gcc_clang::mfence();
    } else { static_assert(detail::unimplemented<C>, "mfence: no backend"); }
}

template <compilers C=this_compiler, archs A=this_arch>
void lfence() noexcept {
    if constexpr (supports_lfence<C,A>) {
        if constexpr (C.is_msvc()) msvc::lfence(); else gcc_clang::lfence();
    } else { static_assert(detail::unimplemented<C>, "lfence: no backend"); }
}

// clwb_sfence: bundled intentionally. Persistence requires both.
// Splitting them invites the "flush without fence" class of bug.
// For sfence-only after NT stores, call sfence() above explicitly.
template <compilers C=this_compiler, archs A=this_arch, typename T>
void clwb_sfence(T const* addr) noexcept {
    if constexpr (supports_clwb<C,A>) {
        if constexpr (C.is_msvc()) { msvc::clwb(addr);      msvc::sfence();      }
        else                       { gcc_clang::clwb(addr); gcc_clang::sfence(); }
    } else {
        static_assert(detail::unimplemented<C>,
            "clwb_sfence: check RAWR_ARCH_X86_FEATURE_CLWB + -mclwb flag");
    }
}

// ── segment reads/writes — detail helpers + named public API ──────
namespace detail {
    template <typename T, compilers C, archs A>
    [[nodiscard]] auto seg_read(u32 o, bool gs) noexcept -> T;
    // Full dispatch omitted for brevity — see gs_read*/fs_read* below
    // which follow the same shape as byteswap's compiler dispatch.
}

// Public API: separate names per width — the width IS the semantic.
// gs_read32(0x60) != gs_read64(0x60): materially different operations.
#define RAWR_INTRIN_SEG_RD_(seg, sz)                                              \
template <compilers C=this_compiler, archs A=this_arch>                           \
[[nodiscard]] auto seg##_read##sz(u32 o) noexcept -> u##sz {                      \
    if constexpr (supports_##seg##_read<C,A>) {                                   \
        if constexpr (C.is_msvc()) return msvc::seg##_read##sz(o);               \
        else                       return gcc_clang::seg##_read##sz(o);           \
    } else { static_assert(detail::unimplemented<C>, #seg "_read" #sz ": no backend"); return {}; } \
}
#define RAWR_INTRIN_SEG_WR_(seg, sz)                                              \
template <compilers C=this_compiler, archs A=this_arch>                           \
void seg##_write##sz(u32 o, u##sz v) noexcept {                                   \
    if constexpr (supports_##seg##_write<C,A>) {                                  \
        if constexpr (C.is_msvc()) msvc::seg##_write##sz(o,v);                   \
        else                       gcc_clang::seg##_write##sz(o,v);               \
    } else { static_assert(detail::unimplemented<C>, #seg "_write" #sz ": no backend"); } \
}
RAWR_INTRIN_SEG_RD_(gs, 8)  RAWR_INTRIN_SEG_WR_(gs, 8)
RAWR_INTRIN_SEG_RD_(gs,16)  RAWR_INTRIN_SEG_WR_(gs,16)
RAWR_INTRIN_SEG_RD_(gs,32)  RAWR_INTRIN_SEG_WR_(gs,32)
RAWR_INTRIN_SEG_RD_(gs,64)  RAWR_INTRIN_SEG_WR_(gs,64)
RAWR_INTRIN_SEG_RD_(fs, 8)  RAWR_INTRIN_SEG_WR_(fs, 8)
RAWR_INTRIN_SEG_RD_(fs,16)  RAWR_INTRIN_SEG_WR_(fs,16)
RAWR_INTRIN_SEG_RD_(fs,32)  RAWR_INTRIN_SEG_WR_(fs,32)
RAWR_INTRIN_SEG_RD_(fs,64)  RAWR_INTRIN_SEG_WR_(fs,64)
#undef RAWR_INTRIN_SEG_RD_
#undef RAWR_INTRIN_SEG_WR_

// ── support matrix ────────────────────────────────────────────────
namespace detail { struct support_row { char const* op; bool ok; }; }
inline constexpr detail::support_row this_support_matrix[] = {
    {"byteswap",    supports_byteswap<>   },
    {"popcount",    supports_popcount<>   },
    {"clz",         supports_clz<>        },
    {"ctz",         supports_ctz<>        },
    {"nt_store",    supports_nt_store<>   },
    {"sfence",      supports_sfence<>     },
    {"mfence",      supports_mfence<>     },
    {"lfence",      supports_lfence<>     },
    {"clwb_sfence", supports_clwb<>       },
    {"gs_read",     supports_gs_read<>    },
    {"gs_write",    supports_gs_write<>   },
    {"fs_read",     supports_fs_read<>    },
    {"fs_write",    supports_fs_write<>   },
};
} // namespace rawr::cxx::intrin
#pragma endregion


// ═══════════════════════════════════════════════════════════════════
#pragma region rawr/cxx/intrin/simd.hpp
// SIMD types: f32x4, f64x2, u8x16, s8x16, u16x8, s16x8, u32x4,
//             s32x4, u64x2, s64x2 (128-bit)
//             f32x8, f64x4, u8x32, s8x32, u16x16, s16x16, u32x8,
//             s32x8, u64x4, s64x4 (256-bit)
//
// Naming: element type + lane count. Width is derived, never stated.
// One member: `reg`. No lanes[] alongside native register types —
// union coexistence forces memory-addressability and pervasive spilling.
// Lane access is via extract<I>() and store(), which have explicit cost.
// ═══════════════════════════════════════════════════════════════════

// ── compiler intrinsic headers ────────────────────────────────────
// These are compiler-provided headers, not libc. Freestanding-safe.
#if RAWR_ARCH_X86_FAMILY && (RAWR_COMPILER_GNU || RAWR_COMPILER_MSVC)
#  include <immintrin.h>
#endif
#if RAWR_ARCH_ARM_FEATURE_NEON && RAWR_COMPILER_GNU
#  include <arm_neon.h>
#endif

namespace rawr::simd {

// ── backend selector constants ────────────────────────────────────
// Used in #if inside method bodies. #if is correct here: the inactive
// branches reference identifiers that don't exist on other backends.
// if constexpr would still require all names to be declared (not in template).
#define RAWR_SIMD_X86_128 (RAWR_ARCH_X86_FAMILY && RAWR_ARCH_X86_FEATURE_SSE2)
#define RAWR_SIMD_X86_256 (RAWR_ARCH_X86_FAMILY && RAWR_ARCH_X86_FEATURE_AVX)
#define RAWR_SIMD_X86_256I (RAWR_ARCH_X86_FAMILY && RAWR_ARCH_X86_FEATURE_AVX2)
#define RAWR_SIMD_X86_SSE41 (RAWR_ARCH_X86_FAMILY && RAWR_ARCH_X86_FEATURE_SSE41)
#define RAWR_SIMD_ARM (RAWR_ARCH_ARM_FEATURE_NEON && RAWR_COMPILER_GNU)

// ── storage macros ────────────────────────────────────────────────
// Each expands to `<native_type> reg` or `<elem_t> reg[n]`.
// Uniform name `reg` means all method macros reference `reg` without
// knowing which arch is active.
#if RAWR_SIMD_X86_128
#  define RAWR_VEC_STORAGE_f32x4  __m128   reg
#  define RAWR_VEC_STORAGE_f64x2  __m128d  reg
#  define RAWR_VEC_STORAGE_u8x16  __m128i  reg
#  define RAWR_VEC_STORAGE_s8x16  __m128i  reg
#  define RAWR_VEC_STORAGE_u16x8  __m128i  reg
#  define RAWR_VEC_STORAGE_s16x8  __m128i  reg
#  define RAWR_VEC_STORAGE_u32x4  __m128i  reg
#  define RAWR_VEC_STORAGE_s32x4  __m128i  reg
#  define RAWR_VEC_STORAGE_u64x2  __m128i  reg
#  define RAWR_VEC_STORAGE_s64x2  __m128i  reg
#elif RAWR_SIMD_ARM
#  define RAWR_VEC_STORAGE_f32x4  float32x4_t   reg
#  define RAWR_VEC_STORAGE_f64x2  float64x2_t   reg
#  define RAWR_VEC_STORAGE_u8x16  uint8x16_t    reg
#  define RAWR_VEC_STORAGE_s8x16  int8x16_t     reg
#  define RAWR_VEC_STORAGE_u16x8  uint16x8_t    reg
#  define RAWR_VEC_STORAGE_s16x8  int16x8_t     reg
#  define RAWR_VEC_STORAGE_u32x4  uint32x4_t    reg
#  define RAWR_VEC_STORAGE_s32x4  int32x4_t     reg
#  define RAWR_VEC_STORAGE_u64x2  uint64x2_t    reg
#  define RAWR_VEC_STORAGE_s64x2  int64x2_t     reg
#else
#  define RAWR_VEC_STORAGE_f32x4  f32 reg[4]
#  define RAWR_VEC_STORAGE_f64x2  f64 reg[2]
#  define RAWR_VEC_STORAGE_u8x16  u8  reg[16]
#  define RAWR_VEC_STORAGE_s8x16  s8  reg[16]
#  define RAWR_VEC_STORAGE_u16x8  u16 reg[8]
#  define RAWR_VEC_STORAGE_s16x8  s16 reg[8]
#  define RAWR_VEC_STORAGE_u32x4  u32 reg[4]
#  define RAWR_VEC_STORAGE_s32x4  s32 reg[4]
#  define RAWR_VEC_STORAGE_u64x2  u64 reg[2]
#  define RAWR_VEC_STORAGE_s64x2  s64 reg[2]
#endif

#if RAWR_SIMD_X86_256
#  define RAWR_VEC_STORAGE_f32x8  __m256   reg
#  define RAWR_VEC_STORAGE_f64x4  __m256d  reg
#else
#  define RAWR_VEC_STORAGE_f32x8  f32 reg[8]
#  define RAWR_VEC_STORAGE_f64x4  f64 reg[4]
#endif
#if RAWR_SIMD_X86_256I
#  define RAWR_VEC_STORAGE_u8x32  __m256i  reg
#  define RAWR_VEC_STORAGE_s8x32  __m256i  reg
#  define RAWR_VEC_STORAGE_u16x16 __m256i  reg
#  define RAWR_VEC_STORAGE_s16x16 __m256i  reg
#  define RAWR_VEC_STORAGE_u32x8  __m256i  reg
#  define RAWR_VEC_STORAGE_s32x8  __m256i  reg
#  define RAWR_VEC_STORAGE_u64x4  __m256i  reg
#  define RAWR_VEC_STORAGE_s64x4  __m256i  reg
#else
#  define RAWR_VEC_STORAGE_u8x32  u8  reg[32]
#  define RAWR_VEC_STORAGE_s8x32  s8  reg[32]
#  define RAWR_VEC_STORAGE_u16x16 u16 reg[16]
#  define RAWR_VEC_STORAGE_s16x16 s16 reg[16]
#  define RAWR_VEC_STORAGE_u32x8  u32 reg[8]
#  define RAWR_VEC_STORAGE_s32x8  s32 reg[8]
#  define RAWR_VEC_STORAGE_u64x4  u64 reg[4]
#  define RAWR_VEC_STORAGE_s64x4  s64 reg[4]
#endif

// ── dispatch macro for binary ops ─────────────────────────────────
// Defined once per active backend. Arguments appearing in inactive
// branches are discarded preprocessor tokens — never reach the compiler.
// This is intentional: x86 intrinsic names in ARM arguments and vice
// versa are valid token sequences even when undefined as identifiers.
#if RAWR_SIMD_X86_128 || RAWR_SIMD_X86_256
#  define RAWR_SIMD_OP3(ret, name, p0, p1, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0, p1) noexcept { x86_body }
#  define RAWR_SIMD_OP2(ret, name, p0, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0)     noexcept { x86_body }
#elif RAWR_SIMD_ARM
#  define RAWR_SIMD_OP3(ret, name, p0, p1, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0, p1) noexcept { arm_body }
#  define RAWR_SIMD_OP2(ret, name, p0, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0)     noexcept { arm_body }
#else
#  define RAWR_SIMD_OP3(ret, name, p0, p1, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0, p1) noexcept { scalar_body }
#  define RAWR_SIMD_OP2(ret, name, p0, x86_body, arm_body, scalar_body) \
     [[nodiscard]] inline ret name(p0)     noexcept { scalar_body }
#endif

// ── extract helper ────────────────────────────────────────────────
// GCC/Clang: reg[I] is the vector subscript extension — pure register op.
// MSVC: store to aligned tmp (perf TODO: type-specific intrinsics per width).
// Scalar: plain array index.
// Template required so I is a compile-time constant in all paths.
#if RAWR_COMPILER_GNU
#  define RAWR_SIMD_EXTRACT(T, n, elem_t)                                  \
     template <upt I>                                                       \
     [[nodiscard]] auto extract() const noexcept -> elem_t {               \
         static_assert(I < n, "extract: index out of range");              \
         return static_cast<elem_t>(reg[I]);                               \
     }
#elif RAWR_COMPILER_MSVC
#  define RAWR_SIMD_EXTRACT(T, n, elem_t)                                  \
     template <upt I>                                                       \
     [[nodiscard]] auto extract() const noexcept -> elem_t {               \
         static_assert(I < n, "extract: index out of range");              \
         alignas(sizeof(T)) elem_t tmp[n];                                 \
         __builtin_memcpy(tmp, &reg, sizeof(T)); return tmp[I];            \
     }
#else
#  define RAWR_SIMD_EXTRACT(T, n, elem_t)                                  \
     template <upt I>                                                       \
     [[nodiscard]] auto extract() const noexcept -> elem_t {               \
         static_assert(I < n, "extract: index out of range");              \
         return reg[I];                                                     \
     }
#endif

// ── as<Target>() — explicit bit reinterpret ───────────────────────
// __builtin_memcpy: the safe bit-cast mechanism. Eliminated entirely
// by optimizers when src/dst types are the same size and register-held.
// RAWR_SIMD_AS is used inside struct bodies where 'this' is available.
#define RAWR_SIMD_AS(self_t)                                               \
    template <typename Target>                                             \
    [[nodiscard]] auto as() const noexcept -> Target {                    \
        static_assert(sizeof(Target)==sizeof(self_t),                     \
            "as<>: Target must have the same byte width");                 \
        Target t{};                                                        \
        __builtin_memcpy(&t, this, sizeof(self_t));                       \
        return t;                                                          \
    }

// ─────────────────────────────────────────────────────────────────
// f32x4 — 4 × f32, 128-bit
// ─────────────────────────────────────────────────────────────────
struct f32x4 {
    static constexpr upt  lanes = 4;
    using elem_type = f32;
    RAWR_VEC_STORAGE_f32x4;
    f32x4() noexcept = default;

    // ── factory ────────────────────────────────────────────────────
    [[nodiscard]] static auto broadcast(f32 v) noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_set1_ps(v)};
#elif RAWR_SIMD_ARM
        return {vdupq_n_f32(v)};
#else
        f32x4 r{}; for (upt i=0;i<4;++i) r.reg[i]=v; return r;
#endif
    }

    // ── load / store ───────────────────────────────────────────────
    // Default: unaligned. Zero throughput penalty on aligned addresses
    // on modern hardware (post-Nehalem x86; all AArch64).
    // Aligned variants: caller contract ptr % 16 == 0. UB otherwise.
    [[nodiscard]] static auto load(f32 const* p) noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_loadu_ps(p)};
#elif RAWR_SIMD_ARM
        return {vld1q_f32(p)};
#else
        f32x4 r{}; __builtin_memcpy(r.reg, p, 16); return r;
#endif
    }
    [[nodiscard]] static auto load_aligned(f32 const* p) noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_load_ps(p)};
#elif RAWR_SIMD_ARM
        return {vld1q_f32(p)};  // ARM handles alignment internally
#else
        return load(p);
#endif
    }
    void store(f32* p) const noexcept {
#if   RAWR_SIMD_X86_128
        _mm_storeu_ps(p, reg);
#elif RAWR_SIMD_ARM
        vst1q_f32(p, reg);
#else
        __builtin_memcpy(p, reg, 16);
#endif
    }
    void store_aligned(f32* p) const noexcept {
#if   RAWR_SIMD_X86_128
        _mm_store_ps(p, reg);
#elif RAWR_SIMD_ARM
        vst1q_f32(p, reg);
#else
        store(p);
#endif
    }

    // ── lane access ────────────────────────────────────────────────
    RAWR_SIMD_EXTRACT(f32x4, 4, f32)
    RAWR_SIMD_AS(f32x4)

    // ── arithmetic ─────────────────────────────────────────────────
    [[nodiscard]] auto operator+(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_add_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vaddq_f32(reg, b.reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]+b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator-(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_sub_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vsubq_f32(reg, b.reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]-b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator*(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_mul_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vmulq_f32(reg, b.reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]*b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator/(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_div_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM && RAWR_ARCH_ARM64
        return {vdivq_f32(reg, b.reg)};  // ARM64 only; ARM32 has no vdivq
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]/b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator-() const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_xor_ps(reg, _mm_set1_ps(-0.0f))};  // flip sign bit
#elif RAWR_SIMD_ARM
        return {vnegq_f32(reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=-reg[i]; return r;
#endif
    }

    // ── min / max — named, not operators; NaN semantics differ ─────
    // x86 _mm_min_ps: returns b when a==NaN; vminq_f32: unspecified on NaN.
    // Document: behaviour on NaN is implementation-defined at this layer.
    [[nodiscard]] auto min(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_min_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vminq_f32(reg, b.reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]<b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto max(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_max_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vmaxq_f32(reg, b.reg)};
#else
        f32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]>b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }

    // ── fma — Category B: single rounding, not a+b*c substitutable ─
    // Only defined when FMA is a compile-time target feature.
    // A two-op fallback (mul then add) gives a different numerical result
    // and is NOT silently inserted here.
#if RAWR_ARCH_X86_FEATURE_FMA || RAWR_SIMD_ARM
    [[nodiscard]] auto fma(f32x4 b, f32x4 c) const noexcept -> f32x4 {
        // this*b + c
#if   RAWR_ARCH_X86_FEATURE_FMA
        return {_mm_fmadd_ps(reg, b.reg, c.reg)};
#elif RAWR_SIMD_ARM
        return {vfmaq_f32(c.reg, reg, b.reg)};  // c + a*b
#endif
    }
#endif

    // ── bitwise — operates on the raw bit pattern of the floats ────
    [[nodiscard]] auto operator&(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_and_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return as<u32x4>().operator&(b.as<u32x4>()).as<f32x4>();  // forward decl needed
#else
        f32x4 r{};
        for(upt i=0;i<4;++i) { u32 a,bv;
            __builtin_memcpy(&a,&reg[i],4); __builtin_memcpy(&bv,&b.reg[i],4);
            u32 res=a&bv; __builtin_memcpy(&r.reg[i],&res,4);
        } return r;
#endif
    }
    [[nodiscard]] auto operator|(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_or_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return as<u32x4>().operator|(b.as<u32x4>()).as<f32x4>();
#else
        f32x4 r{};
        for(upt i=0;i<4;++i) { u32 a,bv;
            __builtin_memcpy(&a,&reg[i],4); __builtin_memcpy(&bv,&b.reg[i],4);
            u32 res=a|bv; __builtin_memcpy(&r.reg[i],&res,4);
        } return r;
#endif
    }
    [[nodiscard]] auto operator^(f32x4 b) const noexcept -> f32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_xor_ps(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return as<u32x4>().operator^(b.as<u32x4>()).as<f32x4>();
#else
        f32x4 r{};
        for(upt i=0;i<4;++i) { u32 a,bv;
            __builtin_memcpy(&a,&reg[i],4); __builtin_memcpy(&bv,&b.reg[i],4);
            u32 res=a^bv; __builtin_memcpy(&r.reg[i],&res,4);
        } return r;
#endif
    }
};

// ─────────────────────────────────────────────────────────────────
// u32x4 — 4 × u32, 128-bit
// ─────────────────────────────────────────────────────────────────
struct u32x4 {
    static constexpr upt  lanes = 4;
    using elem_type = u32;
    RAWR_VEC_STORAGE_u32x4;
    u32x4() noexcept = default;

    [[nodiscard]] static auto broadcast(u32 v) noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_set1_epi32((int)v)};
#elif RAWR_SIMD_ARM
        return {vdupq_n_u32(v)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=v; return r;
#endif
    }
    [[nodiscard]] static auto load(u32 const* p) noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_loadu_si128(((__m128i const*)p))};
#elif RAWR_SIMD_ARM
        return {vld1q_u32(p)};
#else
        u32x4 r{}; __builtin_memcpy(r.reg, p, 16); return r;
#endif
    }
    [[nodiscard]] static auto load_aligned(u32 const* p) noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_load_si128((__m128i const*)p)};
#elif RAWR_SIMD_ARM
        return {vld1q_u32(p)};
#else
        return load(p);
#endif
    }
    void store(u32* p) const noexcept {
#if   RAWR_SIMD_X86_128
        _mm_storeu_si128((__m128i*)p, reg);
#elif RAWR_SIMD_ARM
        vst1q_u32(p, reg);
#else
        __builtin_memcpy(p, reg, 16);
#endif
    }
    void store_aligned(u32* p) const noexcept {
#if   RAWR_SIMD_X86_128
        _mm_store_si128((__m128i*)p, reg);
#elif RAWR_SIMD_ARM
        vst1q_u32(p, reg);
#else
        store(p);
#endif
    }

    RAWR_SIMD_EXTRACT(u32x4, 4, u32)
    RAWR_SIMD_AS(u32x4)

    [[nodiscard]] auto operator+(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_add_epi32(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vaddq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]+b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator-(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_sub_epi32(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vsubq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]-b.reg[i]; return r;
#endif
    }
    // * = low 32 bits of 32x32 product. Requires SSE4.1 on x86.
    // The SSE4.1 gate is in the storage macro chain — on SSE2-only builds,
    // this op has no intrinsic and falls to scalar, which IS bit-identical.
    [[nodiscard]] auto operator*(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_SSE41
        return {_mm_mullo_epi32(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vmulq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]*b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator&(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_and_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vandq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]&b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator|(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_or_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vorrq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]|b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator^(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_xor_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {veorq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]^b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator~() const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        // No direct NOT. XOR with all-ones.
        return {_mm_xor_si128(reg, _mm_cmpeq_epi32(reg, reg))};
#elif RAWR_SIMD_ARM
        return {vmvnq_u32(reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=~reg[i]; return r;
#endif
    }
    [[nodiscard]] auto min(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_SSE41
        return {_mm_min_epu32(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vminq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]<b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto max(u32x4 b) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_SSE41
        return {_mm_max_epu32(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vmaxq_u32(reg, b.reg)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]>b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
    // shift — scalar shift count applies to all lanes
    [[nodiscard]] auto operator<<(u32 n) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_slli_epi32(reg, (int)n)};
#elif RAWR_SIMD_ARM
        return {vshlq_n_u32(reg, (int)n)};  // n must be compile-time on ARM; see below
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]<<n; return r;
#endif
    }
    [[nodiscard]] auto operator>>(u32 n) const noexcept -> u32x4 {
#if   RAWR_SIMD_X86_128
        return {_mm_srli_epi32(reg, (int)n)};  // logical (unsigned) right shift
#elif RAWR_SIMD_ARM
        return {vshrq_n_u32(reg, (int)n)};
#else
        u32x4 r{}; for(upt i=0;i<4;++i) r.reg[i]=reg[i]>>n; return r;
#endif
    }
};

// ─────────────────────────────────────────────────────────────────
// u8x16 — 16 × u8, 128-bit
// Multiply omitted: no 8-bit lane-wise multiply in SSE2.
// Saturating add/sub available but not shown (named variants only).
// ─────────────────────────────────────────────────────────────────
struct u8x16 {
    static constexpr upt  lanes = 16;
    using elem_type = u8;
    RAWR_VEC_STORAGE_u8x16;
    u8x16() noexcept = default;

    [[nodiscard]] static auto broadcast(u8 v) noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_set1_epi8((char)v)};
#elif RAWR_SIMD_ARM
        return {vdupq_n_u8(v)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=v; return r;
#endif
    }
    [[nodiscard]] static auto load(u8 const* p) noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_loadu_si128((__m128i const*)p)};
#elif RAWR_SIMD_ARM
        return {vld1q_u8(p)};
#else
        u8x16 r{}; __builtin_memcpy(r.reg, p, 16); return r;
#endif
    }
    [[nodiscard]] static auto load_aligned(u8 const* p) noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_load_si128((__m128i const*)p)};
#elif RAWR_SIMD_ARM
        return {vld1q_u8(p)};
#else
        return load(p);
#endif
    }
    void store(u8* p) const noexcept {
#if   RAWR_SIMD_X86_128
        _mm_storeu_si128((__m128i*)p, reg);
#elif RAWR_SIMD_ARM
        vst1q_u8(p, reg);
#else
        __builtin_memcpy(p, reg, 16);
#endif
    }

    RAWR_SIMD_EXTRACT(u8x16, 16, u8)
    RAWR_SIMD_AS(u8x16)

    [[nodiscard]] auto operator+(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_add_epi8(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vaddq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=(u8)(reg[i]+b.reg[i]); return r;
#endif
    }
    [[nodiscard]] auto operator-(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_sub_epi8(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vsubq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=(u8)(reg[i]-b.reg[i]); return r;
#endif
    }
    [[nodiscard]] auto operator&(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_and_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vandq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=reg[i]&b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator|(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_or_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vorrq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=reg[i]|b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator^(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_xor_si128(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {veorq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=reg[i]^b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator~() const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_xor_si128(reg, _mm_cmpeq_epi8(reg, reg))};
#elif RAWR_SIMD_ARM
        return {vmvnq_u8(reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=~reg[i]; return r;
#endif
    }
    [[nodiscard]] auto min(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_min_epu8(reg, b.reg)};  // SSE2: unsigned byte min exists
#elif RAWR_SIMD_ARM
        return {vminq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=reg[i]<b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto max(u8x16 b) const noexcept -> u8x16 {
#if   RAWR_SIMD_X86_128
        return {_mm_max_epu8(reg, b.reg)};
#elif RAWR_SIMD_ARM
        return {vmaxq_u8(reg, b.reg)};
#else
        u8x16 r{}; for(upt i=0;i<16;++i) r.reg[i]=reg[i]>b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
};

// ─────────────────────────────────────────────────────────────────
// f32x8 — 8 × f32, 256-bit (AVX)
// On non-AVX builds the storage falls to f32 reg[8] (scalar).
// ─────────────────────────────────────────────────────────────────
struct f32x8 {
    static constexpr upt  lanes = 8;
    using elem_type = f32;
    RAWR_VEC_STORAGE_f32x8;
    f32x8() noexcept = default;

    [[nodiscard]] static auto broadcast(f32 v) noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_set1_ps(v)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=v; return r;
#endif
    }
    [[nodiscard]] static auto load(f32 const* p) noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_loadu_ps(p)};
#else
        f32x8 r{}; __builtin_memcpy(r.reg, p, 32); return r;
#endif
    }
    [[nodiscard]] static auto load_aligned(f32 const* p) noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_load_ps(p)};  // requires 32-byte alignment
#else
        return load(p);
#endif
    }
    void store(f32* p) const noexcept {
#if   RAWR_SIMD_X86_256
        _mm256_storeu_ps(p, reg);
#else
        __builtin_memcpy(p, reg, 32);
#endif
    }
    void store_aligned(f32* p) const noexcept {
#if   RAWR_SIMD_X86_256
        _mm256_store_ps(p, reg);
#else
        store(p);
#endif
    }

    RAWR_SIMD_EXTRACT(f32x8, 8, f32)
    RAWR_SIMD_AS(f32x8)

    [[nodiscard]] auto operator+(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_add_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]+b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator-(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_sub_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]-b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator*(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_mul_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]*b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto operator/(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_div_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]/b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto min(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_min_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]<b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
    [[nodiscard]] auto max(f32x8 b) const noexcept -> f32x8 {
#if   RAWR_SIMD_X86_256
        return {_mm256_max_ps(reg, b.reg)};
#else
        f32x8 r{}; for(upt i=0;i<8;++i) r.reg[i]=reg[i]>b.reg[i]?reg[i]:b.reg[i]; return r;
#endif
    }
#if RAWR_ARCH_X86_FEATURE_FMA
    [[nodiscard]] auto fma(f32x8 b, f32x8 c) const noexcept -> f32x8 {
        return {_mm256_fmadd_ps(reg, b.reg, c.reg)};
    }
#endif
};

// Remaining types — s32x4, s8x16, u16x8, s16x8, u64x2, s64x2,
// u32x8, s32x8, u8x32, s8x32, u16x16, s16x16, u64x4, s64x4, f64x2, f64x4
// follow the exact same pattern as the types above.
// Key differences per type:
//   - Storage macro (already defined above)
//   - Intrinsic names (epi8/epi16/epi32/epi64, ps/pd, etc.)
//   - Which ops exist: u64 has no SIMD multiply; u8/s8 have no mul
//   - Signed vs unsigned shift/min/max variants (_epi32 vs _epu32 etc.)
//   - ARM: int vs uint NEON type names

// ── cross-type deletion ───────────────────────────────────────────
// Arithmetic between different element types is always a mistake.
// Deleting the overloads makes this a compile error with the type
// names visible, rather than a silent implicit conversion.
// Same-width types with different element interpretation cannot add.
#define RAWR_DELETE_CROSS(A, B)                                         \
    auto operator+(A, B) noexcept -> A = delete;                       \
    auto operator-(A, B) noexcept -> A = delete;                       \
    auto operator*(A, B) noexcept -> A = delete;
RAWR_DELETE_CROSS(f32x4, u32x4)  RAWR_DELETE_CROSS(u32x4, f32x4)
RAWR_DELETE_CROSS(f32x4, u8x16)  RAWR_DELETE_CROSS(u8x16, f32x4)
RAWR_DELETE_CROSS(u32x4, u8x16)  RAWR_DELETE_CROSS(u8x16, u32x4)
RAWR_DELETE_CROSS(f32x8, u32x4)  RAWR_DELETE_CROSS(u32x4, f32x8)
// Extend for all pairs that share a register width.
#undef RAWR_DELETE_CROSS

// Different widths: no operator defined at all — lookup fails with
// "no match for operator" which is diagnostic enough.

// ── backend selector cleanup ──────────────────────────────────────
#undef RAWR_SIMD_X86_128
#undef RAWR_SIMD_X86_256
#undef RAWR_SIMD_X86_256I
#undef RAWR_SIMD_X86_SSE41
#undef RAWR_SIMD_ARM

} // namespace rawr::simd
#pragma endregion


// ── smoke test ────────────────────────────────────────────────────
#include <cstdio>
int main() {
    using namespace rawr;
    using namespace rawr::cxx::intrin;
    using namespace rawr::simd;

    // Compile-time ops
    static_assert(byteswap(u32(0x12345678)) == u32(0x78563412));
    static_assert(byteswap(u16(0xAABB))     == u16(0xBBAA));
    static_assert(byteswap(u8(0xFF))        == u8(0xFF));
    static_assert([]() constexpr { return is_constant_evaluated(); }());

    // Support matrix
    puts("support matrix:");
    for (auto& r : this_support_matrix)
        printf("  %-14s %s\n", r.op, r.ok ? "yes" : "no");

    // SIMD round-trip: broadcast, add, store, read back
    alignas(16) f32 buf[4] = {};
    auto a = f32x4::broadcast(1.0f);
    auto b = f32x4::broadcast(2.0f);
    auto c = a + b;
    c.store(buf);
    printf("f32x4: 1+2 = %.1f (expected 3.0)\n", (double)buf[0]);

    // Integer vector
    alignas(16) u32 ibuf[4] = {};
    auto ia = u32x4::broadcast(10u);
    auto ib = u32x4::broadcast(32u);
    auto ic = ia + ib;
    ic.store(ibuf);
    printf("u32x4: 10+32 = %u (expected 42)\n", ibuf[0]);

    // as<> reinterpret
    auto fu = c.as<u32x4>();
    fu.store(ibuf);
    printf("f32x4(3.0f).as<u32x4>()[0] = 0x%08X (expected 0x40400000)\n", ibuf[0]);

    // extract
    printf("a.extract<2>() = %.1f (expected 1.0)\n", (double)a.extract<2>());

    return 0;
}
