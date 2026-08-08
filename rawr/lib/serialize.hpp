#pragma once

#include "lm/core/base.hpp"
#include "lm/core/veil.hpp"
#include "lm/core/endian.hpp"

#include <cstring> // std::memcpy.

namespace lm {
    // An enum that *should* be returned by every serialization/deserialization action,
    // but really you can return whatever you want.
    enum class serialize_status : char {
        ok = 0,
        buffer_too_small,
        bad_args,
        invalid_data,       // for deserialization (e.g., bad magic number)
    };

    template <typename T>
    struct serialize_traits {
        static_assert(veil::false_type<T>::value,
            "No serialization defined for this type. "
            "Specialise lm::serialize_traits<T> or use LM_SERIALIZE_BASIC / LM_SERIALIZE_STRUCT.");
    };
    template <typename T>
    struct deserialize_traits {
        static_assert(veil::false_type<T>::value,
            "No deserialization defined for this type. "
            "Specialise lm::deserialize_traits<T> or use LM_SERIALIZE_BASIC / LM_SERIALIZE_STRUCT.");
    };

    // ---------------------------------------------------------------------------
    // Free functions (entry point for users)
    // It is implied that these return serialize_status, but it's not hard and fast, they can return anything really.
    // ---------------------------------------------------------------------------
    /// Serialize `val` into buffer `buf`. Extra arguments (e.g., endian::order) forwarded to traits.
    template <typename T, typename... Args>
    constexpr auto serialize(const T& val, mut_buf b, Args&&... args) {
        return serialize_traits<T>::serialize(val, b, veil::forward<Args>(args)...);
    }

    /// Deserialize from buffer `buf` into `val`.
    template <typename T, typename... Args>
    constexpr auto deserialize(buf b, T& val, Args&&... args) {
        return deserialize_traits<T>::deserialize(b, val, veil::forward<Args>(args)...);
    }
} // namespace lm

// ---------------------------------------------------------------------------
// LM_SERIALIZE_STRUCT – explicit field‑by‑field serialization / deserialization
//
// Usage:
//   LM_SERIALIZE_STRUCT(my_struct, field1, field2, field3)
//
// The generated serialize/deserialize methods accept any additional
// arguments (e.g., endian::order) and forward them to each field’s traits.
// ---------------------------------------------------------------------------

#define LM_SERIALIZE_STRUCT(StructType, ...)                                   \
    template <> struct lm::serialize_traits<StructType> {                      \
        template <typename... Args>                                            \
        static constexpr auto serialize(const StructType& obj,                 \
                                        mut_buf b, Args&&... args)             \
            -> serialize_status                                                \
        {                                                                      \
            LM_PP_FOR_EACH(LM_SERIALIZE_STRUCT_IMPL, __VA_ARGS__)              \
            return serialize_status::ok;                                       \
        }                                                                      \
    };

#define LM_DESERIALIZE_STRUCT(StructType, ...)                                 \
    template <> struct lm::deserialize_traits<StructType> {                    \
        template <typename... Args>                                            \
        static constexpr auto deserialize(buf b, StructType& obj,              \
                                          Args&&... args)                      \
            -> serialize_status                                                \
        {                                                                      \
            LM_PP_FOR_EACH(LM_DESERIALIZE_STRUCT_IMPL, __VA_ARGS__)            \
            return serialize_status::ok;                                       \
        }                                                                      \
    }

// ---------------------------------------------------------------------------
// Per‑field helpers (internal)
// ---------------------------------------------------------------------------

#define LM_SERIALIZE_STRUCT_IMPL(field)                                        \
    if (auto _st = lm::serialize_traits<                                       \
                    veil::remove_cvref_t<decltype(obj.field)>>::serialize(     \
                        obj.field, b, veil::forward<Args>(args)...);           \
        _st != serialize_status::ok)                                           \
        return _st;

#define LM_DESERIALIZE_STRUCT_IMPL(field)                                      \
    if (auto _st = lm::deserialize_traits<                                     \
                    veil::remove_cvref_t<decltype(obj.field)>>::deserialize(   \
                        b, obj.field, veil::forward<Args>(args)...);           \
        _st != serialize_status::ok)                                           \
        return _st;

// ---------------------------------------------------------------------------
// LM_SERIALIZE_STRUCT_PATCH – field list = only the fields that need
//                              endian conversion (the rest are raw bytes)
// ---------------------------------------------------------------------------
#define LM_SERIALIZE_STRUCT_PATCH(StructType, ...)                             \
    template <> struct lm::serialize_traits<StructType> {                      \
        static_assert(veil::is_standard_layout_v<StructType>,                   \
            "LM_SERIALIZE_STRUCT_PATCH requires a standard-layout type");      \
                                                                               \
        template <typename... Args>                                            \
        static constexpr auto serialize(const StructType& obj,                 \
                                        mut_buf b, Args&&... args)             \
            -> serialize_status                                                \
        {                                                                      \
            if (b.size < sizeof(StructType))                                   \
                return serialize_status::buffer_too_small;                     \
            /* 1. raw copy of the whole struct */                              \
            std::memcpy(b.data, &obj, sizeof(StructType));                     \
            /* 2. patch listed fields in‑place (in the buffer) */              \
            LM_PP_FOR_EACH(LM_SERIALIZE_PATCH_SERIALIZE_FIELD, __VA_ARGS__)    \
            return serialize_status::ok;                                       \
        }                                                                      \
    };

