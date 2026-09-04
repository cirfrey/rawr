#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/bitfield.pp"
#endif
#pragma once

#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.lib.bitfield;
    import rawr.lib.bits;
    import rawr.lib.integer.base;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/lib/bitfield.hpp"
    #include "rawr/lib/bits.hpp"
    #include "rawr/lib/integer/base.hpp"
#endif
#include "rawr/lib/pp.pp"

/// --- entry points: word type inferred (3 args) or explicit (4 args) ---
#define RAWR_BITFIELD(Name, Bytes, Fields, ...)                     RAWR_BITFIELD_IMPL(Name, Bytes, ::rawr::lib::integer::base::ruint_exact<Bytes>, Fields, RAWR_BF_OVERLAP_CHECK_ON,  RAWR_PP_STRIP(__VA_ARGS__))
#define RAWR_BITFIELD_W(Name, Bytes, WordType, Fields, ...)         RAWR_BITFIELD_IMPL(Name, Bytes, WordType,                  Fields, RAWR_BF_OVERLAP_CHECK_ON,  RAWR_PP_STRIP(__VA_ARGS__))
#define RAWR_BITFIELD_RELAXED(Name, Bytes, Fields, ...)             RAWR_BITFIELD_IMPL(Name, Bytes, ::rawr::lib::integer::base::ruint_exact<Bytes>, Fields, RAWR_BF_OVERLAP_CHECK_OFF, RAWR_PP_STRIP(__VA_ARGS__))
#define RAWR_BITFIELD_RELAXED_W(Name, Bytes, WordType, Fields, ...) RAWR_BITFIELD_IMPL(Name, Bytes, WordType,                  Fields, RAWR_BF_OVERLAP_CHECK_OFF, RAWR_PP_STRIP(__VA_ARGS__))

#define RAWR_BF_OVERLAP_CHECK_ON(Fields) static_assert(!::rawr::lib::bitfield::fields_overlap(Fields), \
    "RAWR_BITFIELD: accessor fields overlap — use RAWR_BITFIELD_RELAXED if intentional");
#define RAWR_BF_OVERLAP_CHECK_OFF(Fields)

/// --- struct body, shared by all four entry points ---
#define RAWR_BITFIELD_IMPL(Name, Bytes, WordType, Fields, OverlapCheck, ...)      \
    struct Name {                                                                 \
        using storage_word_type = WordType;                                       \
        static constexpr auto word_bits = bitsof<storage_word_type>;              \
        static_assert(Bytes.val % sizeof(storage_word_type) == 0,                 \
            #Name ": Bytes must be an exact multiple of the storage word size");  \
        static constexpr auto word_count = Bytes.val / sizeof(storage_word_type); \
        storage_word_type storage[word_count]{};                                  \
                                                                                  \
        static constexpr ::rawr::lib::bitfield::field fields[] = {                \
            RAWR_PP_EACH_CTX(                                                     \
                RAWR_BF_FIELDS,                                                   \
                Name,                                                             \
                RAWR_PP_EACH_SEP(                                                 \
                    RAWR_PP_ENSURE_PAREN,                                         \
                    RAWR_PP_COMMA_SEP,                                            \
                    RAWR_PP_STRIP(Fields)                                         \
                    )                                                             \
                )                                                                 \
        };                                                                        \
        OverlapCheck(fields)                                                      \
                                                                                  \
        RAWR_PP_EACH_CTX(                                                         \
            RAWR_BF_ACCESSOR,                                                     \
            Name,                                                                 \
            RAWR_PP_EACH_SEP(                                                     \
                RAWR_PP_ENSURE_PAREN,                                             \
                RAWR_PP_COMMA_SEP,                                                \
                RAWR_PP_STRIP(Fields)                                             \
            )                                                                     \
        )                                                                         \
                                                                                  \
        __VA_ARGS__                                                               \
    }

#define RAWR_BF_FIELDS(Name, quad) RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_BF_FIELDS_, RAWR_PP_PREPEND_PLIST(quad, Name))
#define RAWR_BF_FIELDS_4(Name, AccessorName, Offset, Width)       { #AccessorName, Offset, Width},
#define RAWR_BF_FIELDS_5(Name, AccessorName, Offset, Width, Type) { #AccessorName, Offset, Width},

/// --- accessor leaf ---
#define RAWR_BF_ACCESSOR(Name, quad) \
    RAWR_PP_DISPATCH_PLIST_BY_ARITY(RAWR_BF_ACCESSOR_, RAWR_PP_PREPEND_PLIST(quad, Name))
#define RAWR_BF_ACCESSOR_4(Name, AccessorName, Offset, Width) \
    RAWR_BF_ACCESSOR_5(Name, AccessorName, Offset, Width, ::rawr::lib::integer::base::ruint_exact<::rawr::lib::bits::bytewidth{(Width+7)/8}>)
#define RAWR_BF_ACCESSOR_5(Name, AccessorName, Offset, Width, Type) \
    static_assert(\
        ((Offset % (sizeof(storage_word_type) * 8)) + Width <= sizeof(storage_word_type) * 8) && \
        (Width <= sizeof(Type) * 8),                                                                   \
        #Name "[" #AccessorName "]: Make sure that:"\
            "\n- Field does not cross a storage word boundary — split the field or widen the word type."\
            "\n- Declared value_type is wide enough to hold a " #Type " field."\
    );                                          \
    constexpr auto AccessorName()                noexcept -> ::rawr::lib::bitfield::accessor<storage_word_type,                Type> { return {storage, Offset, Width}; } \
    constexpr auto AccessorName() const          noexcept -> ::rawr::lib::bitfield::accessor<storage_word_type const,          Type> { return {storage, Offset, Width}; } \
              auto AccessorName() volatile       noexcept -> ::rawr::lib::bitfield::accessor<storage_word_type volatile,       Type> { return {storage, Offset, Width}; } \
              auto AccessorName() const volatile noexcept -> ::rawr::lib::bitfield::accessor<storage_word_type const volatile, Type> { return {storage, Offset, Width}; } \
    constexpr auto AccessorName(Type val) noexcept          ->          Name& { AccessorName() = val; return *this; }                                                     \
              auto AccessorName(Type val) volatile noexcept -> volatile Name& { AccessorName() = val; return *this; }
