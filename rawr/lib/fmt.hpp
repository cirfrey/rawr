// lm/core/format.hpp
// Zero-dependency compile-time/runtime string formatter.
//
// Primary interface:
//   "x={} y={}"_fmt(x, y)                    → format::buf<N> (auto sized)
//
// Extend via formatter<T> specialisations. See LM_FORMATTER for a
// convenience macro for simple types.
#pragma once

namespace lm::format
{
    using st = decltype(sizeof(0));

    struct buf {
        char* data = nullptr;
        st    size = 0;

        constexpr st append_safe(char const* src, st n)
        {
            auto copied = n > size ? size : n;
            __builtin_memcpy(data, src, copied);
            size -= copied;
            data += copied;
            return copied;
        }

        constexpr st append(char const* src, st n)
        {
            __builtin_memcpy(data, src, n);
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

        // Accepts any view-like — lm::text, tn_array, fixed_str, etc.
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

    // Owning fixed-size stack buffer container returned by lm::format
    template <st N> struct output {
        char data[N == 0 ? 1 : N] = {0};
        st length = 0;

        constexpr operator const char*() const noexcept { return data; }
        constexpr const char* c_str() const noexcept { return data; }
        constexpr st capacity() const noexcept { return N; }
    };

    // ── fixed_str — structural NTTP string wrapper ────────────────────────────────

    template <st N>
    struct fixed_str {
        char data[N];
        static constexpr st size = N - 1;
        constexpr fixed_str(char const (&s)[N]) noexcept { __builtin_memcpy(data, s, N); }
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

// ── Public API ────────────────────────────────────────────────────────────────
namespace lm
{
    // ── formatter — open extensible trait ────────────────────────────────────

    template <typename T> struct formatter;

    namespace format {
        template <typename T>
        constexpr void erased_formatter(buf& b, void const* v, view s) {
            formatter<T>::format(b, *static_cast<T const*>(v), s);
        }

        struct erased_arg {
            void const* object;
            void (*format)(format::buf&, void const*, format::view);
            st required_buf;

            template <typename T>
            constexpr erased_arg(T const& v)
                : object{&v}
                , format{erased_formatter<T>}
                , required_buf{ formatter<T>::default_format_buffer_size }
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
            if(buf.size < args[i].required_buf) return;
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
            : (formatter<Args>::default_format_buffer_size + ... + (FS.size + 1));

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

        result.length = static_cast<format::st>(buf.data - result.data);
        return result;
    }

    template <format::fixed_str FS, format::st BufSize = 0, typename... Args>
    [[nodiscard]] consteval auto fmt_constant(Args const&... args) noexcept
    {
        constexpr format::st cap = BufSize
            ? BufSize
            : (formatter<Args>::default_format_buffer_size + ... + (FS.size + 1));

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
            if(buf.size < F::default_format_buffer_size) return false;
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
        result.length = static_cast<format::st>(buf.data - result.data);
        return result;
    }

} // namespace lm


// ── _fmt UDL family ───────────────────────────────────────────────────────────

namespace lm::inline format_literals
{
    template <format::fixed_str FS>
    struct fmt_lit {
        [[nodiscard]] constexpr auto operator()(auto const&... args) const noexcept {
            return lm::fmt<FS>(args...);
        }

        template <format::st BufSize = 0>
        [[nodiscard]] consteval auto ct(auto const&... args) const noexcept {
            return  lm::fmt_constant<FS, BufSize>(args...);
        }

        template <format::st BufSize = 0>
        [[nodiscard]] constexpr auto bufsize(auto const&... args) const noexcept {
            return lm::fmt<FS, BufSize>(args...);
        }
    };

    template <format::fixed_str FS> consteval auto operator""_fmt() noexcept { return fmt_lit<FS>{}; }
} // namespace lm


#define LM_FORMATTER(Type, SizeBound)                                                  \
    template <> struct lm::formatter<Type> {                                \
        static constexpr lm::format::st default_format_buffer_size = SizeBound;          \
        static constexpr void format(lm::format::buf&, Type const&, lm::format::view) noexcept; \
    };                                                                           \
    constexpr void lm::formatter<Type>::format(                              \
        [[maybe_unused]] lm::format::buf& buf, [[maybe_unused]] Type const& v, [[maybe_unused]] lm::format::view specifier) noexcept


#include "lm/core/fmt.formatters.hpp"
