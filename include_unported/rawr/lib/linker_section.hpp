#pragma once

// lm/static_section.hpp
//
// Decentralised linker-section registry.
// Requires: lm/core/types.hpp  (u32, usize)
//
// API:
//   LM_STATIC_SECTION_DEFINE(tag_name, T)    — declare a registry (namespace scope)
//   LM_STATIC_SECTION_REGISTER(tag_name, ...) — register an entry; args brace-init T
//   for (auto& x : tag_name) { ... }         — iterate at runtime
//   lm::static_section::manifest             — range over all declared registries
//
// Domain-specific wrappers are the recommended usage pattern:
//   #define LM_REGISTER_TEST(...)  LM_STATIC_SECTION_REGISTER(sys_tests, __VA_ARGS__)
//   LM_REGISTER_TEST("RAM", run_ram)   // expands to TestEntry{"RAM", run_ram}
//
// Registered entries are const (→ .rodata / flash). Mutable variant deferred.
// Section names must be valid C identifiers (no dots, no '$').
// On ELF,  T need not be default-constructible.
// On PE,   sentinels are char-buffer wrappers; T construction never occurs.
//
// Platform split:
//   LM_SS_ELF      GCC / Clang → ELF    (Linux x86-64, ESP32, bare-metal)
//   LM_SS_PE_MSVC  MSVC / clang-cl      (Windows PE)
//   LM_SS_PE_GNU   MinGW GCC / Clang    (Windows PE  — needs integration test)
//
// MSVC NOTE: __pragma(section(...)) resolves to #pragma section, which MSVC
// requires at file scope. If LM_STATIC_SECTION_DEFINE is called inside a
// namespace and the pragma is rejected, factor the pragma lines into a
// companion LM_STATIC_SECTION_DECLARE_PE(tag, T) macro at file scope.

#if defined(_MSC_VER)
    #define LM_SS_PE_MSVC 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define LM_SS_PE_GNU 1
#else
    #define LM_SS_ELF 1
#endif

// ============================================================================
// lm::static_section public types
// ============================================================================

namespace lm::static_section {

// Stored in lm_ss_manifest; consumed by the host linker-script generator.
// All fields are fixed-width value types — no pointers — so the tool reads
// raw section bytes without needing to resolve object-file relocations.
struct manifest_entry {
    char section_name[48]; // C identifier, null-terminated
    u32  element_size;     // sizeof(T)
    u32  element_alignment;// alignof(T)
};

// Stateless range over a typed linker section.
// begin()/end() call Tag static methods; no state stored in the range object.
template<typename Tag>
struct range {
    using value_type = typename Tag::value_type;

    auto begin()             const noexcept -> const value_type* { return Tag::begin(); }
    auto end()               const noexcept -> const value_type* { return Tag::end();   }
    auto size()              const noexcept -> usize { return static_cast<usize>(end() - begin()); }
    auto empty()             const noexcept -> bool  { return begin() == end(); }
    auto operator[](usize i) const noexcept -> const value_type& { return begin()[i]; }
};

} // namespace lm::static_section

// ============================================================================
// Manifest section bootstrap  (file scope, instantiated by this header)
//
// "lm_ss_manifest" is the fixed well-known section. The host tool starts here.
// ELF:  __start/__stop auto-symbols; no sentinels needed for the manifest.
// PE:   sentinels use manifest_entry directly (it is always aggregate / trivial).
// ============================================================================

#if defined(LM_SS_PE_MSVC)

    __pragma(section("lm_ss_manifest$A", read))
    __pragma(section("lm_ss_manifest$I", read))
    __pragma(section("lm_ss_manifest$Z", read))

    __declspec(selectany) __declspec(allocate("lm_ss_manifest$A"))
    const lm::static_section::manifest_entry lm_ss_manifest_sentinel_start_{};

    __declspec(selectany) __declspec(allocate("lm_ss_manifest$Z"))
    const lm::static_section::manifest_entry lm_ss_manifest_sentinel_stop_{};

    struct lm_ss_manifest_tag_ {
        using value_type = lm::static_section::manifest_entry;
        static auto begin() noexcept -> const value_type* {
            return &lm_ss_manifest_sentinel_start_ + 1;
        }
        static auto end() noexcept -> const value_type* {
            return &lm_ss_manifest_sentinel_stop_;
        }
    };

#elif defined(LM_SS_PE_GNU)

    __declspec(selectany) __attribute__((section("lm_ss_manifest$A"), used))
    const lm::static_section::manifest_entry lm_ss_manifest_sentinel_start_{};

    __declspec(selectany) __attribute__((section("lm_ss_manifest$Z"), used))
    const lm::static_section::manifest_entry lm_ss_manifest_sentinel_stop_{};

    struct lm_ss_manifest_tag_ {
        using value_type = lm::static_section::manifest_entry;
        static auto begin() noexcept -> const value_type* {
            return &lm_ss_manifest_sentinel_start_ + 1;
        }
        static auto end() noexcept -> const value_type* {
            return &lm_ss_manifest_sentinel_stop_;
        }
    };

