/// The only thing unported from fmt.

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
