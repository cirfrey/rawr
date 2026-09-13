#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/linker_section.pp"
#endif
// Decentralised typed linker-section registry.
//
// A section is basically a section_name (actual section string) + tag_name (c++ acessor).
// Defining a section will define section_name in the binary and a tag_name acessor for said
// section in the current namespace.
//
// section_name has to be unique, it is best practice to follow c namespacing standards here:
// prefix with the framework and context.
// E.g: Declaring a section for holding tests for rawr would have a section_name "rawr_tests" or similar,
//      *not* just "tests", as that could potentially conflict with any number of other people's section.
//
// Conflicting section names are a huge footgun, follow this rule religiously.
//
// The linker might complain, it might now, it will likely just put everything in one bucket
// and call it a day, with complete disregard for the underlying entry type.
//
// API:
//   RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, entry_type) - declare a section registry (namespace scope)
//   RAWR_LINKER_SECTION_REGISTER(section_name, tag_name, ...)      - register one T entry; args brace-init T
//   for (auto& e : tag_name) { ... }                               - iterate at runtime
//
// NOTE: On MSVC the tag_name iterators are forward iterators due to platform constraints.
//       If you are just using the suggested iteration syntax you can ignore this limitation.
//
// If you define a section, its best practice to also define an accompanying macro
// for registering into that section. such as (pseudocode, not actually what RAWR_TEST does):
//     namespace rawr::inline lib::test
//     {
//         struct section_entry { ... };
//         RAWR_LINKER_SECTION_DEFINE(rawr_lib_test_section, section, section_entry)
//         #define RAWR_REGISTER_TEST(Test) \
//             RAWR_LINKER_SECTION_REGISTER(rawr_lib_test_section, ::rawr::lib::test::section, Test)
//     }
//
// Its also best practice to define a section in a namespace and not in the global
// or file scope.
//
// -- Entry ordering ------------------------------------------------------------
// Entries appear in link order - the order .o files are passed to the linker.
// This is a de-facto property of all major linkers, not a standard guarantee.
// Do not build ordering-dependent logic on it silently. If a guaranteed order
// is required, include an index field in T and sort in the runner.
//
// -- Section name constraint - all platforms -----------------------------------
// `tag` must be a valid C identifier (a-z A-Z 0-9 _). No dots, slashes, or $
// characters. On ELF the __start_/__stop_ auto-symbols are derived from the
// section name and must satisfy C identifier rules. Validated at compile time.
//
// -- Section name constraint - Mach-O additional -------------------------------
// Mach-O section names are stored in a 16-byte null-terminated field.
// Names longer than 16 characters produce silent truncation or a linker error
// depending on ld64 version. A separate static_assert enforces this limit.
//
// -- LTO ----------------------------------------------------------------------
// RAWR_ATTRIBUTE(used) prevents DCE at the object-file level. In modern
// GCC (>=11) and Clang (>=13), pairing it with RAWR_ATTRIBUTE(retain) defeats
// linker-level garbage collection (--gc-sections) and survives LTO (-flto)
// completely. The MSVC path uses the /include linker pragma to achieve the
// same effect under /GL /LTCG.
// If this code is ever backported to older compilers lacking `retain`, build
// with -fno-lto or add KEEP(*(.section_name)) to your linker script.
//
// -- Empty sections - ELF -----------------------------------------------------
// ld.bfd, lld, mold: emit __start_X / __stop_X for any section referenced by
// an extern declaration, even with no REGISTER calls.
// gold: may omit these symbols when no entries exist. The externs are declared
// RAWR_ATTRIBUTE(weak) so that absent symbols resolve to null: start() ==
// stop() == nullptr and the loop body never executes. No crash.
//
// -- Empty sections - Mach-O --------------------------------------------------
// ld64 does not emit section$start / section$end for sections with no entries.
// RAWR_ATTRIBUTE(weak) on Darwin maps to Mach-O weak_import: absent symbols
// resolve to null. start() == stop() == nullptr; loop body never executes.
//
// -- Empty sections - PE -------------------------------------------------------
// Unlike ELF/Mach-O, the PE implementation does not depend on linker-generated
// start/stop symbols. The explicitly allocated $A and $Z sentinel objects are
// themselves the boundaries of the range, so begin()/end() remain valid even
// when no registrations exist.
//
// The $A/$Z contributions also establish the ordering envelope for $I:
// section$A < section$I < section$Z.
//
// The iterator still has to tolerate linker-inserted padding inside $I; see
// the MSVC implementation below.
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
//   ELF    ld.bfd, lld, gold, mold - Linux, embedded ARM / RISC-V / Xtensa
//   Mach-O ld64, lld/MachO        - macOS, iOS
//   PE     MSVC link, lld-link    - Windows (MSVC and MinGW / Clang)
//   WASM                          - not supported; no equivalent mechanism
//
// NOTE: For some more reference on how to properly use this, look at how rawr/lib/test.(h)pp
//       declares a section with RAWR_LINKER_SECTION_DEFINE and how RAWR_TEST registers a
//       test into that section with RAWR_LINKER_SECTION_REGISTER. Note the namespace scoping
//       on declaration/registration and the way the arguments are passed around inside the macros.
#pragma once

