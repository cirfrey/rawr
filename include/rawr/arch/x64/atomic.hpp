//// rawr/arch/x64/atomic.hpp.

#ifdef RAWR_MODULE
    export module rawr.arch.x64.atomic;
    import rawr.lib.integer.base;
    import rawr.lib.integer.raw;
    import rawr.lib.sync.base;
    import rawr.lib.detection;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/sync/base.hpp"
    #include "rawr/lib/detection.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"
#include "rawr/lib/detection.pp"
#include "rawr/lib/compiler.pp"

RAWR_EXPORT namespace rawr::arch::x64::atomic::msvc
{
    // RAWR_IS_64BIT is used instead of RAWR_ARCH_X64 because those intrinsics are also supported on arm64.

    // MSVC intrinsics often need an exact type match, which can be a problem if you do signed char instead of
    // char or things like using int instead of long, even if they are the same size in that architecture.
    using rchar = RAWR_MSVC_OR(char, rs8);
    using rlong = RAWR_MSVC_OR(long, rs32);

    RAWR_MSVC_INTRIN(1, _InterlockedCompareExchange8,  (rchar volatile*, rchar, rchar) -> rchar);
    RAWR_MSVC_INTRIN(1, _InterlockedCompareExchange16, (rs16  volatile*, rs16,  rs16)  -> rs16);
    RAWR_MSVC_INTRIN(1, _InterlockedCompareExchange,   (rlong volatile*, rlong, rlong) -> rlong);
    RAWR_MSVC_INTRIN(1, _InterlockedCompareExchange64, (rs64  volatile*, rs64,  rs64)  -> rs64);

    RAWR_MSVC_INTRIN(1,             _InterlockedExchange8,  (rchar volatile*, rchar) -> rchar);
    RAWR_MSVC_INTRIN(1,             _InterlockedExchange16, (rs16  volatile*, rs16)  -> rs16);
    RAWR_MSVC_INTRIN(1,             _InterlockedExchange,   (rlong volatile*, rlong) -> rlong);
    RAWR_MSVC_INTRIN(RAWR_IS_64BIT, _InterlockedExchange64, (rs64  volatile*, rs64)  -> rs64);

    RAWR_MSVC_INTRIN(1,             _InterlockedExchangeAdd8,  (rchar volatile*, rchar) -> rchar);
    RAWR_MSVC_INTRIN(1,             _InterlockedExchangeAdd16, (rs16  volatile*, rs16)  -> rs16);
    RAWR_MSVC_INTRIN(1,             _InterlockedExchangeAdd,   (rlong volatile*, rlong) -> rlong);
    RAWR_MSVC_INTRIN(RAWR_IS_64BIT, _InterlockedExchangeAdd64, (rs64  volatile*, rs64)  -> rs64);

    RAWR_MSVC_INTRIN(RAWR_ARCH_FAMILY_X86, _ReadWriteBarrier, () -> void);
}

RAWR_EXPORT namespace rawr::arch::x64::atomic::gnu
{
    // Explicit mapping, not a numeric cast: rawr::memory_order's
    // ordinal values do not line up with GCC's __ATOMIC_* constants
    // (which reserve a slot for the unused/deprecated `consume`), so
    // casting one to the other silently picks the wrong fence.
    RAWR_ALWAYS_INLINE constexpr auto to_gnu_order(sync::memory_order order) noexcept -> int
    RAWR_GNU({
        switch (order) {
            case sync::memory_order::relaxed: return __ATOMIC_RELAXED;
            case sync::memory_order::acquire: return __ATOMIC_ACQUIRE;
            case sync::memory_order::release: return __ATOMIC_RELEASE;
            case sync::memory_order::acq_rel: return __ATOMIC_ACQ_REL;
            default:                          return __ATOMIC_SEQ_CST;
        }
    });

    template <raint Type>
    RAWR_ALWAYS_INLINE constexpr auto atomic_compare_exchange_n(
        Type* ptr,
        Type* expected,
        Type  desired,
        bool  weak,
        int   success,
        int   failure
    ) -> bool RAWR_GNU({
        return ::__atomic_compare_exchange_n(ptr, expected, desired, weak, success, failure);
    });

    template <raint Type> RAWR_ALWAYS_INLINE constexpr auto atomic_load_n    (Type* addr,             int memorder) -> Type RAWR_GNU({ return ::__atomic_load_n(addr, memorder); });
    template <raint Type> RAWR_ALWAYS_INLINE constexpr auto atomic_store_n   (Type* addr, Type val,   int memorder) -> void RAWR_GNU({ ::__atomic_store_n(addr, val, memorder); });
    template <raint Type> RAWR_ALWAYS_INLINE constexpr auto atomic_fetch_add (Type* addr, Type delta, int memorder) -> Type RAWR_GNU({ return ::__atomic_fetch_add(addr, delta, memorder); });
    template <raint Type> RAWR_ALWAYS_INLINE constexpr auto atomic_exchange_n(Type* addr, Type val,   int memorder) -> Type RAWR_GNU({ return ::__atomic_exchange_n(addr, val, memorder); });
}

