#pragma once

#include "lm/core/fmt.hpp"
#include "lm/core/type_name.hpp"

template <typename T>
struct lm::formatter {
    static constexpr format::st default_format_buffer_size = tn<T>.size + 1 + formatter<T*>::default_format_buffer_size;

    static constexpr void format(format::buf& buf, T const& v, format::view) noexcept {
        buf.append(tn<T>.data, tn<T>.size);
        buf.append('@');
        formatter<T const*>::format(buf, &v, {});
    }
};

namespace lm::format
{
    namespace detail
    {
        constexpr char digits[] =
            "0123456789abcdef";

        constexpr char digit_pairs[] =
            "00010203040506070809"
            "10111213141516171819"
            "20212223242526272829"
            "30313233343536373839"
            "40414243444546474849"
            "50515253545556575859"
            "60616263646566676869"
            "70717273747576777879"
            "80818283848586878889"
            "90919293949596979899";

        constexpr void format_dec(
            buf& buf,
            unsigned long long value
        ) noexcept
        {
            // max decimal digits of uint64_t = 20
            char tmp[20];
            char* p = tmp + sizeof(tmp);

            while (value >= 100)
            {
                auto pair = value % 100;
                value /= 100;

                auto idx = static_cast<unsigned>(pair) * 2;

                *--p = digit_pairs[idx + 1];
                *--p = digit_pairs[idx];
            }

            if (value < 10)
            {
                *--p = char('0' + value);
            }
            else
            {
                auto idx = static_cast<unsigned>(value) * 2;

                *--p = digit_pairs[idx + 1];
                *--p = digit_pairs[idx];
            }

            buf.append(p, static_cast<st>(tmp + sizeof(tmp) - p));
        }

        constexpr void format_hex(
            buf& buf,
            unsigned long long value
        ) noexcept
        {
            // max hex digits of uint64_t = 16
            char tmp[16];
            char* p = tmp + sizeof(tmp);

            do
            {
                *--p = digits[value & 0xf];
                value >>= 4;
            }
            while (value);

            buf.append(p, static_cast<st>(tmp + sizeof(tmp) - p));
        }
    }

    template<typename T>
    struct number_formatter
    {
        static constexpr st default_format_buffer_size = 21;

        static constexpr void format(
            buf& buf,
            T const& value,
            view spec
        ) noexcept
        {
            bool const hex =
                spec.size >= 2 &&
                spec.data[0] == ':' &&
                spec.data[1] == 'x';

            if constexpr (T(-1) < T(0)) // If is signed.
            {
                auto v = static_cast<long long>(value);
                auto magnitude = static_cast<unsigned long long>(v);

                if (v < 0)
                {
                    buf.append("-", 1);
                    magnitude = ~magnitude + 1;
                }

                if (hex) detail::format_hex(buf, magnitude);
                else detail::format_dec(buf, magnitude);
            }
            else // Else if is unsigned.
            {
                auto magnitude = static_cast<unsigned long long>(value);

                if (hex) detail::format_hex(buf, magnitude);
                else     detail::format_dec(buf, magnitude);
            }
        }
    };
}

template <> struct lm::formatter<unsigned char>      : lm::format::number_formatter<unsigned char>      {};
template <> struct lm::formatter<signed char>        : lm::format::number_formatter<signed char>        {};
template <> struct lm::formatter<unsigned short>     : lm::format::number_formatter<unsigned short>     {};
template <> struct lm::formatter<short>              : lm::format::number_formatter<short>              {};
template <> struct lm::formatter<unsigned int>       : lm::format::number_formatter<unsigned int>       {};
template <> struct lm::formatter<int>                : lm::format::number_formatter<int>                {};
template <> struct lm::formatter<unsigned long>      : lm::format::number_formatter<unsigned long>      {};
template <> struct lm::formatter<long>               : lm::format::number_formatter<long>               {};
template <> struct lm::formatter<unsigned long long> : lm::format::number_formatter<unsigned long long> {};
template <> struct lm::formatter<long long>          : lm::format::number_formatter<long long>          {};

LM_FORMATTER(bool, 5){
    buf.append(v ? "true" : "false", v ? 4 : 5);
}

LM_FORMATTER(decltype(nullptr), 7){
    buf.append("nullptr");
}

// ── T* — pointer as hex ───────────────────────────────────────────────────
// Writes "0x" followed by the address in lowercase hex.
// sizeof(void*) * 2 digits + "0x" + null = 18 on 64-bit.

