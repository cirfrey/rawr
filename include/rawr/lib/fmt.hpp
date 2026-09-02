//// rawr/lib/fmt.hpp.

#ifdef RAWR_MODULE
    export module rawr.lib.fmt;
    import rawr.lib.type_name;
    import rawr.lib.intrin;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/type_name.hpp"
    #include "rawr/lib/intrin.hpp"

    #include "rawr/lib/dist/header.pp"
#endif

RAWR_EXPORT namespace rawr::inline lib::format
{
    using st = decltype(sizeof(0));

    struct buf {
        char* data = nullptr;
        st    size = 0;

        constexpr st append_safe(char const* src, st n)
        {
            auto copied = n > size ? size : n;
            intrin::memcpy(data, src, copied);
            size -= copied;
            data += copied;
            return copied;
        }

        constexpr st append(char const* src, st n)
        {
            intrin::memcpy(data, src, n);
            size -= n;
            data += n;
            return n;
        }

        constexpr st append(char c)
        {
            *data++ = c;
            --size;
            return 1;
        }

        template <st N>
        constexpr st append(char const (&src)[N]) { return append(src, N - 1); }
    };

    struct view {
        char const* data = nullptr;
        st size = 0;

        // Accepts any view-like thing (.data and .size).
        template <typename S>
        requires requires(S const& s) {
            { static_cast<decltype(data)>(s.data) };
            { static_cast<decltype(size)>(s.size) };
        }
        static constexpr auto of(S const& s) -> view
        {
            return view{
                .data = static_cast<decltype(data)>(s.data),
                .size = static_cast<decltype(size)>(s.size),
            };
        }
    };

    // Owning fixed-size buffer container returned by formatting functions.

    template <st> struct output;

    template <st Capacity>
    requires (Capacity > 0)
    struct output<Capacity> {
        static constexpr auto capatity = Capacity;

        char data[Capacity] = {0};
        st size = 0;

        constexpr operator const char*() const noexcept { return data; }
        constexpr const char* c_str() const noexcept { return data; }
    };

    template <st Capacity>
    requires (Capacity == 0)
    struct output<Capacity> {
        static constexpr auto capatity = Capacity;

        static constexpr auto data = nullptr;
        static constexpr auto size = 0;

        constexpr operator const char*() const noexcept { return data; }
        constexpr const char* c_str() const noexcept { return data; }
    };

    // ── fixed_str — structural NTTP string wrapper ────────────────────────────────

    template <st N>
    struct fixed_str {
        char data[N];
        static constexpr st size = N - 1;
        constexpr fixed_str(char const (&s)[N]) noexcept { intrin::memcpy(data, s, N); }
    };
    template <st N> fixed_str(char const (&)[N]) -> fixed_str<N>;

    // ── Engine ────────────────────────────────────────────────────────────────────

    struct str_offset {
        st begin;
        st len;
    };

    template <st MaxArgs>
    struct parsed_format {
        str_offset literals[MaxArgs + 1]{};
        str_offset specs[MaxArgs]{};
    };

    consteval st count_args(view v) {
        st count = 0;

        for (st i = 0; i < v.size; ++i) {
            if (v.data[i] == '{') {
                ++count;

                while (v.data[++i] != '}');
            }
        }

        return count;
    }

    template <st ArgCount>
    consteval auto parse(view v) {
        parsed_format<ArgCount> out{};

        st literal_begin = 0;
        st arg = 0;

        for (st i = 0; i < v.size; ++i) {

            if (v.data[i] != '{')
                continue;

            out.literals[arg] = {
                static_cast<st>(literal_begin),
                static_cast<st>(i - literal_begin)
            };

            st spec_begin = i + 1;

            while (v.data[++i] != '}');

            out.specs[arg] = {
                static_cast<st>(spec_begin),
                static_cast<st>(i - spec_begin)
            };

            literal_begin = i + 1;
            ++arg;
        }

        out.literals[arg] = {
            static_cast<st>(literal_begin),
            static_cast<st>(v.size - literal_begin)
        };

        return out;
    }

    template <fixed_str FS>
    constexpr auto parsed_ast = parse<count_args({FS.data, FS.size})>({FS.data, FS.size});
}

