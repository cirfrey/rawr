#pragma once

#include "lm/core/base.hpp"

namespace lm
{
    constexpr auto fnv1a_32(buf data) -> u32 {
        u32 hash = 0x811c9dc5_u32;
        for (auto i = 0_st; i < data.size; ++i) {
            hash ^= u32{data.data[i]};
            hash *= 0x01000193_u32;
        }
        return hash;
    }
    constexpr auto fnv1a_32(text data) -> u32 {
        u32 hash = 0x811c9dc5_u32;
        for (auto i = 0_st; i < data.size; ++i) {
            hash ^= u32{(u8)data.data[i]};
            hash *= 0x01000193_u32;
        }
        return hash;
    }

    constexpr auto fnv1a_64(buf data) -> u64 {
        u64 hash = 0xcbf29ce484222325_u64;
        for (auto i = 0_st; i < data.size; ++i) {
            hash ^= u64{data.data[i]};
            hash *= 0x100000001b3_u64;
        }
        return hash;
    }
    constexpr auto fnv1a_64(text data) -> u64 {
        u64 hash = 0xcbf29ce484222325_u64;
        for (auto i = 0_st; i < data.size; ++i) {
            hash ^= u64{(u8)data.data[i]};
            hash *= 0x100000001b3_u64;
        }
        return hash;
    }

    inline namespace literals
    {
        constexpr auto operator ""_hash32(char const* str, st len) { return fnv1a_32({str, len}); }
        constexpr auto operator ""_hash64(char const* str, st len) { return fnv1a_64({str, len}); }
    }
}
