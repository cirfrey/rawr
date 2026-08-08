#pragma once

#include <cstddef>   // std::size_t
#include <type_traits>

namespace lm {

// ============================================================================
// Forward declaration
// ============================================================================
template <typename Storage, std::size_t Offset, std::size_t Width, typename FieldType = Storage>
class bitfield_ref;

// ============================================================================
// bitfield_ref – proxy that gives typed read/write access to a bit range
// ============================================================================
template <typename Storage, std::size_t Offset, std::size_t Width, typename FieldType>
class bitfield_ref {
    // Ensure the field fits inside the storage type
    static_assert(Offset + Width <= sizeof(Storage) * 8,
                  "Bitfield exceeds storage width");
    static_assert(Width > 0, "Bitfield width must be > 0");

    // The storage integer is either mutable or const, depending on constness
    Storage* storage;

public:
    // Construct from a reference to the underlying storage
    explicit constexpr bitfield_ref(Storage& s) noexcept : storage(&s) {}

    // ------------------------------------------------------------------
    // Implicit conversion to FieldType (read)
    // ------------------------------------------------------------------
    constexpr operator FieldType() const noexcept {
        return static_cast<FieldType>(
            (*storage >> Offset) & mask_value
        );
    }

    // ------------------------------------------------------------------
    // Assignment from FieldType (write) – only when storage is non‑const
    // ------------------------------------------------------------------
    constexpr bitfield_ref& operator=(FieldType val) noexcept
        requires (!std::is_const_v<Storage>)
    {
        Storage raw = static_cast<Storage>(val);
        *storage = (*storage & ~write_mask) | ((raw & mask_value) << Offset);
        return *this;
    }

    // ------------------------------------------------------------------
    // Compound assignment (example) – you can add more operators as needed
    // ------------------------------------------------------------------
    constexpr bitfield_ref& operator+=(FieldType val) noexcept
        requires (!std::is_const_v<Storage>)
    {
        return *this = static_cast<FieldType>(*this + val);
    }

    constexpr bitfield_ref& operator-=(FieldType val) noexcept
        requires (!std::is_const_v<Storage>)
    {
        return *this = static_cast<FieldType>(*this - val);
    }

    // (Add &, |, ^, shifts if desired…)

private:
    // Bitmask that covers exactly the field's bits inside the storage
    static constexpr Storage mask_value =
        (Width == sizeof(Storage) * 8)
            ? static_cast<Storage>(~Storage{0})
            : static_cast<Storage>((Storage{1} << Width) - 1);

    // Pre‑shifted mask for clearing the field during write
    static constexpr Storage write_mask = mask_value << Offset;
};

// ============================================================================
// bitfield – base class that holds the storage and provides accessors
// ============================================================================
// TODO: Needs to enforce StorageType is an unsigned integer type (e.g., via static_assert or concepts)
template <typename StorageType>
struct bitfield {
    StorageType _storage{};
    using storage_type = StorageType;

    constexpr operator storage_type() const noexcept { return _storage; }
    constexpr auto as_storage() noexcept -> StorageType& { return _storage; }
    constexpr auto as_storage() const noexcept -> const StorageType& { return _storage; }

    // Non‑const version – returns a mutable proxy
    template <std::size_t Offset, std::size_t Width, typename FieldType = StorageType>
    constexpr auto _bitfield() noexcept {
        return bitfield_ref<StorageType, Offset, Width, FieldType>(_storage);
    }

    // Const version – returns a read‑only proxy (assignment disabled)
    template <std::size_t Offset, std::size_t Width, typename FieldType = StorageType>
    constexpr auto _bitfield() const noexcept {
        return bitfield_ref<const StorageType, Offset, Width, FieldType>(_storage);
    }
};

} // namespace lm

// ============================================================================
// Convenience macros for declaring bitfields inside a struct derived from
// bitfield<StorageType>
// ============================================================================

// Untyped bitfield – FieldType defaults to the storage type (e.g., u8)
#define LM_BITFIELD(name, offset, width)                                  \
    constexpr auto name() noexcept {                                      \
        return this->template _bitfield<offset, width>();                 \
    }                                                                     \
    constexpr auto name() const noexcept {                                \
        return this->template _bitfield<offset, width>();                 \
    }

// Typed bitfield – allows enum classes, rich_flags, etc.
#define LM_BITFIELD_TYPED(name, offset, width, FieldType)                 \
    constexpr auto name() noexcept {                                      \
        return this->template _bitfield<offset, width, FieldType>();      \
    }                                                                     \
    constexpr auto name() const noexcept {                                \
        return this->template _bitfield<offset, width, FieldType>();      \
    }
