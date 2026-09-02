//// rawr/lib/detection.hpp.
// Flat constexpr values in rawr:: for if constexpr dispatch.
// Enum type names are plural nouns to avoid clashing with module namespaces
// (rawr::abi is a namespace; rawr::abis is this enum type).

#ifdef RAWR_MODULE
    export module rawr.lib.detection;
    import rawr.lib.integer.raw;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/raw.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/rich_enum.pp"

RAWR_EXPORT namespace rawr::inline lib::inline detection
{
    RAWR_RICH_ENUM(compilers, ru8, (unknown, gcc, clang, msvc), (
        [[nodiscard]] constexpr auto is_family_gnu() const noexcept
        { return to_enum_type() == enum_type::gcc || to_enum_type() == enum_type::clang; }
    ));
    constexpr compilers this_compiler =
        RAWR_COMPILER_CLANG ? compilers::clang :
        RAWR_COMPILER_GCC   ? compilers::gcc :
        RAWR_COMPILER_MSVC  ? compilers::msvc :
                              compilers::unknown;
    struct compiler_version { ru16 major, minor, patch, build; };
    constexpr compiler_version this_compiler_version = {
        .major = RAWR_COMPILER_VERSION_MAJOR,
        .minor = RAWR_COMPILER_VERSION_MINOR,
        .patch = RAWR_COMPILER_VERSION_PATCH,
        .build = RAWR_COMPILER_VERSION_BUILD
    };


    RAWR_RICH_ENUM(abis, ru8, (
        unknown,
        sysv, win64,
        aapcs64, aapcs64_apple, aapcs32,
        riscv_lp64, riscv_ilp32,
        xtensa, avr, wasm
    ), ());
    constexpr abis this_abi =
        RAWR_ABI_SYSV ?          abis::sysv :
        RAWR_ABI_WIN64 ?         abis::win64 :
        RAWR_ABI_AAPCS64_APPLE ? abis::aapcs64_apple :
        RAWR_ABI_AAPCS64 ?       abis::aapcs64 :
        RAWR_ABI_AAPCS32 ?       abis::aapcs32 :
        RAWR_ABI_RISCV_LP64 ?    abis::riscv_lp64 :
        RAWR_ABI_RISCV_ILP32 ?   abis::riscv_ilp32 :
        RAWR_ABI_XTENSA ?        abis::xtensa :
        RAWR_ABI_AVR ?           abis::avr :
        RAWR_ABI_WASM ?          abis::wasm :
                                 abis::unknown;


    RAWR_RICH_ENUM(cxx_abis, ru8, (unknown, itanium, msvc), ());
    constexpr cxx_abis this_cxx_abi =
        RAWR_CXX_ABI_ITANIUM ? cxx_abis::itanium :
        RAWR_CXX_ABI_MSVC    ? cxx_abis::msvc :
                               cxx_abis::unknown;


    // NOLINTBEGIN(readability-identifier-length)
    RAWR_RICH_ENUM(bins, ru8, (unknown, elf, macho, pe, wasm), ());
    constexpr bins this_bin =
        RAWR_BIN_ELF   ? bins::elf :
        RAWR_BIN_MACHO ? bins::macho :
        RAWR_BIN_PE    ? bins::pe :
        RAWR_BIN_WASM  ? bins::wasm :
                         bins::unknown;
    // NOLINTEND(readability-identifier-length)


    RAWR_RICH_ENUM(platforms,    ru8, (
        unknown,
        linux, windows, macos, ios, android, wasm,
        esp32, esp8266, stm32, nordic, pico, teensy, avr
    ), ());
    constexpr platforms this_platform =
        RAWR_PLATFORM_WASM    ?  platforms::wasm :
        RAWR_PLATFORM_ESP32   ?  platforms::esp32 :
        RAWR_PLATFORM_ESP8266 ?  platforms::esp8266 :
        RAWR_PLATFORM_STM32   ?  platforms::stm32 :
        RAWR_PLATFORM_NORDIC  ?  platforms::nordic :
        RAWR_PLATFORM_PICO    ?  platforms::pico :
        RAWR_PLATFORM_TEENSY  ?  platforms::teensy :
        RAWR_PLATFORM_AVR     ?  platforms::avr :
        RAWR_PLATFORM_MACOS   ?  platforms::macos :
        RAWR_PLATFORM_IOS     ?  platforms::ios :
        RAWR_PLATFORM_ANDROID ?  platforms::android :
        RAWR_PLATFORM_LINUX   ?  platforms::linux :
        RAWR_PLATFORM_WINDOWS ?  platforms::windows :
                                 platforms::unknown;


    RAWR_RICH_ENUM(archs, ru8, (
        unknown,
        x86, x64, arm32, arm64,
        riscv32, riscv64,
        xtensa, avr, wasm32, wasm64
    ), ());
    constexpr archs this_arch =
        RAWR_ARCH_WASM32  ? archs::wasm32 :
        RAWR_ARCH_WASM64  ? archs::wasm64 :
        RAWR_ARCH_X64     ? archs::x64 :
        RAWR_ARCH_X86     ? archs::x86 :
        RAWR_ARCH_ARM64   ? archs::arm64 :
        RAWR_ARCH_ARM32   ? archs::arm32 :
        RAWR_ARCH_RISCV64 ? archs::riscv64 :
        RAWR_ARCH_RISCV32 ? archs::riscv32 :
        RAWR_ARCH_XTENSA  ? archs::xtensa :
        RAWR_ARCH_AVR     ? archs::avr :
                            archs::unknown;
    RAWR_RICH_FLAGS(x86_family_features, ru16, (
        (sse,       1 << 0),
        (sse2,      1 << 1),
        (sse41,     1 << 2),
        (sse42,     1 << 3),
        (avx,       1 << 4),
        (avx2,      1 << 5),
        (avx512f,   1 << 6),
        (fma,       1 << 7),
        (bmi1,      1 << 8),
        (bmi2,      1 << 9),
        (popcnt,    1 << 10),
        (lzcnt,     1 << 11),
        (clwb,      1 << 12)
    ), ());
    constexpr x86_family_features this_x86_features = x86_family_features{}
        .set_if(RAWR_ARCH_X86_SSE,     x86_family_features::sse)
        .set_if(RAWR_ARCH_X86_SSE2,    x86_family_features::sse2)
        .set_if(RAWR_ARCH_X86_SSE41,   x86_family_features::sse41)
        .set_if(RAWR_ARCH_X86_SSE42,   x86_family_features::sse42)
        .set_if(RAWR_ARCH_X86_AVX,     x86_family_features::avx)
        .set_if(RAWR_ARCH_X86_AVX2,    x86_family_features::avx2)
        .set_if(RAWR_ARCH_X86_AVX512F, x86_family_features::avx512f)
        .set_if(RAWR_ARCH_X86_FMA,     x86_family_features::fma)
        .set_if(RAWR_ARCH_X86_BMI1,    x86_family_features::bmi1)
        .set_if(RAWR_ARCH_X86_BMI2,    x86_family_features::bmi2)
        .set_if(RAWR_ARCH_X86_POPCNT,  x86_family_features::popcnt)
        .set_if(RAWR_ARCH_X86_LZCNT,   x86_family_features::lzcnt)
        .set_if(RAWR_ARCH_X86_CLWB,    x86_family_features::clwb);
    RAWR_RICH_FLAGS(arm_family_features, ru8, (
        (neon,       1 << 0),
        (sve,        1 << 1),
        (sve2,       1 << 2),
        (dotprod,    1 << 3),
        (fp16,       1 << 4),
        (bf16,       1 << 5),
        (atomic_cas, 1 << 6)
    ), ());
    constexpr arm_family_features this_arm_features = arm_family_features{}
        .set_if(RAWR_ARCH_ARM_NEON,       arm_family_features::neon)
        .set_if(RAWR_ARCH_ARM_SVE,        arm_family_features::sve)
        .set_if(RAWR_ARCH_ARM_SVE2,       arm_family_features::sve2)
        .set_if(RAWR_ARCH_ARM_DOTPROD,    arm_family_features::dotprod)
        .set_if(RAWR_ARCH_ARM_FP16,       arm_family_features::fp16)
        .set_if(RAWR_ARCH_ARM_BF16,       arm_family_features::bf16)
        .set_if(RAWR_ARCH_ARM_ATOMIC_CAS, arm_family_features::atomic_cas);
    RAWR_RICH_FLAGS(riscv_family_features, ru8, (
        (atomic, 1 << 0)
    ), ());
    constexpr riscv_family_features this_riscv_features = riscv_family_features{}
        .set_if(RAWR_ARCH_RISCV_ATOMIC, riscv_family_features::atomic);
    RAWR_RICH_FLAGS(wasm_family_features, ru8, (
        (atomics, 1 << 0)
    ), ());
    constexpr wasm_family_features this_wasm_features = wasm_family_features{}
        .set_if(RAWR_ARCH_WASM_ATOMICS, wasm_family_features::atomics);


    RAWR_RICH_FLAGS(sanitizers,  ru16, (
        (asan,      1 << 0),
        (hwasan,    1 << 1),
        (tsan,      1 << 2),
        (msan,      1 << 3),
        (lsan,      1 << 4),
        (ubsan,     1 << 5),
        (cfi,       1 << 6),
        (safestack, 1 << 7),
        (rtc,       1 << 8)
    ), ());
    constexpr sanitizers this_sanitizers = sanitizers{}
        .set_if(RAWR_SAN_ASAN,      sanitizers::asan)
        .set_if(RAWR_SAN_HWASAN,    sanitizers::hwasan)
        .set_if(RAWR_SAN_TSAN,      sanitizers::tsan)
        .set_if(RAWR_SAN_MSAN,      sanitizers::msan)
        .set_if(RAWR_SAN_LSAN,      sanitizers::lsan)
        .set_if(RAWR_SAN_UBSAN,     sanitizers::ubsan)
        .set_if(RAWR_SAN_CFI,       sanitizers::cfi)
        .set_if(RAWR_SAN_SAFESTACK, sanitizers::safestack)
        .set_if(RAWR_SAN_RTC,       sanitizers::rtc);


    RAWR_RICH_ENUM(endians, ru8, (unknown, little, big), ());
    constexpr endians this_endian =
        RAWR_ENDIAN_LITTLE ? endians::little :
        RAWR_ENDIAN_BIG    ? endians::big :
                             endians::unknown;


    RAWR_RICH_ENUM(cxx_versions, ru8, (unknown, cpp98, cpp11, cpp14, cpp17, cpp20, cpp23, cpp26), ());
    constexpr cxx_versions this_cxx_version =
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_26 ? cxx_versions::cpp26 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_23 ? cxx_versions::cpp23 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_20 ? cxx_versions::cpp20 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_17 ? cxx_versions::cpp17 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_14 ? cxx_versions::cpp14 :
        RAWR_CXX_VERSION >= RAWR_CXX_VERSION_11 ? cxx_versions::cpp11 :
                                                  cxx_versions::cpp98;

    constexpr bool this_is_posix       = RAWR_IS_POSIX;
    constexpr bool this_is_64bit       = RAWR_IS_64BIT;
    constexpr bool this_is_32bit       = RAWR_IS_32BIT;
    constexpr bool this_has_exceptions = RAWR_HAS_EXCEPTIONS;
    constexpr bool this_has_rtti       = RAWR_HAS_RTTI;
}
