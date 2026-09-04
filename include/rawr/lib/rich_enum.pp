#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/data/rich_enum.pp"
#endif
// Rawr's strong enumeration abstraction:
// Provides RAWR_RICH_ENUM and RAWR_RICH_FLAGS.
// MSVC: /Zc:preprocessor required.
#pragma once

#include "rawr/lib/pp.pp"

// ── RAWR_RICH_ENUM ─────────────────────────────────────────────────────────────
// Usage:
//   RAWR_RICH_ENUM(role, unsigned char, (
//       (host),
//       (device, 12),
//       (observer, 255)
//   ), (
//       [[nodiscard]] constexpr bool is_host() const noexcept { return to_enum_type() == enum_type::host; }
//   ))
//
//   int main(int argc, char** argv)
//   {
//       role r = role::from_underlying_unchecked(argc);
//
//       auto x = role::host;                                                             // deduces to role, not role::enum_type.
//       x = role::from_enum_type(role::enum_type::device);                               // ::from_enum_type() escape hatch.
//       { auto _ = r.to_underlying(); }                                                  // → static_cast<unsigned char>(1).
//       { auto _ = r.to_enum_type();  }                                                  // → role::enum_type::device, A.k.a: role::_enum_type::role::device.
//       switch (r) { case role::host: break; }                                           // via operator enum_type().
//       switch (r) { using enum role::enum_type; case host: break; case device: break; } // Short syntax supported.
//       role::enum_count;                                                                // → 3.
//       role::enum_table[1].name;                                                        // → "device".
//       { auto _ = role::from_enum_type(role::enum_table[0].value); }                    // → role::host.
//
//       return r.to_underlying();
//   }
//
// template <role R> struct whatever{}; // NTTP: role is a structural type.
#define RAWR_RICH_ENUM(Name, Under, Enumerations, Body)                                                                  \
    RAWR_RICH_BASE_(Name, Under, RAWR_PP_STRIP(Enumerations))                                                            \
        static constexpr bool _is_rawr_rich_enum = true;                                                                 \
                                                                                                                         \
        Name() = delete;                                                                                                 \
                                                                                                                         \
        constexpr operator enum_type() const noexcept { return _enum_value; }                                            \
                                                                                                                         \
        /* Generates is_IDENT() for each IDENT passed to this macro. */                                                  \
        RAWR_PP_EACH(RAWR_RE_IS, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, RAWR_PP_STRIP(Enumerations))) \
                                                                                                                         \
        RAWR_PP_STRIP(Body)                                                                                              \
    };                                                                                                                   \
    /* static_assert(sizeof(Name) == sizeof(Under) && alignof(Name) == alignof(Under)); */                               \
    RAWR_PP_EACH_CTX(RAWR_RE_STATIC_IMPL, Name, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, RAWR_PP_STRIP(Enumerations)))