RAWR_EXPORT namespace rawr::inline lib
{
    template <typename Formatter>
    concept compliant_formatter = requires(Formatter const& fmt, format::buf& buf, format::view specifier){
        typename Formatter::value_type;

        { fmt.worst_case_buffer_size } -> intrin::convertible_to<format::st>;
        // Commented for now, need to get a T from formatter.
        { fmt.format(
            buf,
            intrin::declval<typename Formatter::value_type const&>(),
            specifier
        ) } noexcept;
    };

    // default_formatter is rawr-owned, you are not expected to override it. If you
    // have custom formatting necessities override formatter<> instead.
    // The global default_formatter just prints the type name and the memory address.
    // We have some more default_formatters below too.
    template <typename T>
    struct default_formatter
    {
        using value_type = T;
        static constexpr format::st worst_case_buffer_size = tn<T>.size + 1 + default_formatter<T*>::worst_case_buffer_size;
        static constexpr void format(format::buf& buf, T const& v, format::view) noexcept {
            buf.append(tn<T>.data, tn<T>.size);
            buf.append('@');
            default_formatter<T const*>::format(buf, &v, {});
        }
    };

    // The global formatter defers to default_formatter of the type.
    // This ensures you can  still override it for you own types if needed.
    template <typename T>
    struct formatter
    {
        using value_type = T;
        static constexpr auto is_global_formatter = true; // Might be handy to someone somewhere.

        static constexpr format::st worst_case_buffer_size = default_formatter<T>::worst_case_buffer_size;
        static constexpr auto format(format::buf& buf, T const& val, format::view view) noexcept -> void
        { return default_formatter<T>::format(buf, val, view); }
    };

    namespace format {
        template <typename T, template <typename> typename Formatter = formatter>
        requires (compliant_formatter<Formatter<T>>)
        constexpr void erase_formatter(buf& b, void const* v, view s) {
            Formatter<T>::format(b, *static_cast<T const*>(v), s);
        }

        struct erased_arg {
            void const* object;
            void (*format)(format::buf&, void const*, format::view);
            st worst_case_buffer_size;

            template <template <typename> typename Formatter = formatter, typename T>
            requires (compliant_formatter<Formatter<T>>)
            constexpr erased_arg(T const& v)
                : object{&v}
                , format{ erase_formatter<T, Formatter> }
                , worst_case_buffer_size{ Formatter<T>::worst_case_buffer_size }
            {}
        };
    }

    constexpr auto fmt_erased(
        const char* fmt,
        format::buf& buf,
        format::str_offset const* literals,
        format::str_offset const* specs,
        format::erased_arg const* args,
        format::st argcount
    ) {
        for(format::st i = 0; i < argcount; ++i){
            auto& literal = literals[i];
            if(buf.size < literal.len) return;
            buf.append(fmt + literal.begin, literal.len);
            if(buf.size < args[i].worst_case_buffer_size) return;
            args[i].format(buf, args[i].object, {fmt + specs[i].begin, specs[i].len});
        }
        // Copy the last literal.
        auto& literal = literals[argcount];
        if(buf.size < literal.len) return;
        buf.append(fmt + literal.begin, literal.len);
    }

    template <format::fixed_str FS, format::st BufSize = 0, typename... Args>
    [[nodiscard]] constexpr auto fmt(Args const&... args) noexcept
    {
        constexpr format::st cap = BufSize
            ? BufSize
            : (formatter<Args>::worst_case_buffer_size + ... + (FS.size + 1));

        auto result = format::output<cap>{};
        auto buf    = format::buf{result.data, cap - 1};

        const format::erased_arg erased[] = {{args}...};
        fmt_erased(
            FS.data,
            buf,
            &format::parsed_ast<FS>.literals[0],
            &format::parsed_ast<FS>.specs[0],
            erased,
            sizeof...(args)
        );

        result.size = static_cast<format::st>(buf.data - result.data);
        return result;
    }

    template <format::fixed_str FS, format::st BufSize = 0, typename... Args>
    [[nodiscard]] consteval auto fmt_constant(Args const&... args) noexcept
    {
        constexpr format::st cap = BufSize
            ? BufSize
            : (formatter<Args>::worst_case_buffer_size + ... + (FS.size + 1));

        auto result     = format::output<cap>{};
        auto buf        = format::buf{result.data, cap - 1};
        const char* fmt = FS.data;

        auto& literals = format::parsed_ast<FS>.literals;
        auto& specs = format::parsed_ast<FS>.specs;
        auto emit_lit = [&]<typename Arg>(auto i, Arg const& arg){
            auto& literal = literals[i];
            if(buf.size < literal.len) return false;
            buf.append(fmt + literal.begin, literal.len);
            using F = formatter< Arg >;
            if(buf.size < F::worst_case_buffer_size) return false;
            F::format(
                buf,
                arg,
                {fmt + specs[i].begin, specs[i].len}
            );
            return true;
        };

        format::st argcount = 0;
        (
            emit_lit(argcount++, args) &&  ...
        );

        // Copy the last literal.
        auto& literal = literals[argcount];
        if(buf.size >= literal.len) {
            buf.append(fmt + literal.begin, literal.len);
        }
        result.size = static_cast<format::st>(buf.data - result.data);
        return result;
    }

} // namespace lm


// ── _fmt UDL family ───────────────────────────────────────────────────────────

RAWR_EXPORT namespace rawr::inline lib::inline format_literals
{
    template <format::fixed_str FS>
    struct fmt_lit {
        [[nodiscard]] constexpr auto operator()(auto const&... args) const noexcept
        { return rawr::fmt<FS>(args...); }

        template <format::st BufSize = 0>
        [[nodiscard]] consteval auto ct(auto const&... args) const noexcept
        { return  rawr::fmt_constant<FS, BufSize>(args...); }

        template <format::st BufSize = 0>
        [[nodiscard]] constexpr auto bufsize(auto const&... args) const noexcept
        { return rawr::fmt<FS, BufSize>(args...); }
    };

    template <format::fixed_str FS> consteval auto operator""_fmt() noexcept { return fmt_lit<FS>{}; }
}


