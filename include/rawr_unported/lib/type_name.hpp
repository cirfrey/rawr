
//// rawr/lib/type_name.hpp
#pragma region rawr/lib/type_name.hpp

#ifndef RAWR_UNITY
    #pragma once
    #include "rawr/lib/detection.pp"
#endif

// Details.
namespace rawr::inline lib::detail::tn
{
    // Basically string_view::find.
    constexpr auto tn_find(const char* haystack, const char* needle, unsigned long long start)
    {
        for(auto i = start; haystack[i]; ++i) {
            for(auto j = 0;; ++j) {
                if(!needle[j]) return i;
                if(haystack[i + j] != needle[j]) break;
            }
        }
        return ~0ull;
    }

    // Basically string_view.
    struct tn_info
    {
        const char* start;
        unsigned long long len;
    };

    template <typename T>
    consteval auto get_tn_info() -> tn_info {
        #if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
            constexpr auto name           = __PRETTY_FUNCTION__;
            constexpr char start_marker[] = "T = ";
            constexpr char end_marker[]   = "]";
        #elif RAWR_COMPILER_MSVC
            constexpr auto name           = __FUNCSIG__;
            constexpr char start_marker[] = "get_tn_info<";
            constexpr char end_marker[]   = ">";
        #else
            static_assert(sizeof(T) == 0, "Please implement get_tn_info() for this compiler.");
        #endif

        auto start = tn_find(name, start_marker, 0);
        if(start == ~0ull) throw "Start marker not found";

        auto end = tn_find(name, end_marker, start);
        if(end == ~0ull) throw "End marker not found";

        return {
            .start = name + start + sizeof(start_marker) - 1,
            .len   = end  - start - sizeof(start_marker) + 1,
        };
    }

    template <auto N>
    struct tn_array {
        char data[N] = {0};
        // For the purposes of treating this as a non-null-terminated view.
        static constexpr unsigned long long size = N - 1;
        constexpr auto c_str() const { return data; }
    };
}

namespace rawr::inline lib
{
    template <typename T>
    constexpr auto tn = []() consteval {
        constexpr auto tni = detail::tn::get_tn_info<T>();

        detail::tn::tn_array<tni.len + 1> ret;
        for(auto i = 0; i < ret.size; ++i)
            ret.data[i] = tni.start[i];

        return ret;
    }();
}

#pragma endregion rawr/lib/type_name.hpp
