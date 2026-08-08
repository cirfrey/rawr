//// rawr/lib/detection.hpp
// Flat constexpr values in rawr:: for if constexpr dispatch.
// Enum type names are plural nouns to avoid clashing with module namespaces
// (rawr::abi is a namespace; rawr::abis is this enum type).
#pragma region rawr/lib/detection.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection_macros.hpp"
    #include "rawr/lib/integer.hpp"
    #include "rawr/data/rich_enum.hpp"
#endif

namespace rawr::inline lib::inline detection
{
    RAWR_RICH_ENUM(compilers, uint_exact<1>, (unknown, gcc, clang, msvc), ());
    constexpr compilers this_compiler =
        RAWR_COMPILER_CLANG ? compilers::clang :
        RAWR_COMPILER_GCC   ? compilers::gcc :
        RAWR_COMPILER_MSVC  ? compilers::msvc :
        compilers::unknown;
    struct compiler_version { uint_exact<2> major, minor, patch; };
    constexpr compiler_version this_compiler_version = {
        RAWR_COMPILER_VERSION_MAJOR,
        RAWR_COMPILER_VERSION_MINOR,
        RAWR_COMPILER_VERSION_PATCH
    };


    RAWR_RICH_ENUM(abis, uint_exact<1>, (
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


    RAWR_RICH_ENUM(cxx_abis, uint_exact<1>, (unknown, itanium, msvc), ());
    constexpr cxx_abis this_cxx_abi =
        RAWR_CXX_ABI_ITANIUM ? cxx_abis::itanium :
        RAWR_CXX_ABI_MSVC    ? cxx_abis::msvc :
        cxx_abis::unknown;


    RAWR_RICH_ENUM(bins, uint_exact<1>, (unknown, elf, macho, pe, wasm), ());
    constexpr bins this_bin =
        RAWR_BIN_ELF   ? bins::elf :
        RAWR_BIN_MACHO ? bins::macho :
        RAWR_BIN_PE    ? bins::pe :
        RAWR_BIN_WASM  ? bins::wasm :
        bins::unknown;


    RAWR_RICH_ENUM(platforms,    uint_exact<1>, (
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


    RAWR_RICH_ENUM(archs,        uint_exact<1>, (
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
    RAWR_RICH_FLAGS(x86_family_features, uint_exact<2>, (
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
        .set_if(x86_family_features::sse,     RAWR_ARCH_X86_SSE)
        .set_if(x86_family_features::sse2,    RAWR_ARCH_X86_SSE2)
        .set_if(x86_family_features::sse41,   RAWR_ARCH_X86_SSE41)
        .set_if(x86_family_features::sse42,   RAWR_ARCH_X86_SSE42)
        .set_if(x86_family_features::avx,     RAWR_ARCH_X86_AVX)
        .set_if(x86_family_features::avx2,    RAWR_ARCH_X86_AVX2)
        .set_if(x86_family_features::avx512f, RAWR_ARCH_X86_AVX512F)
        .set_if(x86_family_features::fma,     RAWR_ARCH_X86_FMA)
        .set_if(x86_family_features::bmi1,    RAWR_ARCH_X86_BMI1)
        .set_if(x86_family_features::bmi2,    RAWR_ARCH_X86_BMI2)
        .set_if(x86_family_features::popcnt,  RAWR_ARCH_X86_POPCNT)
        .set_if(x86_family_features::lzcnt,   RAWR_ARCH_X86_LZCNT)
        .set_if(x86_family_features::clwb,    RAWR_ARCH_X86_CLWB);
    RAWR_RICH_FLAGS(arm_family_features, uint_exact<1>, (
        (neon,      1 << 0),
        (sve,       1 << 1),
        (sve2,      1 << 2),
        (dotprod,   1 << 3),
        (fp16,      1 << 4),
        (bf16,      1 << 5)
    ), ());
    constexpr arm_family_features this_arm_features = arm_family_features{}
        .set_if(arm_family_features::neon,    RAWR_ARCH_ARM_NEON)
        .set_if(arm_family_features::sve,     RAWR_ARCH_ARM_SVE)
        .set_if(arm_family_features::sve2,    RAWR_ARCH_ARM_SVE2)
        .set_if(arm_family_features::dotprod, RAWR_ARCH_ARM_DOTPROD)
        .set_if(arm_family_features::fp16,    RAWR_ARCH_ARM_FP16)
        .set_if(arm_family_features::bf16,    RAWR_ARCH_ARM_BF16);


    RAWR_RICH_FLAGS(sanitizers,  uint_exact<2>, (
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
        .set_if(sanitizers::asan,      RAWR_SAN_ASAN)
        .set_if(sanitizers::hwasan,    RAWR_SAN_HWASAN)
        .set_if(sanitizers::tsan,      RAWR_SAN_TSAN)
        .set_if(sanitizers::msan,      RAWR_SAN_MSAN)
        .set_if(sanitizers::lsan,      RAWR_SAN_LSAN)
        .set_if(sanitizers::ubsan,     RAWR_SAN_UBSAN)
        .set_if(sanitizers::cfi,       RAWR_SAN_CFI)
        .set_if(sanitizers::safestack, RAWR_SAN_SAFESTACK)
        .set_if(sanitizers::rtc,       RAWR_SAN_RTC);


    RAWR_RICH_ENUM(endians, uint_exact<1>, (unknown, little, big), ());
    constexpr endians this_endian =
        RAWR_ENDIAN_LITTLE ? endians::little :
        RAWR_ENDIAN_BIG    ? endians::big :
        endians::unknown;


    RAWR_RICH_ENUM(cxx_versions, uint_exact<1>, (unknown, cpp98, cpp11, cpp14, cpp17, cpp20, cpp23, cpp26), ());
    constexpr cxx_versions this_cxx_version =
        RAWR_CPP >= RAWR_CPP26 ? cxx_versions::cpp26 :
        RAWR_CPP >= RAWR_CPP23 ? cxx_versions::cpp23 :
        RAWR_CPP >= RAWR_CPP20 ? cxx_versions::cpp20 :
        RAWR_CPP >= RAWR_CPP17 ? cxx_versions::cpp17 :
        RAWR_CPP >= RAWR_CPP14 ? cxx_versions::cpp14 :
        RAWR_CPP >= RAWR_CPP11 ? cxx_versions::cpp11 :
        cxx_versions::cpp98;

    constexpr bool this_is_posix       = RAWR_IS_POSIX;
    constexpr bool this_is_64bit       = RAWR_IS_64BIT;
    constexpr bool this_is_32bit       = RAWR_IS_32BIT;
    constexpr bool this_has_exceptions = RAWR_HAS_EXCEPTIONS;
    constexpr bool this_has_rtti       = RAWR_HAS_RTTI;
}

#pragma endregion rawr/lib/detection.hpp
