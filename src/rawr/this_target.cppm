// Flat constexpr values in rawr:: for if constexpr dispatch.
// Enum type names are plural nouns to avoid clashing with module namespaces
// (rawr::abi is a namespace; rawr::abis is this enum type).
export module rawr.this_target;
import        rawr.lib.integer.raw;
#include     "rawr/abi/cc/detection.pp"
#include     "rawr/abi/dm/detection.pp"
#include     "rawr/arch/detection.pp"
#include     "rawr/arch/endian.pp"
#include     "rawr/arch/arm/detection.pp"
#include     "rawr/arch/riscv/detection.pp"
#include     "rawr/arch/wasm/detection.pp"
#include     "rawr/arch/x86/detection.pp"
#include     "rawr/bin/detection.pp"
#include     "rawr/compiler/detection.pp"
#include     "rawr/cxx/detection.pp"
#include     "rawr/cxx/version.pp"
#include     "rawr/cxx/feature.pp"
#include     "rawr/env/detection.pp"
#include     "rawr/platform/detection.pp"
#include     "rawr/san/detection.pp"
#include     "rawr/lib/integer/detection.pp"
#include     "rawr/lib/rich_enum.pp"

export namespace rawr::inline this_target
{
    RAWR_RICH_ENUM(abi_ccs, ru8, (
        (unknown,       RAWR_ABI_CCID_UNKNOWN),
        (sysv,          RAWR_ABI_CCID_SYSV),
        (win64,         RAWR_ABI_CCID_WIN64),
        (aapcs64,       RAWR_ABI_CCID_AAPCS64),
        (aapcs64_apple, RAWR_ABI_CCID_AAPCS64_APPLE),
        (aapcs64_win,   RAWR_ABI_CCID_AAPCS64_WIN),
        (arm64ec,       RAWR_ABI_CCID_ARM64EC),
        (aapcs32,       RAWR_ABI_CCID_AAPCS32),
        (riscv,         RAWR_ABI_CCID_RISCV),
        (xtensa,        RAWR_ABI_CCID_XTENSA),
        (avr,           RAWR_ABI_CCID_AVR),
        (wasm,          RAWR_ABI_CCID_WASM)
    ), ());
    constexpr abi_ccs this_abi_cc   = abi_ccs::from_underlying_unchecked(RAWR_ABI_CCID);
    RAWR_RICH_ENUM(abi_dms, ru8, (
        (unknown, RAWR_ABI_DMID_UNKNOWN),
        (ilp32,   RAWR_ABI_DMID_ILP32),
        (lp64,    RAWR_ABI_DMID_LP64),
        (llp64,   RAWR_ABI_DMID_LLP64),
        (ilp64,   RAWR_ABI_DMID_ILP64),
        (lp32,    RAWR_ABI_DMID_LP32),
        (ip16,    RAWR_ABI_DMID_IP16)
    ), (
        [[nodiscard]] constexpr auto is_ptr64() const noexcept { return to_enum_type() == enum_type::lp64  || to_enum_type() == enum_type::llp64 || to_enum_type() == enum_type::ilp64; }
        [[nodiscard]] constexpr auto is_ptr32() const noexcept { return to_enum_type() == enum_type::ilp32 || to_enum_type() == enum_type::lp32; }
    ));
    constexpr abi_dms this_abi_dm  = abi_dms::from_underlying_unchecked(RAWR_ABI_DMID);