// ── RAWR_RICH_FLAGS ─────────────────────────────────────────────────────────────
// No operator enum_type() — flag combinations have no meaningful single
// enumerator identity and switch on flags is a design mistake.
//
// Usage:
//   RAWR_RICH_FLAGS(perms, unsigned char, (
//       (read,  1 << 0),
//       (write, 1 << 1),
//       (exec,  1 << 2)
//   ), (
//       static const perms rw;
//       static const perms all;
//   ))
//   inline constexpr perms perms::rw  = perms::read | perms::write;
//   inline constexpr perms perms::all = perms::read | perms::write | perms::exec;
//
//   int main(int argc, char** argv)
//   {
//       auto p = perms::from_underlying_unchecked(argc);
//       { auto _ = p.has(perms::read); }                   // true.
//       { auto _ = p.has_read(); }                         // true — generated per enumerator.
//       { auto _ = p.has_all(perms::read, perms::write); } // true.
//       { auto _ = p.has_any(perms::exec, perms::write); } // true.
//       p.set(perms::exec);    p.clear(perms::write);    p.toggle(perms::read);
//       return p.to_underlying();
//   }
#define RAWR_RICH_FLAGS(Name, Under, Enumerations, Body)                                                                                                                      \
    RAWR_RICH_BASE_(Name, Under, RAWR_PP_STRIP(Enumerations))                                                                                                                 \
        static constexpr bool _is_rawr_rich_flags = true;                                                                                                                     \
                                                                                                                                                                              \
        constexpr Name() noexcept : _enum_value{} {}                                                                                                                          \
                                                                                                                                                                              \
        [[nodiscard]] friend constexpr auto operator|(Name lhs, Name rhs) noexcept -> Name { return from_underlying_unchecked( lhs.to_underlying() | rhs.to_underlying() ); } \
        [[nodiscard]] friend constexpr auto operator&(Name lhs, Name rhs) noexcept -> Name { return from_underlying_unchecked( lhs.to_underlying() & rhs.to_underlying() ); } \
        [[nodiscard]] friend constexpr auto operator^(Name lhs, Name rhs) noexcept -> Name { return from_underlying_unchecked( lhs.to_underlying() ^ rhs.to_underlying() ); } \
        [[nodiscard]] friend constexpr auto operator~(Name flags)         noexcept -> Name { return from_underlying_unchecked( ~flags.to_underlying() ); }                    \
                                                                                                                                                                              \
        [[nodiscard]] constexpr auto has(Name other) const noexcept -> bool                                                                                                   \
        {                                                                                                                                                                     \
            const Under val = static_cast<Under>(_enum_value);                                                                                                                \
            const Under other_val = static_cast<Under>(other._enum_value);                                                                                                    \
            return (val & other_val) == other_val;                                                                                                                            \
        }                                                                                                                                                                     \
        [[nodiscard]] constexpr auto has_all(auto... flags) const noexcept -> bool { return (has(flags) && ...); }                                                            \
        [[nodiscard]] constexpr auto has_any(auto... flags) const noexcept -> bool { return (has(flags) || ...); }                                                            \
        /* Generates has_IDENT for each IDENT passed to this macro. */                                                                                                        \
        RAWR_PP_EACH_CTX(RAWR_RF_HAS, Name, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, RAWR_PP_STRIP(Enumerations)))                                           \
                                                                                                                                                                              \
                      constexpr auto clear(Name other)                   noexcept -> Name& { *this = *this & ~other; return *this; }                                          \
                      constexpr auto toggle(Name other)                  noexcept -> Name& { *this = *this ^ other;  return *this; }                                          \
                      constexpr auto set(Name other)                     noexcept -> Name& { *this = *this | other;  return *this; }                                          \
                      constexpr auto set_if(bool cond, Name other)       noexcept -> Name& { if(cond) set(other); return *this; }                                             \
        [[nodiscard]] constexpr auto set(Name other)               const noexcept -> Name  { return *this | other;  }                                                         \
        [[nodiscard]] constexpr auto clear(Name other)             const noexcept -> Name  { return *this & ~other; }                                                         \
        [[nodiscard]] constexpr auto toggle(Name other)            const noexcept -> Name  { return *this ^ other;  }                                                         \
                                                                                                                                                                              \
        RAWR_PP_STRIP(Body)                                                                                                                                                   \
    };                                                                                                                                                                        \
    /* static_assert(sizeof(Name) == sizeof(Under) && alignof(Name) == alignof(Under)); */                                                                                    \
    RAWR_PP_EACH_CTX(RAWR_RE_STATIC_IMPL, Name, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, RAWR_PP_STRIP(Enumerations)))

// ── RAWR_RICH_BASE_ ────────────────────────────────────────────────────────────
// Internal scaffold shared by RAWR_RICH_ENUM and RAWR_RICH_FLAGS.
// Not part of the public API — use the two public macros above.
//
// Storage: enum_type _enum_value (public, underscore convention).
// Storing the inner enum type rather than a raw scalar means:
//   - operator== compares enum values directly, no cast
//   - direct assignment to _enum_value is restricted to enum_type values;
//     arbitrary integers require an explicit static_cast
//   - the struct is a structural type (all non-static data members are
//     public and of structural type), so template<Name R> works as an NTTP
//     without any alias — no nttp_type alias needed
//
// _enum_type is private. External access goes through the public enum_type
// typedef. The reflection table stores enum_type values; callers reconstruct
// Name via from_enum_type(entry.value) at their own layer.
//
// The inner enum is named the same as the outer type so that reflection libraries
// that generate the full - or semi-scoped name - create a nicer user experience.
//
// Reflection tools provided: enum_info struct + enum_table[] in declaration order +
// enum_count. No to_string, no iteration helper, no operator const char* —
// these carry semantic assumptions the caller should own.

