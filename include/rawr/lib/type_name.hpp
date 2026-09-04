#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/type_name.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.type_name;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"

namespace rawr::inline lib::type_name::detail
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
        #if RAWR_COMPILER_FAMILY_GNU
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

RAWR_EXPORT namespace rawr::inline lib
{
    template <typename T>
    constexpr auto tn = []() consteval {
        constexpr auto tni = type_name::detail::get_tn_info<T>();

        type_name::detail::tn_array<tni.len + 1> ret;
        for(auto i = 0; i < ret.size; ++i)
            ret.data[i] = tni.start[i];

        return ret;
    }();
}