// NOTE: This is *not* generic code, it's very explicit on what compiler/bin its selecting, thats why
//       we dont use RAWR_DECLSPEC or RAWR_ATTRIBUTE: We know what compiler is compiling this as a fact,
//       so we use __attribute__ and __declspec directly to save on macro expansions.

#include "rawr/lib/detection.pp"
#include "rawr/lib/dist/pp.pp"
#if RAWR_COMPILER_MSVC && RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.lib.linker_section;
#endif
#if RAWR_COMPILER_MSVC && RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/lib/linker_section.hpp"
#endif

#define RAWR_LS_CONCAT_(a_, b_) a_##b_
#define RAWR_LS_CONCAT(a_, b_)  RAWR_LS_CONCAT_(a_, b_)
#define RAWR_LS_STR_(x) #x
#define RAWR_LS_STR(x) RAWR_LS_STR_(x)

// The #section_name stringification embeds the invalid name in the diagnostic.
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
        " - section name must be a plain C identifier (a-z A-Z 0-9 _)" \
    )

// ============================================================================
// PE / MSVC
// ============================================================================
#if RAWR_BIN_PE && RAWR_COMPILER_MSVC

    // MSVC/COFF does not provide ELF-like __start_/__stop_ symbols for arbitrary
    // user sections. Instead, we construct an ordered range using three COFF
    // dollar-subsections:
    //
    //     section$A   begin sentinel
    //     section$I   registered entries
    //     section$Z   end sentinel
    //
    // The PE linker groups dollar-subsections by the part before '$' and then
    // sorts the suffixes lexicographically. Consequently $A < $I < $Z, giving
    // us stable begin/end anchors regardless of the order in which individual
    // object files and section contributions are encountered.
    //
    // We deliberately use $I rather than the base section name for registrations:
    // it leaves the ordering scheme explicit and gives us a place between the two
    // sentinels for all registered entries.
    //
    // Reference:
    //   Raymond Chen, "Using linker segments and __declspec(allocate(...)) to
    //   arrange data in a specific order"
    //   https://devblogs.microsoft.com/oldnewthing/20181107-00/?p=100155
    //
    //
    // MSVC's linker may insert padding between separate COFF section
    // contributions. That means a section containing:
    //
    //     T, T, T, ...
    //
    // cannot safely be treated as one contiguous C++ array of T objects:
    // pointer arithmetic through the range may encounter linker padding that
    // is not part of any T object and need not be a multiple of sizeof(T).
    //
    // Instead, $I contains pointers to the actual T objects:
    //
    //     T const*, T const*, T const*, ...
    //
    // The iterator operates on those pointer slots and skips null slots,
    // allowing linker-inserted padding to be ignored. This is also the pattern
    // recommended for robust section walking on MSVC/COFF.
    //
    // The actual T objects remain ordinary TU-local objects; only pointers to
    // them are allocated into the linker section.
    //
    // Reference:
    //   Raymond Chen, "Gotchas when using linker sections to arrange data,
    //   part 2"
    //   https://devblogs.microsoft.com/oldnewthing/20181109-00/?p=100165
    //
    //
    // Keeping a section contribution alive is a separate problem from placing it
    // into the section. __declspec(allocate(...)) only specifies where an object
    // is emitted; it does not by itself make an otherwise-unreferenced object
    // survive compiler/linker dead-stripping.
    //
    // Each registration therefore gets a compiler-generated anchor function whose
    // symbol is forced into the final link with:
    //
    //     #pragma comment(linker, "/include:<symbol>")
    //
    // The anchor takes the address of the corresponding pointer in $I. This gives
    // the linker a live reference to the section entry, while the pointer itself
    // keeps the underlying T object alive.
    //
    // The anchor's symbol must be externally linkable because /include operates
    // on linker symbol names. A TU-local anonymous-namespace function cannot be
    // used for this purpose.
    //
    // We therefore encode __FILE__ and __COUNTER__ into a template specialization
    // of linker_anchor. __FILE__ is represented by a private fixed_string NTTP,
    // producing a TU-specific external symbol without requiring build-system
    // cooperation or another globally-generated identifier.
    //
    // __COUNTER__ only needs to be unique within the translation unit here; the
    // file component supplies the cross-TU identity.
    //
    // The resulting chain is:
    //
    //     /include anchor
    //          |
    //          v
    //     linker_anchor<file, counter>::fn()
    //          |
    //          v
    //     rawr_ls_ptr_N
    //          |
    //          v
    //     rawr_ls_item_N
    //
    // This is the MSVC equivalent of the used/retain mechanism used by the
    // ELF and Mach-O implementations.
    //
    //
    // RAWR_LS_STR(...) is needed around section names because
    // section_name##$A / $I / $Z first has to undergo token pasting and then be
    // converted into a string literal for __pragma(section(...)) and
    // __declspec(allocate(...)).
    //
    // A single-level stringification macro would stringify the token-pasted
    // spelling before macro expansion, producing the wrong section name.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)                        \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                                      \
        __pragma(section(RAWR_LS_STR(section_name##$A), read))                            \
        __pragma(section(RAWR_LS_STR(section_name##$I), read))                            \
        __pragma(section(RAWR_LS_STR(section_name##$Z), read))                            \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name)                                  \
        {                                                                                 \
            using value_type_ = T_;                                                       \
            using pointer_type_ = value_type_ const*;                                     \
                                                                                          \
            __declspec(allocate(RAWR_LS_STR(section_name##$A)))                           \
            inline pointer_type_ sent_start_ = nullptr;                                   \
                                                                                          \
            __declspec(allocate(RAWR_LS_STR(section_name##$Z)))                           \
            inline pointer_type_ sent_stop_ = nullptr;                                    \
                                                                                          \
            struct tag_type                                                               \
            {                                                                             \
                using value_type = value_type_;                                           \
                using iterator =                                                          \
                    ::rawr::lib::linker_section::msvc::iterator<value_type const*>;       \
                                                                                          \
                auto begin() const noexcept -> iterator                                   \
                {                                                                         \
                    auto const first = &sent_start_ + 1;                                  \
                    auto const last = &sent_stop_;                                        \
                    iterator it { first, last };                                          \
                    it.skip_nulls();                                                      \
                    return it;                                                            \
                }                                                                         \
                                                                                          \
                auto end() const noexcept -> iterator                                     \
                {                                                                         \
                    auto const last = &sent_stop_;                                        \
                    return iterator { last, last };                                       \
                }                                                                         \
                                                                                          \
                auto size() const noexcept -> decltype(sizeof(0))                         \
                {                                                                         \
                    auto result = decltype(sizeof(0)){};                                  \
                    for (auto it = begin(), last = end(); it != last; ++it) ++result;     \
                    return result;                                                        \
                }                                                                         \
                                                                                          \
                auto empty() const noexcept -> bool                                       \
                { return size() == 0; }                                                   \
            };                                                                            \
        }                                                                                 \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_, ...)                                                      \
        namespace                                                                                                            \
        {                                                                                                                    \
            const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_) __VA_ARGS__;                            \
                                                                                                                             \
            __pragma(section(RAWR_LS_STR(section_name##$I), read))                                                           \
            __declspec(allocate(RAWR_LS_STR(section_name##$I)))                                                              \
            const decltype(tag_name)::value_type* RAWR_LS_CONCAT(rawr_ls_ptr_, ctr_) = &RAWR_LS_CONCAT(rawr_ls_item_, ctr_); \
        }                                                                                                                    \
        template<>                                                                                                           \
        auto ::rawr::lib::linker_section::msvc::linker_anchor<__FILE__, ctr_>::fn() -> void                                  \
        {                                                                                                                    \
            volatile auto p = &RAWR_LS_CONCAT(rawr_ls_ptr_, ctr_);                                                           \
            (void)p;                                                                                                         \
            __pragma(comment(linker, "/include:" __FUNCDNAME__))                                                             \
        }

// ============================================================================
// PE / GNU - MinGW (GCC or Clang targeting Windows PE)
// ============================================================================
// TODO: Validate non-msvc PE.
#elif RAWR_BIN_PE

    // MinGW sentinels use __attribute__(weak) for COMDAT deduplication - the linker
    // picks one definition across TUs.
    // Equivalent to __declspec(selectany) on this toolchain.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)        \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                      \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {                \
            struct sentinel_t_ { alignas(T_) char _[sizeof(T_)]; };       \
            __attribute__((weak))                                         \
            __attribute__((section(#section_name "$A")))                  \
            __attribute__((used, retain))                                 \
            static const sentinel_t_ sent_start_{};                       \
            __attribute__((weak))                                         \
            __attribute__((section(#section_name "$Z")))                  \
            __attribute__((used, retain))                                 \
            static const sentinel_t_ sent_stop_{};                        \
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

    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_, ...) \
        __attribute__((section(#section_name "$I")))                    \
        __attribute__((used, retain))                                   \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_) __VA_ARGS__

// ============================================================================
// Mach-O - macOS, iOS (ld64, lld/MachO)
// ============================================================================
// TODO: validate MACHO.
#elif RAWR_BIN_MACHO

    // ld64 generates section$start$SEGMENT$section and section$end$SEGMENT$section
    // for non-empty sections. __asm__ binds the C++ extern to those raw
    // linker symbols regardless of which namespace the declaration lives in.
    // __attribute__((weak)) maps to Mach-O weak_import: absent symbol → null.
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
            " - Mach-O section names are limited to 16 characters"      \
        );                                                              \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {              \
            extern const T_ begin_[]                                    \
                __attribute__((weak))                                   \
                __asm__("section$start$__DATA$" #section_name);         \
            extern const T_ end_[]                                      \
                __attribute__((weak))                                   \
                __asm__("section$end$__DATA$"   #section_name);         \
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
    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_, ...) \
        __attribute__((section("__DATA," #section_name)))               \
        __attribute__((used, retain))                                   \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_) __VA_ARGS__

// ============================================================================
// ELF - Linux, bare-metal (any ELF toolchain)
// ============================================================================
#elif RAWR_BIN_ELF

    // __asm__ binds the C++ name to the raw linker-generated symbol,
    // bypassing name mangling and namespace qualification entirely.
    // The namespace the extern lives in is irrelevant to the linker symbol binding.
    // __attribute__((weak)): if the linker omits __start_X or __stop_X (gold +
    // empty section; bare-metal without explicit linker script entries), the symbol
    // resolves to null rather than a link error. start() == stop() == nullptr;
    // the loop body never executes. See bare-metal note in file header.
    #define RAWR_LINKER_SECTION_DEFINE(section_name, tag_name, T_)                            \
        RAWR_LS_DETAIL_VALIDATE_NAME_(section_name);                                          \
        namespace RAWR_LS_CONCAT(rawr_ls_, section_name) {                                    \
            __attribute__((weak)) extern const T_ start_[] __asm__("__start_" #section_name); \
            __attribute__((weak)) extern const T_ stop_[]  __asm__("__stop_"  #section_name); \
            struct tag_type {                                                                 \
                using value_type = T_;                                                        \
                auto begin() const -> const T_* { return start_; }                            \
                auto end()   const -> const T_* { return stop_;  }                            \
                auto size()  const  -> decltype(end() - begin()) {                            \
                    return end() - begin();                                                   \
                }                                                                             \
                auto empty() const -> bool { return begin() == end(); }                       \
            };                                                                                \
        }                                                                                     \
        inline constexpr RAWR_LS_CONCAT(rawr_ls_, section_name)::tag_type tag_name {}

    // static: internal linkage prevents ODR conflicts across TUs registering into
    // the same section. __attribute__((used)) suppresses object-file-level DCE.
    // See LTO note in file header.
    #define RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, ctr_, ...) \
        __attribute__((section(#section_name)))                         \
        __attribute__((used, retain))                                   \
        static const decltype(tag_name)::value_type RAWR_LS_CONCAT(rawr_ls_item_, ctr_) __VA_ARGS__

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
// Registers one entry into the section section_name. Ideally tag_name is the fully
// scoped symbol to avoid any shenanigans.
//
// Remaining arguments initialise T directly, sorta-like so:
//     decltype(tag_name)::value_type some_generated_name __VA_ARGS__;
// some examples that should compile on most cases (prefer the first one):
//     RAWR_LINKER_SECTION_REGISTER(my_hooks, tag_name, {"label", arg2})
//     RAWR_LINKER_SECTION_REGISTER(my_hooks, tag_name, = {"label", arg2})
//     RAWR_LINKER_SECTION_REGISTER(my_hooks, tag_name, = Some_precomputed_value)
//
// RAWR_LS_DETAIL_REGISTER_ is not #undef'd: it is an expansion-time dependency
// of this macro. Do not call it directly.
//
// Two-level indirection forces __COUNTER__ to expand to its integer value
// before token-pasting. ## suppresses expansion of adjacent tokens, so the
// expansion must happen at the call boundary via argument passing.
#define RAWR_LINKER_SECTION_REGISTER(section_name, tag_name, ...) \
    RAWR_LS_DETAIL_REGISTER_(section_name, tag_name, __COUNTER__, __VA_ARGS__)
