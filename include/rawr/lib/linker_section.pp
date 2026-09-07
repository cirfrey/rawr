#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/linker_section.pp"
#endif
// TODO: Update comments with new API (section_name, tag_name).

//
// Decentralised typed linker-section registry.
//
// API:
//   RAWR_LINKER_SECTION_DEFINE(section, T)      — declare a section registry (namespace scope)
//   RAWR_LINKER_SECTION_REGISTER(section, ...)  — register one T entry; args brace-init T
//   for (auto& e : section) { ... }             — iterate at runtime
//
// If you define a section, its best practice to also define an accompanying macro
// for registering into that section. such as:
//     namespace rawr::inline lib::test
//     {
//         struct section_entry { ... };
//         RAWR_LINKER_SECTION_DEFINE(section, section_entry)
//         #define RAWR_REGISTER_TEST(Test) \
//             RAWR_LINKER_SECTION_REGISTER(::rawr::lib::test::section, Test)
//     }
//
// Its also best practice to define a section in a namespace and not in the global
// or file scope.
//
// -- Entry ordering ------------------------------------------------------------
// Entries appear in link order — the order .o files are passed to the linker.
// This is a de-facto property of all major linkers, not a standard guarantee.
// Do not build ordering-dependent logic on it silently. If a guaranteed order
// is required, include an index field in T and sort in the runner.
//
// -- Section name constraint — all platforms -----------------------------------
// `tag` must be a valid C identifier (a-z A-Z 0-9 _). No dots, slashes, or $
// characters. On ELF the __start_/__stop_ auto-symbols are derived from the
// section name and must satisfy C identifier rules. Validated at compile time.
//
// -- Section name constraint — Mach-O additional -------------------------------
// Mach-O section names are stored in a 16-byte null-terminated field.
// Names longer than 16 characters produce silent truncation or a linker error
// depending on ld64 version. A separate static_assert enforces this limit.
//
// -- LTO ----------------------------------------------------------------------
// RAWR_ATTRIBUTE(used) prevents DCE at the object-file level but not across
// link-time optimisation. With LTO, entries may be silently eliminated and the
// section appears empty. Build consuming executables with -fno-lto, or add
// KEEP(*(.section_name)) to your linker script.
//
// -- Empty sections — ELF -----------------------------------------------------
// ld.bfd, lld, mold: emit __start_X / __stop_X for any section referenced by
// an extern declaration, even with no REGISTER calls.
// gold: may omit these symbols when no entries exist. The externs are declared
// RAWR_ATTRIBUTE(weak) so that absent symbols resolve to null: start() ==
// stop() == nullptr and the loop body never executes. No crash.
//
// -- Empty sections — Mach-O --------------------------------------------------
// ld64 does not emit section$start / section$end for sections with no entries.
// RAWR_ATTRIBUTE(weak) on Darwin maps to Mach-O weak_import: absent symbols
// resolve to null. start() == stop() == nullptr; loop body never executes.
//
// -- Empty sections — PE -------------------------------------------------------
// Explicit $A / $Z sentinel objects bound the section. start() and stop() are
// derived from sentinel addresses and are always valid regardless of whether
// any REGISTER calls have been made.
//
// -- Bare-metal ELF -----------------------------------------------------------
// __start_/__stop_ auto-symbols require a hosted linker with its default
// internal script. On bare-metal with a custom linker script, declare them:
//
//   .my_section : {
//       __start_my_section = .;
//       KEEP(*(.my_section))
//       __stop_my_section  = .;
//   }
//
// If the script omits them, the weak externs resolve to null and the section
// appears empty rather than crashing.
//
// -- Supported targets ---------------------------------------------------------
//   ELF    ld.bfd, lld, gold, mold — Linux, embedded ARM / RISC-V / Xtensa
//   Mach-O ld64, lld/MachO        — macOS, iOS
//   PE     MSVC link, lld-link    — Windows (MSVC and MinGW / Clang)
//   WASM                          — not supported; no equivalent mechanism
#pragma once

#include "rawr/lib/detection.pp"
#include "rawr/lib/attributes.pp"

#define RAWR_LS_CONCAT_(a_, b_) a_##b_
#define RAWR_LS_CONCAT(a_, b_)  RAWR_LS_CONCAT_(a_, b_)
#define RAWR_LS_STR_DETAIL_(x) #x
#define RAWR_LS_STR_(x) RAWR_LS_STR_DETAIL_(x)