RAWR_EXPORT namespace rawr::arch::x64::atomic
{
    template <
        raint T,
        sync::memory_order Success = sync::memory_order::seq_cst,
        sync::memory_order Failure = sync::memory_order::seq_cst
    >
    RAWR_ALWAYS_INLINE auto cas(T* addr, T& expected, T desired) noexcept -> bool
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert(false);
            return false;
        } else if constexpr(this_compiler.is_family_gnu()) {
            return gnu::atomic_compare_exchange_n(
                addr, &expected, desired, /*weak=*/true,
                gnu::to_gnu_order(Success), gnu::to_gnu_order(Failure));
        } else if constexpr(this_compiler.is_msvc()){
            // x64 MSVC intrinsics take no ordering argument: the LOCK-prefixed
            // instruction underneath is a full fence on this ISA regardless of
            // what was asked for (x86/x64 is TSO). Correct for every order,
            // including relaxed — just not as cheap as it could be for relaxed,
            // since these intrinsics are also full compiler barriers.
            using U = ruint_of<T>;
            U prior;
                 if constexpr (sizeof(T) == 1) prior = static_cast<U>(msvc::_InterlockedCompareExchange8 (reinterpret_cast<      rs8   volatile*>(addr), static_cast<      rs8>(desired),   static_cast<      rs8>(expected)));
            else if constexpr (sizeof(T) == 2) prior = static_cast<U>(msvc::_InterlockedCompareExchange16(reinterpret_cast<      rs16  volatile*>(addr), static_cast<      rs16>(desired),  static_cast<      rs16>(expected)));
            else if constexpr (sizeof(T) == 4) prior = static_cast<U>(msvc::_InterlockedCompareExchange  (reinterpret_cast<msvc::rlong volatile*>(addr), static_cast<msvc::rlong>(desired), static_cast<msvc::rlong>(expected)));
            else if constexpr (sizeof(T) == 8) prior = static_cast<U>(msvc::_InterlockedCompareExchange64(reinterpret_cast<      rs64  volatile*>(addr), static_cast<      rs64>(desired),  static_cast<      rs64>(expected)));
            else static_assert(sizeof(T) == 0, "rawr::arch::x64::cas: unsupported width");

            bool const ok = (prior == static_cast<U>(expected));
            if (!ok) expected = static_cast<T>(prior);
            return ok;
        }
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto load(T const* addr) noexcept -> T
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert(false);
            return T{};
        } else if constexpr(this_compiler.is_family_gnu()) {
            return gnu::atomic_load_n(addr, gnu::to_gnu_order(Order));
        } else if constexpr(this_compiler.is_msvc()){
            // A naturally-aligned load/store of a machine word is already
            // atomic on x64 without any interlocked op — the cache-coherency
            // protocol guarantees a cache line can't tear mid-transfer. Only
            // a compiler barrier is needed to stop reordering across it, and
            // for seq_cst specifically, TSO still allows StoreLoad reordering
            // between an earlier store and this load, so seq_cst needs a real
            // fence.
            // No extra fence needed here for any order, including seq_cst:
            // store() below unconditionally routes through a LOCK-prefixed
            // exchange, which is what actually closes the StoreLoad-reordering
            // gap TSO allows. As long as every store on this word goes through
            // that path, a plain load + compiler barrier is sufficient here.
            T v = *static_cast<T const volatile*>(addr);
            msvc::_ReadWriteBarrier();
            return v;
        }
    }

    template <raint T, sync::memory_order Order = sync::memory_order::release>
    RAWR_ALWAYS_INLINE auto store(T* addr, T value) noexcept -> void
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert(false);
        } else if constexpr(this_compiler.is_family_gnu()) {
            gnu::atomic_store_n(addr, value, gnu::to_gnu_order(Order));
        } else if constexpr(this_compiler.is_msvc()){
            // Conservative on purpose: route every order through the full-fence
            // exchange rather than trying to hand-tune relaxed/release on TSO.
            // Correct for all orders, costs a touch more than a bare relaxed
            // store would need — see the note on cas() above for the same tradeoff.
                 if constexpr (sizeof(T) == 1) msvc::_InterlockedExchange8 (reinterpret_cast<      rs8   volatile*>(addr), static_cast<      rs8>(value));
            else if constexpr (sizeof(T) == 2) msvc::_InterlockedExchange16(reinterpret_cast<      rs16  volatile*>(addr), static_cast<      rs16>(value));
            else if constexpr (sizeof(T) == 4) msvc::_InterlockedExchange  (reinterpret_cast<msvc::rlong volatile*>(addr), static_cast<msvc::rlong>(value));
            else if constexpr (sizeof(T) == 8) msvc::_InterlockedExchange64(reinterpret_cast<      rs64  volatile*>(addr), static_cast<      rs64>(value));
            else static_assert(sizeof(T) == 0, "rawr::arch::x64::store: unsupported width");
        }
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto fetch_add(T* addr, T delta) noexcept -> T
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert(false);
            return T{};
        } else if constexpr(this_compiler.is_family_gnu()) {
            return gnu::atomic_fetch_add(addr, delta, gnu::to_gnu_order(Order));
        } else if constexpr(this_compiler.is_msvc()){
                 if constexpr (sizeof(T) == 1) return static_cast<T>(msvc::_InterlockedExchangeAdd8 (reinterpret_cast<      rs8   volatile*>(addr), static_cast<      rs8>(delta)));
            else if constexpr (sizeof(T) == 2) return static_cast<T>(msvc::_InterlockedExchangeAdd16(reinterpret_cast<      rs16  volatile*>(addr), static_cast<      rs16>(delta)));
            else if constexpr (sizeof(T) == 4) return static_cast<T>(msvc::_InterlockedExchangeAdd  (reinterpret_cast<msvc::rlong volatile*>(addr), static_cast<msvc::rlong>(delta)));
            else if constexpr (sizeof(T) == 8) return static_cast<T>(msvc::_InterlockedExchangeAdd64(reinterpret_cast<      rs64  volatile*>(addr), static_cast<      rs64>(delta)));
            else { static_assert(sizeof(T) == 0, "rawr::arch::x64::fetch_add: unsupported width"); return T{}; }
        }
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto exchange(T* addr, T value) noexcept -> T
    {
        if constexpr(!this_arch.is_x64()) {
            static_assert(false);
            return T{};
        } else if constexpr(this_compiler.is_family_gnu()) {
            return gnu::atomic_exchange_n(addr, value, gnu::to_gnu_order(Order));
        } else if constexpr(this_compiler.is_msvc()){
                 if constexpr (sizeof(T) == 1) return static_cast<T>(msvc::_InterlockedExchange8 (reinterpret_cast<      rs8   volatile*>(addr), static_cast<      rs8>(value)));
            else if constexpr (sizeof(T) == 2) return static_cast<T>(msvc::_InterlockedExchange16(reinterpret_cast<      rs16  volatile*>(addr), static_cast<      rs16>(value)));
            else if constexpr (sizeof(T) == 4) return static_cast<T>(msvc::_InterlockedExchange  (reinterpret_cast<msvc::rlong volatile*>(addr), static_cast<msvc::rlong>(value)));
            else if constexpr (sizeof(T) == 8) return static_cast<T>(msvc::_InterlockedExchange64(reinterpret_cast<      rs64  volatile*>(addr), static_cast<      rs64>(value)));
            else { static_assert(sizeof(T) == 0, "rawr::arch::x64::exchange: unsupported width"); return T{}; }
        }
    }
}