    RAWR_RICH_ENUM(archs, ru8, (
        (unknown, RAWR_ARCHID_UNKNOWN),
        (ia32,    RAWR_ARCHID_IA32),
        (x64,     RAWR_ARCHID_X64),
        (arm32,   RAWR_ARCHID_ARM32),
        (arm64,   RAWR_ARCHID_ARM64),
        (riscv32, RAWR_ARCHID_RISCV32),
        (riscv64, RAWR_ARCHID_RISCV64),
        (xtensa,  RAWR_ARCHID_XTENSA),
        (avr,     RAWR_ARCHID_AVR),
        (wasm32,  RAWR_ARCHID_WASM32),
        (wasm64,  RAWR_ARCHID_WASM64),
        (ppc32,   RAWR_ARCHID_PPC32),
        (ppc64,   RAWR_ARCHID_PPC64),
        (loong64, RAWR_ARCHID_LOONG64),
        (mips32,  RAWR_ARCHID_MIPS32),
        (mips64,  RAWR_ARCHID_MIPS64),
        (s390,    RAWR_ARCHID_S390),
        (s390x,   RAWR_ARCHID_S390X),
        (msp430,  RAWR_ARCHID_MSP430),
        (sparc32, RAWR_ARCHID_SPARC32),
        (sparc64, RAWR_ARCHID_SPARC64),
        (superh,  RAWR_ARCHID_SUPERH)
    ), (
        [[nodiscard]] constexpr auto is_x86()   const noexcept { return to_enum_type() == enum_type::ia32    || to_enum_type() == enum_type::x64; }
        [[nodiscard]] constexpr auto is_riscv() const noexcept { return to_enum_type() == enum_type::riscv32 || to_enum_type() == enum_type::riscv64; }
        [[nodiscard]] constexpr auto is_wasm()  const noexcept { return to_enum_type() == enum_type::wasm32  || to_enum_type() == enum_type::wasm64; }
        [[nodiscard]] constexpr auto is_arm()   const noexcept { return to_enum_type() == enum_type::arm32   || to_enum_type() == enum_type::arm64; }
        [[nodiscard]] constexpr auto is_ppc()   const noexcept { return to_enum_type() == enum_type::ppc32   || to_enum_type() == enum_type::ppc64; }
        [[nodiscard]] constexpr auto is_mips()  const noexcept { return to_enum_type() == enum_type::mips32  || to_enum_type() == enum_type::mips64; }
        [[nodiscard]] constexpr auto is_ibmz()  const noexcept { return to_enum_type() == enum_type::s390    || to_enum_type() == enum_type::s390x; }
        [[nodiscard]] constexpr auto is_sparc() const noexcept { return to_enum_type() == enum_type::sparc32 || to_enum_type() == enum_type::sparc64; }
    ));
    constexpr archs this_arch = archs::from_underlying_unchecked(RAWR_ARCHID);
    RAWR_RICH_ENUM(endians, ru8, (
        (unknown, RAWR_ARCH_ENDIANID_UNKNOWN),
        (le,      RAWR_ARCH_ENDIANID_LE),
        (be,      RAWR_ARCH_ENDIANID_BE)
    ), ());
    constexpr endians this_endian = endians::from_underlying_unchecked(RAWR_ARCH_ENDIANID);
    RAWR_RICH_FLAGS(arm_family_features, ru8, (
        (neon,       RAWR_ARCH_ARMFEATUREID_NEON),
        (sve,        RAWR_ARCH_ARMFEATUREID_SVE),
        (sve2,       RAWR_ARCH_ARMFEATUREID_SVE2),
        (dotprod,    RAWR_ARCH_ARMFEATUREID_DOTPROD),
        (fp16,       RAWR_ARCH_ARMFEATUREID_FP16),
        (bf16,       RAWR_ARCH_ARMFEATUREID_BF16),
        (atomic_cas, RAWR_ARCH_ARMFEATUREID_ATOMIC_CAS)
    ), ());
    constexpr arm_family_features this_arm_features = arm_family_features::from_underlying_unchecked(RAWR_ARCH_ARMFEATURES);
    RAWR_RICH_FLAGS(riscv_family_features, ru8, (
        (atomic, RAWR_ARCH_RISCVFEATUREID_ATOMIC)
    ), ());
    constexpr riscv_family_features this_riscv_features = riscv_family_features::from_underlying_unchecked(RAWR_ARCH_RISCVFEATURES);
    RAWR_RICH_FLAGS(wasm_family_features, ru8, (
        (atomics, RAWR_ARCH_WASMFEATUREID_ATOMICS)
    ), ());
    constexpr wasm_family_features this_wasm_features = wasm_family_features::from_underlying_unchecked(RAWR_ARCH_WASMFEATURES);
    RAWR_RICH_FLAGS(x86_family_features, ru16, (
        (sse,     RAWR_ARCH_X86FEATUREID_SSE),
        (sse2,    RAWR_ARCH_X86FEATUREID_SSE2),
        (sse41,   RAWR_ARCH_X86FEATUREID_SSE41),
        (sse42,   RAWR_ARCH_X86FEATUREID_SSE42),
        (avx,     RAWR_ARCH_X86FEATUREID_AVX),
        (avx2,    RAWR_ARCH_X86FEATUREID_AVX2),
        (avx512f, RAWR_ARCH_X86FEATUREID_AVX512F),
        (fma,     RAWR_ARCH_X86FEATUREID_FMA),
        (bmi1,    RAWR_ARCH_X86FEATUREID_BMI1),
        (bmi2,    RAWR_ARCH_X86FEATUREID_BMI2),
        (popcnt,  RAWR_ARCH_X86FEATUREID_POPCNT),
        (lzcnt,   RAWR_ARCH_X86FEATUREID_LZCNT),
        (clwb,    RAWR_ARCH_X86FEATUREID_CLWB)
    ), ());
    constexpr x86_family_features this_x86_features = x86_family_features::from_underlying_unchecked(RAWR_ARCH_X86FEATURES);


    // NOLINTBEGIN(readability-identifier-length)
    RAWR_RICH_ENUM(bins, ru8, (
        (unknown, RAWR_BINID_UNKNOWN),
        (elf,     RAWR_BINID_ELF),
        (macho,   RAWR_BINID_MACHO),
        (pe,      RAWR_BINID_PE),
        (wasm,    RAWR_BINID_WASM)
    ), ());
    constexpr bins this_bin = bins::from_underlying_unchecked(RAWR_BINID);
    // NOLINTEND(readability-identifier-length)


