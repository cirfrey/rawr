#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/bitfield.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.bitfield;
    import rawr.lib.integer.base;
    import rawr.lib.bits;
    import rawr.lib.intrin.base;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/bits.hpp"
    #include "rawr/lib/intrin/base.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib::bitfield
{
    struct field
    {
        char const* name;
        decltype(sizeof(0)) offset;
        decltype(sizeof(0)) width;
    };

    /// Pairwise interval-overlap check, N known at compile time.
    template <int N>
    constexpr auto fields_overlap(field const (&f_in)[N]) -> bool {
        field sorted_fields[N];
        for (int i = 0; i < N; ++i) { sorted_fields[i] = f_in[i]; }

        // Insertion sort by offset:
        for (int i = 1; i < N; ++i) {
            field key = sorted_fields[i];
            int idx2 = i - 1;
            while (idx2 >= 0 && sorted_fields[idx2].offset > key.offset) { sorted_fields[idx2+1] = sorted_fields[idx2]; --idx2; }
            sorted_fields[idx2+1] = key;
        }

        // At this point the fields are sorted by bit order (bit 1 first - bit N last),
        // now we just check for overlap by comparing neighbours.
        for (int i = 0; i + 1 < N; ++i)
        { if (sorted_fields[i].offset + sorted_fields[i].width > sorted_fields[i+1].offset) { return true; } }

        return false;
    }

    template <class T> inline constexpr bool is_const_v = false;
    template <class T> inline constexpr bool is_const_v<const T> = true;

    template <typename WordType, typename ValueType>
    struct accessor
    {
        using word_type  = WordType;
        using value_type = ValueType;
        using mask_type  = ruint_exact<bitsof<WordType>>;
        using uv         = ruint_exact<bitsof<ValueType>>; // value_type-sized unsigned bridge

        static constexpr auto word_bits = bitsof<WordType>;

        word_type* const storage;
        decltype(sizeof(0)) const offset;
        decltype(sizeof(0)) const width;

        constexpr auto get() noexcept -> value_type
        {
            const auto word_index  = offset / word_bits.val;
            const auto bit_in_word = offset % word_bits.val;
            const mask_type mask_value = (width == word_bits.val)
                ? static_cast<mask_type>(~mask_type{0})
                : static_cast<mask_type>((mask_type{1} << width) - 1);
            const mask_type raw = static_cast<mask_type>(storage[word_index] >> bit_in_word) & mask_value;

            if constexpr (aint<value_type>) {
                if constexpr (sint<value_type> && width < bitsof<value_type>) {
                    uv uraw     = static_cast<uv>(raw);
                    uv sign_bit = uv{1} << (width - 1);
                    uv extended = static_cast<uv>((uraw ^ sign_bit) - sign_bit);
                    return static_cast<value_type>(extended); // well-defined since C++20 (two's complement mandated)
                } else {
                    return static_cast<value_type>(raw);
                }
            } else if constexpr (__is_trivially_copyable(value_type)) {
                return __builtin_bit_cast(value_type, static_cast<uv>(raw));
            } else {
                static_assert(sizeof(value_type) == 0,
                    "accessor: value_type is neither an arithmetic integer type nor bit_cast-compatible with this field");
            }
        }

        constexpr operator value_type() noexcept { return get(); }

        constexpr auto operator=(value_type val) noexcept -> value_type
            requires (!is_const_v<word_type>)
        {
            const auto word_index  = offset / word_bits.val;
            const auto bit_in_word = offset % word_bits.val;
            const mask_type mask_value = (width == word_bits.val)
                ? static_cast<mask_type>(~mask_type{0})
                : static_cast<mask_type>((mask_type{1} << width) - 1);
            const mask_type write_mask = mask_value << bit_in_word;

            mask_type raw;
            if constexpr (aint<value_type>) {
                raw = static_cast<mask_type>(val);
            } else if constexpr (intrin::is_trivially_copyable<value_type>) {
                raw = static_cast<mask_type>(intrin::bit_cast<uv>(val));
            } else {
                static_assert(sizeof(value_type) == 0,
                    "accessor: value_type is neither an arithmetic integer type nor bit_cast-compatible with this field");
            }

            storage[word_index] = (storage[word_index] & ~write_mask) | ((raw & mask_value) << bit_in_word);
            return val;
        }
    };
}