#define LM_DESERIALIZE_STRUCT_PATCH(StructType, ...)                           \
    template <> struct lm::deserialize_traits<StructType> {                    \
        static_assert(veil::is_standard_layout_v<StructType>,                  \
            "LM_DESERIALIZE_STRUCT_PATCH requires a standard-layout type");    \
                                                                               \
        template <typename... Args>                                            \
        static constexpr auto deserialize(buf b, StructType& obj,              \
                                          Args&&... args)                      \
            -> serialize_status                                                \
        {                                                                      \
            if (b.size < sizeof(StructType))                                   \
                return serialize_status::buffer_too_small;                     \
            /* 1. raw copy from buffer into object */                          \
            std::memcpy(&obj, b.data, sizeof(StructType));                     \
            /* 2. patch listed fields in the object */                         \
            LM_PP_FOR_EACH(LM_SERIALIZE_PATCH_DESERIALIZE_FIELD, __VA_ARGS__)  \
            return serialize_status::ok;                                       \
        }                                                                      \
    };

/// TODO: review these two macros.

// Internal helpers – use offsetof to access the field inside the raw bytes
#define LM_SERIALIZE_PATCH_SERIALIZE_FIELD(field)                              \
    {                                                                          \
        using FieldT = veil::remove_cvref_t<decltype(StructType::field)>;      \
        auto* ptr = reinterpret_cast<FieldT*>(b.data + offsetof(StructType, field)); \
        *ptr = endian::to_endian(*ptr,                                         \
            /* from = native */ endian::order::native,                         \
            /* to   = wire format, passed in args */                           \
            /* We need to extract the endian::order from args... */            \
            /* For simplicity, assume the first extra arg is the target */     \
            /* endian::order. The macro can be adapted to use args. */         \
        );                                                                     \
    }

// The deserialize version works directly on obj.field
#define LM_SERIALIZE_PATCH_DESERIALIZE_FIELD(field)                            \
    obj.field = endian::to_endian(obj.field,                                   \
        /* from = wire */ endian::order::little,  /* or from args */           \
        /* to   = native */ endian::order::native);

// ---------------------------------------------------------------------------
// Base integral serialiser – used by LM_SERIALIZE_BASIC and enum fallback
// ---------------------------------------------------------------------------
namespace lm {
    template <typename T>
    concept trivially_serializable = veil::is_trivially_copyable_v<T> && !veil::is_pointer_v<T>;

    template <trivially_serializable T>
    struct basic_integral_serializer {
        static constexpr auto serialize(
            const T& val,
            mut_buf b,
            endian::order to   = endian::order::native,   // target wire format
            endian::order from = endian::order::native    // format in memory
        ) -> serialize_status
        {
            if (b.size < sizeof(T)) return serialize_status::buffer_too_small;

            T raw = endian::to_endian(val, to, from);   // convert only if from != to
            std::memcpy(b.data, &raw, sizeof(T));
            return serialize_status::ok;
        }
    };

    template <trivially_serializable T>
    struct basic_integral_deserializer {
        static constexpr auto deserialize(
            buf b,
            T& val,
            endian::order from = endian::order::native,   // wire format we read
            endian::order to   = endian::order::native    // desired memory format
        ) -> serialize_status
        {
            if (b.size < sizeof(T)) return serialize_status::buffer_too_small;

            std::memcpy(&val, b.data, sizeof(T));
            val = endian::to_endian(val, to, from);   // convert if necessary
            return serialize_status::ok;
        }
    };

    #define LM_SERIALIZE_BASIC(Type) \
        template <> struct lm::serialize_traits<Type> : lm::basic_integral_serializer<Type> {}
    #define LM_DESERIALIZE_BASIC(Type) \
        template <> struct lm::deserialize_traits<Type> : lm::basic_integral_deserializer<Type> {}
}

// ---------------------------------------------------------------------------
// Automatic partial specialisation for all enum types (safe fallback)
// ---------------------------------------------------------------------------
namespace lm {
    template <typename T>
    concept serialize_enum_type = veil::is_enum_v<T>;

    template <serialize_enum_type T>
    struct serialize_traits<T> {
        template <typename... Args>
        static constexpr auto serialize(const T& val, mut_buf b, Args&&... args) {
            using underlying = veil::underlying<T>;
            return serialize_traits<underlying>::serialize(static_cast<underlying>(val), b, veil::forward<Args>(args)...);
        }
    };

    template <serialize_enum_type T>
    struct deserialize_traits<T> {
        template <typename... Args>
        static constexpr auto deserialize(buf b, T& val, Args&&... args) {
            using underlying = veil::underlying<T>;
            return serialize_traits<underlying>::deserialize(b, static_cast<underlying&>(val), veil::forward<Args>(args)...);
        }
    };
} // namespace lm