#else // LM_SS_ELF

    // __asm__ pins the extern to the raw linker symbol, bypassing C++ mangling,
    // so this declaration resolves correctly regardless of enclosing namespace.
    extern const lm::static_section::manifest_entry
        lm_ss_manifest_start_[] __asm__("__start_lm_ss_manifest");
    extern const lm::static_section::manifest_entry
        lm_ss_manifest_stop_[]  __asm__("__stop_lm_ss_manifest");

    struct lm_ss_manifest_tag_ {
        using value_type = lm::static_section::manifest_entry;
        static auto begin() noexcept -> const value_type* { return lm_ss_manifest_start_; }
        static auto end()   noexcept -> const value_type* { return lm_ss_manifest_stop_;  }
    };

#endif

namespace lm::static_section {
    inline constexpr range<::lm_ss_manifest_tag_> manifest{};
}

// ============================================================================
// Detail: manifest entry emission
// Called at the tail of LM_STATIC_SECTION_DEFINE; sizeof/alignof captured here.
// ELF: inline ensures a single definition survives if header included in many TUs.
// PE:  selectany provides the same deduplication guarantee via COMDAT.
// ============================================================================

#if defined(LM_SS_PE_MSVC)
    #define LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)                              \
        __declspec(selectany) __declspec(allocate("lm_ss_manifest$I"))            \
        const ::lm::static_section::manifest_entry lm_ss_mfst_##tag_name{        \
            #tag_name,                                                             \
            static_cast<::lm::u32>(sizeof(T)),                                    \
            static_cast<::lm::u32>(alignof(T))                                    \
        }

#elif defined(LM_SS_PE_GNU)
    #define LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)                              \
        __declspec(selectany)                                                      \
        __attribute__((section("lm_ss_manifest$I"), used))                        \
        const ::lm::static_section::manifest_entry lm_ss_mfst_##tag_name{        \
            #tag_name,                                                             \
            static_cast<::lm::u32>(sizeof(T)),                                    \
            static_cast<::lm::u32>(alignof(T))                                    \
        }

#else // LM_SS_ELF
    #define LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)                              \
        __attribute__((section("lm_ss_manifest"), used))                          \
        inline const ::lm::static_section::manifest_entry lm_ss_mfst_##tag_name{ \
            #tag_name,                                                             \
            static_cast<::lm::u32>(sizeof(T)),                                    \
            static_cast<::lm::u32>(alignof(T))                                    \
        }
#endif

// ============================================================================
// Detail: __COUNTER__-based unique name
//
// LM_SS_DETAIL_REGISTER_IMPL_ receives __COUNTER__ already expanded as `ctr`
// (macro arguments are expanded before substitution unless adjacent to ##).
// The ## in the impl body then pastes already-concrete tokens; no recursion.
// ============================================================================