#define RAWR_RICH_BASE_(Name, Under, ...)                                                                                                       \
    struct Name {                                                                                                                               \
    private:                                                                                                                                    \
        struct _enum_type { enum class Name : Under {                                                                                           \
            RAWR_PP_EACH(RAWR_RE_INNER, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, __VA_ARGS__))                                 \
        }; };                                                                                                                                   \
        explicit constexpr Name(_enum_type::Name enm) noexcept : _enum_value{enm} {}                                                            \
        using self = Name;                                                                                                                      \
    public:                                                                                                                                     \
        using underlying_type = Under;                                                                                                          \
        using enum_type       = _enum_type::Name;                                                                                               \
                                                                                                                                                \
        enum_type _enum_value;                                                                                                                  \
                                                                                                                                                \
        /* Constructors: */                                                                                                                     \
        [[nodiscard]] static constexpr auto from_enum_type(enum_type enm)        noexcept -> Name { return Name{enm}; }                         \
        [[nodiscard]] static constexpr auto from_underlying_unchecked(Under val) noexcept -> Name { return Name{static_cast<enum_type>(val)}; } \
                                                                                                                                                \
        /* Conversions: */                                                                                                                      \
        [[nodiscard]] constexpr auto to_underlying() const noexcept -> Under     { return static_cast<Under>(_enum_value); }                    \
        [[nodiscard]] constexpr auto to_enum_type()  const noexcept -> enum_type { return _enum_value; }                                        \
        explicit      constexpr operator  Under()         const noexcept { return static_cast<Under>(_enum_value); }                            \
                                                                                                                                                \
        /* This is pretty much the only guarantee an enum should have. Ordering is not universally needed or even logical. */                   \
        /* The underlying representation is just an unique ID, it shouldn't be conflated with ordering. */                                      \
        [[nodiscard]] constexpr auto operator==(Name other)   const noexcept -> bool { return _enum_value == other._enum_value; }               \
                                                                                                                                                \
        /* This generates the following member for each IDENT passed to this macro: "static const Name IDENT;" */                               \
        RAWR_PP_EACH_CTX(RAWR_RE_STATIC_DECL, Name, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, __VA_ARGS__))                     \
                                                                                                                                                \
        /* And a final little treat for reflection lovers. */                                                                                   \
        static constexpr unsigned enum_count = RAWR_PP_CNT(__VA_ARGS__);                                                                        \
        struct enum_info { enum_type value; char const* name; };                                                                                \
        static constexpr enum_info enum_table[] = { RAWR_PP_EACH(RAWR_RE_TABLE, RAWR_PP_EACH_SEP(RAWR_PP_ENSURE_PAREN, RAWR_PP_COMMA_SEP, __VA_ARGS__)) };


/// Auxiliary macros.
// RAWR_RICH_BASE_:
#define RAWR_RE_INNER(pair)                   RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RE_INNER_, pair)
#define RAWR_RE_INNER_1(ident)                ident,
#define RAWR_RE_INNER_2(ident, val)           ident = val,
#define RAWR_RE_STATIC_DECL(Name, pair)       RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RE_STATIC_DECL_, RAWR_PP_PREPEND_PLIST(pair, Name))
#define RAWR_RE_STATIC_DECL_2(Name, ident)    static const Name ident;
#define RAWR_RE_STATIC_DECL_3(Name, ident, _) static const Name ident;
#define RAWR_RE_TABLE(pair)                   RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RE_TABLE_, pair)
#define RAWR_RE_TABLE_1(ident)                { enum_type::ident, #ident },
#define RAWR_RE_TABLE_2(ident, val)           { enum_type::ident, #ident },
// RAWR_RICH_ENUM:
#define RAWR_RE_IS(pair)                        RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RE_IS_, pair)
#define RAWR_RE_IS_1(ident)                     [[nodiscard]] constexpr auto is_##ident() const noexcept -> bool { return to_enum_type() == enum_type::ident; }
#define RAWR_RE_IS_2(ident, val)                [[nodiscard]] constexpr auto is_##ident() const noexcept -> bool { return to_enum_type() == enum_type::ident; }
#define RAWR_RE_STATIC_IMPL(Name, pair)         RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RE_STATIC_IMPL_, RAWR_PP_PREPEND_PLIST(pair, Name))
#define RAWR_RE_STATIC_IMPL_2(Name, ident)      inline constexpr Name Name::ident = Name::from_enum_type(Name::enum_type::ident);
#define RAWR_RE_STATIC_IMPL_3(Name, ident, val) inline constexpr Name Name::ident = Name::from_enum_type(Name::enum_type::ident);
// RAWR_RICH_FLAGS:
#define RAWR_RF_HAS(Name, pair)         RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_RF_HAS_, RAWR_PP_PREPEND_PLIST(pair, Name))
#define RAWR_RF_HAS_2(Name, ident)      [[nodiscard]] constexpr auto has_##ident() const noexcept -> bool { return has(from_enum_type(Name::enum_type::ident)); }
#define RAWR_RF_HAS_3(Name, ident, val) [[nodiscard]] constexpr auto has_##ident() const noexcept -> bool { return has(from_enum_type(Name::enum_type::ident)); }