// The #tag_name_ stringification embeds the invalid name in the diagnostic.
#define RAWR_LS_DETAIL_VALIDATE_NAME_(section_name)                    \
    static_assert(                                                     \
        []() constexpr noexcept -> bool {                              \
            const char* s_ = #section_name;                            \
            for (; *s_; ++s_)                                          \
                if (!((*s_ >= 'a' && *s_ <= 'z') ||                    \
                      (*s_ >= 'A' && *s_ <= 'Z') ||                    \
                      (*s_ >= '0' && *s_ <= '9') ||                    \
                       *s_ == '_'))                                    \
                    return false;                                      \
            return true;                                               \
        }(),                                                           \
        "rawr::linker_section: '" #section_name "'"                    \
        " — section name must be a plain C identifier (a-z A-Z 0-9 _)" \
    )

// ============================================================================
// PE / MSVC
// ============================================================================
#if RAWR_BIN_PE && RAWR_COMPILER_MSVC

// Sections must be declared before allocating into them.
    // RAWR_PRAGMA expands to __pragma on MSVC, avoiding stringification issues.
    // start() skips the $A sentinel by advancing one sentinel-sized step.
    // stop() is the address of the $Z sentinel.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)                           \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                                         \
        RAWR_PRAGMA(section(RAWR_LS_STR_(section_name##$A), read))                           \
        RAWR_PRAGMA(section(RAWR_LS_STR_(section_name##$I), read))                           \
        RAWR_PRAGMA(section(RAWR_LS_STR_(section_name##$Z), read))                           \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {                                   \
            struct sentinel_t_ { alignas(T_) char _[sizeof(T_)]; };                          \
            RAWR_DECLSPEC(selectany) RAWR_DECLSPEC(allocate(RAWR_LS_STR_(section_name##$A))) \
            extern const sentinel_t_ sent_start_{};                                          \
            RAWR_DECLSPEC(selectany) RAWR_DECLSPEC(allocate(RAWR_LS_STR_(section_name##$Z))) \
            extern const sentinel_t_ sent_stop_{};                                           \
            struct tag_type {                                                                \
                using value_type = T_;                                                       \
                auto begin() const -> const T_* {                                            \
                    return reinterpret_cast<const T_*>(&sent_start_ + 1);                    \
                }                                                                            \
                auto end()   const -> const T_* {                                            \
                    return reinterpret_cast<const T_*>(&sent_stop_);                         \
                }                                                                            \
                auto size()  const -> decltype(end() - begin()) {                            \
                    return end() - begin();                                                  \
                }                                                                            \
                auto empty() const -> bool { return begin() == end(); }                      \
            };                                                                               \
        }                                                                                    \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    // Not #undef'd: expansion-time dependency of RAWR_LINKER_SECTION_REGISTER.
    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_) \
        RAWR_DECLSPEC(allocate(RAWR_LS_STR_(section_name##$I)))    \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_)

// ============================================================================
// PE / GNU — MinGW (GCC or Clang targeting Windows PE)
// ============================================================================
#elif RAWR_BIN_PE

    // RAWR_DECLSPEC is empty on non-MSVC. MinGW sentinels use RAWR_ATTRIBUTE(weak)
    // for COMDAT deduplication — the linker picks one definition across TUs,
    // equivalent to __declspec(selectany) on this toolchain.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)        \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                      \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {                \
            struct sentinel_t_ { alignas(T_) char _[sizeof(T_)]; };       \
            RAWR_ATTRIBUTE(weak)                                          \
            RAWR_ATTRIBUTE(section(#section_name "$A"))                   \
            RAWR_ATTRIBUTE(used)                                          \
            const sentinel_t_ sent_start_{};                              \
            RAWR_ATTRIBUTE(weak)                                          \
            RAWR_ATTRIBUTE(section(#section_name "$Z"))                   \
            RAWR_ATTRIBUTE(used)                                          \
            const sentinel_t_ sent_stop_{};                               \
            struct tag_type {                                             \
                using value_type = T_;                                    \
                auto begin() const -> const T_* {                         \
                    return reinterpret_cast<const T_*>(&sent_start_ + 1); \
                }                                                         \
                auto end()   const -> const T_* {                         \
                    return reinterpret_cast<const T_*>(&sent_stop_);      \
                }                                                         \
                auto size()  const -> decltype(end() - begin()) {         \
                    return end() - begin();                               \
                }                                                         \
                auto empty() const -> bool { return begin() == end(); }   \
            };                                                            \
        }                                                                 \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_) \
        RAWR_ATTRIBUTE(section(#section_name "$I"))                \
        RAWR_ATTRIBUTE(used)                                       \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_)

// ============================================================================
// Mach-O — macOS, iOS (ld64, lld/MachO)
// ============================================================================
#elif RAWR_BIN_MACHO

    // ld64 generates section$start$SEGMENT$section and section$end$SEGMENT$section
    // for non-empty sections. RAWR_ASM binds the C++ extern to those raw
    // linker symbols regardless of which namespace the declaration lives in.
    // RAWR_ATTRIBUTE(weak) maps to Mach-O weak_import: absent symbol → null.
    //
    // Section name limit: Mach-O section names are stored in a 16-byte field.
    // Names longer than 16 characters produce silent truncation or a linker error.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)      \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                    \
        static_assert(                                                  \
            []() constexpr noexcept -> bool {                           \
                const char* s_ = #section_name;                         \
                int n_ = 0;                                             \
                while (*s_++) ++n_;                                     \
                return n_ <= 16;                                        \
            }(),                                                        \
            "rawr::linker_section: '" #section_name "'"                 \
            " — Mach-O section names are limited to 16 characters"      \
        );                                                              \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {              \
            extern const T_ begin_[]                                    \
                RAWR_ATTRIBUTE(weak)                                    \
                RAWR_ASM("section$start$__DATA$" #section_name);        \
            extern const T_ end_[]                                      \
                RAWR_ATTRIBUTE(weak)                                    \
                RAWR_ASM("section$end$__DATA$"   #section_name);        \
            struct tag_type {                                           \
                using value_type = T_;                                  \
                auto begin() const  -> const T_* { return begin_; }     \
                auto end()   const  -> const T_* { return end_;  }      \
                auto size()  const  -> decltype(end() - begin()) {      \
                    return end() - begin();                             \
                }                                                       \
                auto empty() const -> bool { return begin() == end(); } \
            };                                                          \
        }                                                               \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    // Adjacent string literal concat: "__DATA," #tag_name_ → "__DATA,foo".
    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_) \
        RAWR_ATTRIBUTE(section("__DATA," #section_name))           \
        RAWR_ATTRIBUTE(used)                                       \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_)

// ============================================================================
// ELF — Linux, bare-metal (any ELF toolchain)
// ============================================================================
#elif RAWR_BIN_ELF

    // RAWR_ASM binds the C++ name to the raw linker-generated symbol,
    // bypassing name mangling and namespace qualification entirely.
    // The namespace the extern lives in is irrelevant to the linker symbol binding.
    // RAWR_ATTRIBUTE(weak): if the linker omits __start_X or __stop_X (gold +
    // empty section; bare-metal without explicit linker script entries), the symbol
    // resolves to null rather than a link error. start() == stop() == nullptr;
    // the loop body never executes. See bare-metal note in file header.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)                 \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                               \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {                         \
            RAWR_WEAK extern const T_ start_[] RAWR_ASM("__start_" #section_name); \
            RAWR_WEAK extern const T_ stop_[]  RAWR_ASM("__stop_"  #section_name); \
            struct tag_type {                                                      \
                using value_type = T_;                                             \
                auto begin() const -> const T_* { return start_; }                 \
                auto end()   const -> const T_* { return stop_;  }                 \
                auto size()  const  -> decltype(end() - begin()) {                 \
                    return end() - begin();                                        \
                }                                                                  \
                auto empty() const -> bool { return begin() == end(); }            \
            };                                                                     \
        }                                                                          \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    // static: internal linkage prevents ODR conflicts across TUs registering into
    // the same section. RAWR_ATTRIBUTE(used) suppresses object-file-level DCE.
    // See LTO note in file header.
    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_) \
        RAWR_ATTRIBUTE(section(#section_name))                     \
        RAWR_ATTRIBUTE(used)                                       \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_)

// ============================================================================
// Unsupported
// ============================================================================
#elif RAWR_BIN_WASM
    #error "rawr/data/linker_section.pp: WASM has no linker-section equivalent;" \
          " a compile-time registration array is required for this target"
#else
    #error "rawr/data/linker_section.pp: unrecognised binary format"
#endif

// -- RAWR_LINKER_SECTION_REGISTER ---------------------------------------------
// Registers one entry into the section for tag_name_. Must be at namespace
// scope in a scope where `tag_name_` is visible.
//
// Remaining arguments brace-initialise T directly:
//   RAWR_LINKER_SECTION_REGISTER(my_hooks, fn_ptr, "label")
//
// RAWR_LS_DETAIL_REGISTER_ is not #undef'd: it is an expansion-time dependency
// of this macro. Do not call it directly.
//
// Two-level indirection forces __COUNTER__ to expand to its integer value
// before token-pasting. ## suppresses expansion of adjacent tokens, so the
// expansion must happen at the call boundary via argument passing.
#define RAWR_LINKER_SECTION_REGISTER(section_name, tag_name) \
    RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, __COUNTER__)