/// Some default_formatters below.

RAWR_EXPORT namespace rawr::inline lib::format
{
    namespace number_formatter_common
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
        using value_type = T;
        // TODO: calculate this from T.
        static constexpr st worst_case_buffer_size = 21;

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

                if (hex) number_formatter_common::format_hex(buf, magnitude);
                else number_formatter_common::format_dec(buf, magnitude);
            }
            else // Else if is unsigned.
            {
                auto magnitude = static_cast<unsigned long long>(value);

                if (hex) number_formatter_common::format_hex(buf, magnitude);
                else     number_formatter_common::format_dec(buf, magnitude);
            }
        }
    };
}

template <> struct rawr::lib::default_formatter<unsigned char>      : rawr::lib::format::number_formatter<unsigned char>      {};
template <> struct rawr::lib::default_formatter<signed char>        : rawr::lib::format::number_formatter<signed char>        {};
template <> struct rawr::lib::default_formatter<unsigned short>     : rawr::lib::format::number_formatter<unsigned short>     {};
template <> struct rawr::lib::default_formatter<short>              : rawr::lib::format::number_formatter<short>              {};
template <> struct rawr::lib::default_formatter<unsigned int>       : rawr::lib::format::number_formatter<unsigned int>       {};
template <> struct rawr::lib::default_formatter<int>                : rawr::lib::format::number_formatter<int>                {};
template <> struct rawr::lib::default_formatter<unsigned long>      : rawr::lib::format::number_formatter<unsigned long>      {};
template <> struct rawr::lib::default_formatter<long>               : rawr::lib::format::number_formatter<long>               {};
template <> struct rawr::lib::default_formatter<unsigned long long> : rawr::lib::format::number_formatter<unsigned long long> {};
template <> struct rawr::lib::default_formatter<long long>          : rawr::lib::format::number_formatter<long long>          {};

#define RAWR_DEFAULT_FORMATTER(Type, Size)                                                      \
    template <> struct rawr::lib::default_formatter<Type>                                       \
    {                                                                                           \
        using value_type = Type;                                                                \
        static constexpr format::st worst_case_buffer_size = Size;                              \
        static constexpr auto format(format::buf&, Type const&, format::view) noexcept -> void; \
    };                                                                                          \
    constexpr auto rawr::lib::default_formatter<Type>::format(                                  \
        [[maybe_unused]] format::buf& buf,                                                      \
        [[maybe_unused]] Type const& val,                                                       \
        [[maybe_unused]] format::view specifier                                                 \
    ) noexcept -> void

RAWR_DEFAULT_FORMATTER(bool, 5){
    buf.append(val ? "true" : "false", val ? 4 : 5);
}

RAWR_DEFAULT_FORMATTER(decltype(nullptr), 7){
    buf.append("nullptr");
}

// ── T* — pointer as hex ───────────────────────────────────────────────────
// Writes "0x" followed by the address in lowercase hex.
// sizeof(void*) * 2 digits + "0x" + null = 18 on 64-bit.
template <typename T>
struct rawr::lib::default_formatter<T*>
{
    using value_type = T*;
    static constexpr format::st worst_case_buffer_size = 18;
    static constexpr void format(format::buf& buf, T* const& val, format::view) noexcept {
        buf.append("0x");

        // Write pointer digits from most-significant to least, skipping leading zeros.
        auto addr = reinterpret_cast<unsigned long long>(static_cast<void const*>(val));
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
RAWR_DEFAULT_FORMATTER(char const*, 64){
    if(!val) { buf.append("(nil)"); return; }
    format::st size = 0;
    while (val[size] != '\0') ++size;
    buf.append_safe(val, size);
}

RAWR_DEFAULT_FORMATTER(char*, 64){ default_formatter<char const*>::format(buf, val, specifier); }


// ── char[N] — string literal decay ───────────────────────────────────────
template <rawr::format::st N>
struct rawr::default_formatter<char[N]>
{
    using value_type = char[N];
    static constexpr format::st worst_case_buffer_size = N - 1;
    static constexpr void format(format::buf& buf, char const* v, format::view) noexcept {
        buf.append(v, worst_case_buffer_size);
    }
};

// ── formatting one output into another ───────────────────────────────────────
template <rawr::format::st N>
struct rawr::default_formatter<rawr::format::output<N>>
{
    using value_type = rawr::format::output<N>;
    static constexpr format::st worst_case_buffer_size = N ? N - 1 : 0;
    static constexpr void format(format::buf& buf, rawr::format::output<N> const& val, format::view) noexcept {
        buf.append(val.data, val.size);
    }
};

#undef RAWR_DEFAULT_FORMATTER