    RAWR_RICH_ENUM(compilers, ru8, (
        (unknown, RAWR_COMPILERID_UNKNOWN),
        (gcc,     RAWR_COMPILERID_GCC),
        (clang,   RAWR_COMPILERID_CLANG),
        (msvc,    RAWR_COMPILERID_MSVC)
    ), (
        [[nodiscard]] constexpr auto is_gnu() const noexcept { return to_enum_type() == enum_type::gcc || to_enum_type() == enum_type::clang; }
    ));
    constexpr compilers this_compiler = compilers::from_underlying_unchecked(RAWR_COMPILERID);
    struct compiler_version { ru16 major, minor, patch, build; };
    constexpr compiler_version this_compiler_version = {
        .major = RAWR_COMPILER_VERSION_MAJOR,
        .minor = RAWR_COMPILER_VERSION_MINOR,
        .patch = RAWR_COMPILER_VERSION_PATCH,
        .build = RAWR_COMPILER_VERSION_BUILD
    };


    RAWR_RICH_ENUM(cxx, ru8, (
        (unknown, RAWR_CXXID_UNKNOWN),
        (itanium, RAWR_CXXID_ITANIUM),
        (msvc,    RAWR_CXXID_MSVC)
    ), ());
    constexpr cxx this_cxx = cxx::from_underlying_unchecked(RAWR_CXXID);
    RAWR_RICH_ENUM(cxx_features, ru8, (
        (has_exceptions, RAWR_CXXFEATUREID_HAS_EXCEPTIONS),
        (has_cfi_asm,    RAWR_CXXFEATUREID_HAS_CFI_ASM),
        (has_rtti,       RAWR_CXXFEATUREID_HAS_RTTI),
        (is_hosted,      RAWR_CXXFEATUREID_IS_HOSTED)
    ), ());
    constexpr cxx_features this_cxx_features = cxx_features::from_underlying_unchecked(RAWR_CXXFEATURES);
    RAWR_RICH_ENUM(cxx_versions, ru8, (unknown, cpp98, cpp11, cpp14, cpp17, cpp20, cpp23, cpp26), ());
    constexpr cxx_versions this_cxx_version =
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_26 ? cxx_versions::cpp26 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_23 ? cxx_versions::cpp23 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_20 ? cxx_versions::cpp20 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_17 ? cxx_versions::cpp17 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_14 ? cxx_versions::cpp14 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_11 ? cxx_versions::cpp11 :
                                                  cxx_versions::cpp98;


    RAWR_RICH_ENUM(envs, ru8, (
        (unknown,      RAWR_ENVID_UNKNOWN),
        (cygwin,       RAWR_ENVID_CYGWIN),
        (mingw,        RAWR_ENVID_MINGW),
        (emscripten,   RAWR_ENVID_EMSCRIPTEN),
        (native,       RAWR_ENVID_NATIVE)
    ), ());
    constexpr envs this_env = envs::from_underlying_unchecked(RAWR_ENVID);


    RAWR_RICH_ENUM(platforms, ru8, (
        (unknown,     RAWR_PLATFORMID_UNKNOWN),
        (linux,       RAWR_PLATFORMID_LINUX),
        (windows,     RAWR_PLATFORMID_WINDOWS),
        (freebsd,     RAWR_PLATFORMID_FREEBSD),
        (openbsd,     RAWR_PLATFORMID_OPENBSD),
        (netbsd,      RAWR_PLATFORMID_NETBSD),
        (macos,       RAWR_PLATFORMID_MACOS),
        (ios,         RAWR_PLATFORMID_IOS),
        (android,     RAWR_PLATFORMID_ANDROID),
        (wasm,        RAWR_PLATFORMID_WASM),
        (esp32,       RAWR_PLATFORMID_ESP32),
        (esp8266,     RAWR_PLATFORMID_ESP8266),
        (stm32,       RAWR_PLATFORMID_STM32),
        (nordic,      RAWR_PLATFORMID_NORDIC),
        (pico,        RAWR_PLATFORMID_PICO),
        (teensy,      RAWR_PLATFORMID_TEENSY),
        (avr,         RAWR_PLATFORMID_AVR)
    ), ());
    constexpr platforms this_platform = platforms::from_underlying_unchecked(RAWR_PLATFORMID);

    RAWR_RICH_FLAGS(sanitizers,  ru16, (
        (asan,      RAWR_SANFEATUREID_ASAN),
        (hwasan,    RAWR_SANFEATUREID_HWASAN),
        (tsan,      RAWR_SANFEATUREID_TSAN),
        (msan,      RAWR_SANFEATUREID_MSAN),
        (lsan,      RAWR_SANFEATUREID_LSAN),
        (ubsan,     RAWR_SANFEATUREID_UBSAN),
        (cfi,       RAWR_SANFEATUREID_CFI),
        (safestack, RAWR_SANFEATUREID_SAFESTACK),
        (rtc,       RAWR_SANFEATUREID_RTC)
    ), ());
    constexpr sanitizers this_sanitizers = sanitizers::from_underlying_unchecked(RAWR_SANFEATURES);

    constexpr bool this_has_int128 = RAWR_INTEGER_HAS_INT128;
}
