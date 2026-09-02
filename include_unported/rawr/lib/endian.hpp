#pragma once

#include "lm/core/veil.hpp"
#include "lm/core/machine.hpp"

namespace lm::endian
{
    // Enum for explicit endian selection
    enum class order : char { little, big };
    static constexpr auto native = port.is_little_endian ? order::little : order::big;

    // Generic byte‑swap for arbitrary integral types (uses builtins or manual)
    template <typename T>
    constexpr T byte_swap(T val) {
        static_assert(veil::is_integral<T>, "byte_swap requires integral type");
        if constexpr (sizeof(T) == 1) return val;
        else if constexpr (sizeof(T) == 2) return static_cast<T>(__builtin_bswap16(static_cast<u16>(val)));
        else if constexpr (sizeof(T) == 4) return static_cast<T>(__builtin_bswap32(static_cast<u32_t>(val)));
        else if constexpr (sizeof(T) == 8) return static_cast<T>(__builtin_bswap64(static_cast<u64>(val)));
        // TODO: could we maybe dispatch this as many byte_swap calls instead of not supporting?
        else { static_assert(veil::false_type<T>::value, "Unsupported size"); }
    }

    // Convert a value to a given target endianness (byte‑swap if necessary)
    template <order Target, order From = native, typename T>
    constexpr T to_endian(T val) {
        if constexpr (Target == From || sizeof(T) == 1)
            return val;
        else
            return byte_swap(val);
    }

    // Overload with runtime order (if you need dynamic dispatch)
    template <typename T>
    constexpr T to_endian(T val, order target, order from = native) {
        if (target == from) return val;
        if constexpr (native == order::little) {
            return (target == order::big) ? byte_swap(val) : val;
        } else {
            return (target == order::little) ? byte_swap(val) : val;
        }
    }

    // Convert from a given source endian to native
    template <typename T>
    constexpr T from_endian(T val, order source) {
        return to_endian(val, source);  // because swap is its own inverse
    }
} // namespace lm::endian