#if defined(LM_SS_PE_MSVC)
    #define LM_SS_DETAIL_REGISTER_IMPL_(tag_name, ctr, ...)                       \
        __declspec(allocate(#tag_name "$I"))                                       \
        static const lm_ss_tag_##tag_name::value_type                             \
            lm_ss_item_##tag_name##_##ctr{__VA_ARGS__}

#elif defined(LM_SS_PE_GNU)
    #define LM_SS_DETAIL_REGISTER_IMPL_(tag_name, ctr, ...)                       \
        __attribute__((section(#tag_name "$I"), used))                            \
        static const lm_ss_tag_##tag_name::value_type                             \
            lm_ss_item_##tag_name##_##ctr{__VA_ARGS__}

#else // LM_SS_ELF
    #define LM_SS_DETAIL_REGISTER_IMPL_(tag_name, ctr, ...)                       \
        __attribute__((section(#tag_name), used))                                 \
        static const lm_ss_tag_##tag_name::value_type                             \
            lm_ss_item_##tag_name##_##ctr{__VA_ARGS__}
#endif

// ============================================================================
// LM_STATIC_SECTION_DEFINE(tag_name, T)
//
// Declares a typed linker-section registry at the current namespace scope.
// Defines:
//   lm_ss_tag_##tag_name        — internal tag struct  (lm_ss_ prefix, no conflict)
//   tag_name                    — constexpr range object visible to users
//   lm_ss_mfst_##tag_name       — manifest entry for the host tool
//
// PE sentinel strategy: a char-buffer struct sized and aligned to T.
// No T object is ever constructed; begin()/end() reinterpret the boundary.
// This removes the default-constructible requirement on T for PE targets.
// ============================================================================

#if defined(LM_SS_PE_MSVC)

    #define LM_STATIC_SECTION_DEFINE(tag_name, T)                                 \
        __pragma(section(#tag_name "$A", read))                                    \
        __pragma(section(#tag_name "$I", read))                                    \
        __pragma(section(#tag_name "$Z", read))                                    \
        struct lm_ss_sentinel_##tag_name##_t_ { alignas(T) char _[sizeof(T)]; };  \
        __declspec(selectany) __declspec(allocate(#tag_name "$A"))                 \
        const lm_ss_sentinel_##tag_name##_t_ lm_ss_sentinel_##tag_name##_start_{};\
        __declspec(selectany) __declspec(allocate(#tag_name "$Z"))                 \
        const lm_ss_sentinel_##tag_name##_t_ lm_ss_sentinel_##tag_name##_stop_{}; \
        struct lm_ss_tag_##tag_name {                                              \
            using value_type = T;                                                  \
            static auto begin() noexcept -> const T* {                            \
                return reinterpret_cast<const T*>(                                 \
                    &lm_ss_sentinel_##tag_name##_start_ + 1);                     \
            }                                                                      \
            static auto end() noexcept -> const T* {                              \
                return reinterpret_cast<const T*>(                                 \
                    &lm_ss_sentinel_##tag_name##_stop_);                          \
            }                                                                      \
        };                                                                         \
        inline constexpr ::lm::static_section::range<lm_ss_tag_##tag_name>       \
            tag_name{};                                                            \
        LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)

#elif defined(LM_SS_PE_GNU)

    #define LM_STATIC_SECTION_DEFINE(tag_name, T)                                 \
        struct lm_ss_sentinel_##tag_name##_t_ { alignas(T) char _[sizeof(T)]; };  \
        __declspec(selectany) __attribute__((section(#tag_name "$A"), used))       \
        const lm_ss_sentinel_##tag_name##_t_ lm_ss_sentinel_##tag_name##_start_{};\
        __declspec(selectany) __attribute__((section(#tag_name "$Z"), used))       \
        const lm_ss_sentinel_##tag_name##_t_ lm_ss_sentinel_##tag_name##_stop_{}; \
        struct lm_ss_tag_##tag_name {                                              \
            using value_type = T;                                                  \
            static auto begin() noexcept -> const T* {                            \
                return reinterpret_cast<const T*>(                                 \
                    &lm_ss_sentinel_##tag_name##_start_ + 1);                     \
            }                                                                      \
            static auto end() noexcept -> const T* {                              \
                return reinterpret_cast<const T*>(                                 \
                    &lm_ss_sentinel_##tag_name##_stop_);                          \
            }                                                                      \
        };                                                                         \
        inline constexpr ::lm::static_section::range<lm_ss_tag_##tag_name>       \
            tag_name{};                                                            \
        LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)

#else // LM_SS_ELF

    // __asm__ pins the extern arrays to raw linker symbols regardless of the
    // enclosing C++ namespace. The tag's begin()/end() use unqualified lookup
    // and find these declarations in the same scope as the tag struct itself.
    #define LM_STATIC_SECTION_DEFINE(tag_name, T)                                 \
        extern const T lm_ss_start_##tag_name[] __asm__("__start_" #tag_name);    \
        extern const T lm_ss_stop_##tag_name[]  __asm__("__stop_"  #tag_name);    \
        struct lm_ss_tag_##tag_name {                                              \
            using value_type = T;                                                  \
            static auto begin() noexcept -> const T* {                            \
                return lm_ss_start_##tag_name;                                     \
            }                                                                      \
            static auto end() noexcept -> const T* {                              \
                return lm_ss_stop_##tag_name;                                      \
            }                                                                      \
        };                                                                         \
        inline constexpr ::lm::static_section::range<lm_ss_tag_##tag_name>       \
            tag_name{};                                                            \
        LM_SS_DETAIL_EMIT_MANIFEST_(tag_name, T)

#endif

// ============================================================================
// LM_STATIC_SECTION_REGISTER(tag_name, ...)
//
// Registers one entry into the linker section for tag_name. Must be at
// namespace scope. Remaining arguments directly brace-initialize T, so no
// wrapper braces or parentheses are needed at the call site:
//
//   LM_STATIC_SECTION_REGISTER(sys_tests, "RAM", run_ram)
//   LM_STATIC_SECTION_REGISTER(sys_drivers, &uart_driver)
//
// internal linkage (static) prevents ODR conflicts across TUs for same ctr.
// __COUNTER__ is expanded at the LM_STATIC_SECTION_REGISTER call site and
// arrives at the impl macro as a concrete integer token.
// ============================================================================

#define LM_STATIC_SECTION_REGISTER(tag_name, ...) \
    LM_SS_DETAIL_REGISTER_IMPL_(tag_name, __COUNTER__, __VA_ARGS__)