template <typename T>
struct lm::formatter<T*> {
    static constexpr format::st default_format_buffer_size = 18;
    static constexpr void format(format::buf& buf, T* const& v, format::view) noexcept {
        buf.append("0x");

        // Write pointer digits from most-significant to least, skipping leading zeros.
        auto addr = reinterpret_cast<unsigned long long>(
            static_cast<void const*>(v));
        constexpr int digits = sizeof(void*) * 2;
        constexpr char hex[] = "0123456789abcdef";
        bool leading = true;
        for (int i = digits - 1; i >= 0; --i) {
            auto nibble = (addr >> (i * 4)) & 0xF;
            if (leading && nibble == 0 && i != 0) continue;
            leading = false;
            buf.append(hex[nibble]);
        }
        if (leading) buf.append("0"); // v == nullptr
    }
};


// ── const char* ───────────────────────────────────────────────────────────

LM_FORMATTER(char const*, 64){
    if(!v) { buf.append("(nil)"); return; }
    format::st size = 0;
    while (v[size] != '\0') ++size;
    buf.append_safe(v, size);
}

LM_FORMATTER(char*, 64){ formatter<char const*>::format(buf, v, specifier); }

// ── char[N] — string literal decay ───────────────────────────────────────

template <lm::format::st N> struct lm::formatter<char[N]> {
    static constexpr format::st default_format_buffer_size = N - 1;
    static constexpr void format(format::buf& buf, char const* v, format::view) noexcept {
        buf.append(v, default_format_buffer_size);
    }
};

template <lm::format::st N> struct lm::formatter<lm::format::output<N>> {
    static constexpr format::st default_format_buffer_size = N ? N - 1 : 0;
    static constexpr void format(format::buf& buf, lm::format::output<N> const& v, format::view) noexcept {
        buf.append(v.data, v.length);
    }
};

namespace lm::format::detail
{
    struct range_spec {
        view open      = {"[", 1};
        view close     = {"]", 1};
        view sep       = {", ", 2};
        view elem_spec = {"", 0};
    };

    constexpr range_spec parse_range_spec(view spec) noexcept {
        range_spec rs;

        constexpr format::st npos = ~format::st{0};

        if (spec.size && spec.data[0] == ':') { ++spec.data; --spec.size; }
        if (!spec.size) return rs;

        // Find the element specifier delimiter — second ':' in the original,
        // first ':' now that the leading one is consumed.
        st elem_delim = npos;
        for (st i = 0; i < spec.size; ++i)
            if (spec.data[i] == ':') { elem_delim = i; break; }

        view range_part = spec;
        if (elem_delim != npos) {
            range_part  = {spec.data, elem_delim};
            rs.elem_spec = {spec.data + elem_delim, spec.size - elem_delim};
        }

        // Parse open|sep|close.
        st first = npos, second = npos;
        for (st i = 0; i < range_part.size; ++i) {
            if (range_part.data[i] != '|') continue;
            if (first == npos) first = i;
            else { second = i; break; }
        }

        if (first == npos) {
            // No pipes: entire specifier is the separator.
            rs.sep = range_part;
        } else if (second == npos) {
            // One pipe: open|close, separator stays default.
            rs.open  = {range_part.data, first};
            rs.close = {range_part.data + first + 1, range_part.size - first - 1};
        } else {
            // Two pipes: open|sep|close.
            rs.open  = {range_part.data, first};
            rs.sep   = {range_part.data + first + 1, second - first - 1};
            rs.close = {range_part.data + second + 1, range_part.size - second - 1};
        }

        return rs;
    }
}

template <typename T, lm::format::st N>
struct lm::formatter<T[N]> {
    static constexpr format::st default_format_buffer_size = N * (formatter<T>::default_format_buffer_size + 3 /* Typical separator size*/) + 2 /* Typical open+close size */;

    static void format(
        format::buf&  buf,
        T const(&arr)[N],
        format::view  spec
    ) noexcept {
        auto rs = format::detail::parse_range_spec(spec);

        buf.append(rs.open.data, rs.open.size);

        bool first = true;
        for (auto const& elem : arr) {
            if (!first) buf.append(rs.sep.data, rs.sep.size);
            first = false;
            formatter<T>::format(buf, elem, rs.elem_spec);
        }

        buf.append(rs.close.data, rs.close.size);
    }
};
