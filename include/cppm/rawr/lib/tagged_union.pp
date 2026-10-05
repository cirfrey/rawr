#pragma once

#if RAWRSCAN_METADATA
    #define RAWR_PP_MODULE rawr.lib.tagged_union.pp
#endif

import rawr.lib.integer.base;
import rawr.lib.intrin.construct_at;
#include "rawr/lib/pp.pp"

// =========================================================================
// main macro
//
//     RAWR_TAGGED_UNION(Name,
//         (Tag0, Type0),
//         (Tag1, Type1),
//         ...
//     )
// =========================================================================

#define RAWR_TAGGED_UNION(Name, ...)                                                        \
    struct Name                                                                             \
    {                                                                                       \
        using self = Name;                                                                  \
                                                                                            \
        enum class tag_t : ::rawr::ruint_capable<RAWR_PP_CNT(__VA_ARGS__) + 1>              \
        {                                                                                   \
            valueless, /* Used for invalid states (destructor throws/uninitialized). */     \
            RAWR_PP_EACH(RAWR_TU_ENUM, __VA_ARGS__)                                         \
        };                                                                                  \
                                                                                            \
        union storage_t                                                                     \
        {                                                                                   \
            [[no_unique_address]] struct valueless_t {} valueless;                          \
            RAWR_PP_EACH(RAWR_TU_MEMBER, __VA_ARGS__)                                       \
                                                                                            \
            constexpr storage_t() : valueless{} {}                                          \
            /* REQUIRED: union with non-trivial member needs a user-provided destructor. */ \
            /* It does nothing; the wrapping struct handles the active member.           */ \
            constexpr ~storage_t() {}                                                       \
        };                                                                                  \
                                                                                            \
        [[no_unique_address]] storage_t storage{};                                          \
        tag_t tag = tag_t::valueless;                                                       \
                                                                                            \
    private:                                                                                \
        constexpr auto destroy()                                                            \
        {                                                                                   \
            switch(tag)                                                                     \
            {                                                                               \
                case tag_t::valueless: { break; }                                           \
                RAWR_PP_EACH(RAWR_TU_DESTROY, __VA_ARGS__)                                  \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        constexpr Name() = default;                                                         \
                                                                                            \
    public:                                                                                 \
        struct make { RAWR_PP_EACH(RAWR_TU_MAKE, __VA_ARGS__) };                            \
                                                                                            \
        RAWR_PP_EACH(RAWR_TU_QUERY, __VA_ARGS__)                                            \
                                                                                            \
        constexpr Name(Name const& other) : tag{other.tag}                                  \
        {                                                                                   \
            switch (tag)                                                                    \
            {                                                                               \
                case tag_t::valueless: { return; }                                          \
                RAWR_PP_EACH(RAWR_TU_COPY_CONSTRUCT, __VA_ARGS__)                           \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        constexpr Name(Name&& other) noexcept : tag{other.tag}                              \
        {                                                                                   \
            switch (tag)                                                                    \
            {                                                                               \
                case tag_t::valueless: { return; }                                          \
                RAWR_PP_EACH(RAWR_TU_MOVE_CONSTRUCT, __VA_ARGS__)                           \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        constexpr ~Name() { destroy(); }                                                    \
                                                                                            \
        constexpr Name& operator=(Name const& other)                                        \
        {                                                                                   \
            if (this == &other) { return *this; }                                           \
                                                                                            \
            if (tag == other.tag)                                                           \
            {                                                                               \
                switch (tag)                                                                \
                {                                                                           \
                    case tag_t::valueless: { break; }                                       \
                    RAWR_PP_EACH(RAWR_TU_COPY_ASSIGN, __VA_ARGS__)                          \
                }                                                                           \
                return *this;                                                               \
            }                                                                               \
                                                                                            \
            destroy();                                                                      \
            tag = tag_t::valueless;                                                         \
                                                                                            \
            switch (other.tag)                                                              \
            {                                                                               \
                case tag_t::valueless: { break; };                                          \
                RAWR_PP_EACH(RAWR_TU_COPY_CONSTRUCT, __VA_ARGS__)                           \
            }                                                                               \
            tag = other.tag;                                                                \
                                                                                            \
            return *this;                                                                   \
        }                                                                                   \
                                                                                            \
        constexpr Name& operator=(Name&& other)                                             \
        {                                                                                   \
            if (this == &other) { return *this; }                                           \
                                                                                            \
            if (tag == other.tag)                                                           \
            {                                                                               \
                switch (tag)                                                                \
                {                                                                           \
                    case tag_t::valueless: { break; }                                       \
                    RAWR_PP_EACH(RAWR_TU_MOVE_ASSIGN, __VA_ARGS__)                          \
                }                                                                           \
                return *this;                                                               \
            }                                                                               \
                                                                                            \
            destroy();                                                                      \
            tag = tag_t::valueless;                                                         \
                                                                                            \
            switch (other.tag)                                                              \
            {                                                                               \
                case tag_t::valueless: { break; }                                           \
                RAWR_PP_EACH(RAWR_TU_MOVE_CONSTRUCT, __VA_ARGS__)                           \
            }                                                                               \
            tag = other.tag;                                                                \
                                                                                            \
            return *this;                                                                   \
        }                                                                                   \
    }

// =========================================================================
// Macro helpers
// =========================================================================

#define RAWR_TU_ENUM(pair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_ENUM_, pair)
#define RAWR_TU_ENUM_2(Tag, Type) Tag,
#define RAWR_TU_MEMBER(pair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_MEMBER_, pair)
#define RAWR_TU_MEMBER_2(Tag, Type) [[no_unique_address]] Type Tag;

#define RAWR_TU_DESTROY(pair)               RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_DESTROY_, pair)
#define RAWR_TU_COPY_CONSTRUCT(pair)        RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_COPY_CONSTRUCT_, pair)
#define RAWR_TU_MOVE_CONSTRUCT(pair)        RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_MOVE_CONSTRUCT_, pair)
#define RAWR_TU_COPY_ASSIGN(pair)           RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_COPY_ASSIGN_, pair)
#define RAWR_TU_MOVE_ASSIGN(pair)           RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_MOVE_ASSIGN_, pair)
#define RAWR_TU_DESTROY_2(Tag, Type)        case tag_t::Tag: { using T = decltype(storage_t::Tag); storage.Tag.~T(); break; }
#define RAWR_TU_COPY_CONSTRUCT_2(Tag, Type) case tag_t::Tag: { ::rawr::intrin::construct_at(&storage.Tag, other.storage.Tag); break; }
#define RAWR_TU_MOVE_CONSTRUCT_2(Tag, Type) case tag_t::Tag: { ::rawr::intrin::construct_at(&storage.Tag, static_cast<decltype(storage_t::Tag)&&>(other.storage.Tag)); break; }
#define RAWR_TU_COPY_ASSIGN_2(Tag, Type)    case tag_t::Tag: { storage.Tag = other.storage.Tag; break; }
#define RAWR_TU_MOVE_ASSIGN_2(Tag, Type)    case tag_t::Tag: { storage.Tag = static_cast<decltype(storage_t::Tag)&&>(other.storage.Tag); break; }

#define RAWR_TU_MAKE(pair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_MAKE_, pair)
#define RAWR_TU_MAKE_2(Tag, Type)                                                               \
    template <typename... Args>                                                                 \
    [[nodiscard]] static constexpr auto Tag(Args... args)                                       \
    noexcept(::rawr::intrin::NoThrowConstructible<decltype(storage_t::Tag), Args...>)           \
    requires(::rawr::intrin::Constructible<decltype(storage_t::Tag), Args...>)                  \
    {                                                                                           \
        self v{};                                                                               \
        ::rawr::intrin::construct_at(&v.storage.Tag, args...);                                  \
        v.tag = tag_t::Tag;                                                                     \
        return v;                                                                               \
    }

#define RAWR_TU_QUERY(pair) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_TU_QUERY_, pair)
#define RAWR_TU_QUERY_2(Tag, Type) \
    [[nodiscard]] constexpr auto Tag()                                        noexcept -> decltype(storage_t::Tag)*       { if(tag == tag_t::Tag) { return &storage.Tag; } return nullptr; } \
    [[nodiscard]] constexpr auto Tag()                                  const noexcept -> decltype(storage_t::Tag) const* { if(tag == tag_t::Tag) { return &storage.Tag; } return nullptr; } \
    [[nodiscard]] constexpr auto Tag(decltype(storage_t::Tag) const& v) const noexcept -> bool                            { return tag == tag_t::Tag && storage.Tag == v; }
