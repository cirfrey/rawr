//// rawr/arch/x64/atomic.hpp.

#ifdef RAWR_MODULE
    export module rawr.arch.x64.atomic;
    import rawr.lib.integer.base;
    import rawr.lib.sync.base;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/integer/base.hpp"
    #include "rawr/lib/sync/base.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/attributes.pp"

#if RAWR_COMPILER_MSVC
    namespace rawr::arch::x64::atomic::msvc
    {
        extern "C" char    _InterlockedCompareExchange8 (char volatile*,    char,    char);
        extern "C" short   _InterlockedCompareExchange16(short volatile*,   short,   short);
        extern "C" long    _InterlockedCompareExchange  (long volatile*,    long,    long);
        extern "C" __int64 _InterlockedCompareExchange64(__int64 volatile*, __int64, __int64);
        #pragma intrinsic(_InterlockedCompareExchange8)
        #pragma intrinsic(_InterlockedCompareExchange16)
        #pragma intrinsic(_InterlockedCompareExchange)
        #pragma intrinsic(_InterlockedCompareExchange64)

        extern "C" char    _InterlockedExchange8 (char volatile*,    char);
        extern "C" short   _InterlockedExchange16(short volatile*,   short);
        extern "C" long    _InterlockedExchange  (long volatile*,    long);
        extern "C" __int64 _InterlockedExchange64(__int64 volatile*, __int64);
        #pragma intrinsic(_InterlockedExchange8)
        #pragma intrinsic(_InterlockedExchange16)
        #pragma intrinsic(_InterlockedExchange)
        #pragma intrinsic(_InterlockedExchange64)

        extern "C" char    _InterlockedExchangeAdd8 (char volatile*,    char);
        extern "C" short   _InterlockedExchangeAdd16(short volatile*,   short);
        extern "C" long    _InterlockedExchangeAdd  (long volatile*,    long);
        extern "C" __int64 _InterlockedExchangeAdd64(__int64 volatile*, __int64);
        #pragma intrinsic(_InterlockedExchangeAdd8)
        #pragma intrinsic(_InterlockedExchangeAdd16)
        #pragma intrinsic(_InterlockedExchangeAdd)
        #pragma intrinsic(_InterlockedExchangeAdd64)
    }
#endif

#if RAWR_COMPILER_FAMILY_GNU
    namespace rawr::arch::x64::atomic::gnu
    {
        // Explicit mapping, not a numeric cast: rawr::memory_order's
        // ordinal values do not line up with GCC's __ATOMIC_* constants
        // (which reserve a slot for the unused/deprecated `consume`), so
        // casting one to the other silently picks the wrong fence.
        RAWR_ALWAYS_INLINE constexpr auto to_gnu_order(sync::memory_order order) noexcept -> int
        {
            switch (order) {
                case sync::memory_order::relaxed: return __ATOMIC_RELAXED;
                case sync::memory_order::acquire: return __ATOMIC_ACQUIRE;
                case sync::memory_order::release: return __ATOMIC_RELEASE;
                case sync::memory_order::acq_rel: return __ATOMIC_ACQ_REL;
                default:                          return __ATOMIC_SEQ_CST;
            }
        }
    }
#endif

RAWR_EXPORT namespace rawr::arch::x64::atomic
{
    template <
        raint T,
        sync::memory_order Success = sync::memory_order::seq_cst,
        sync::memory_order Failure = sync::memory_order::seq_cst
    >
    RAWR_ALWAYS_INLINE auto cas(T* addr, T& expected, T desired) noexcept -> bool
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return __atomic_compare_exchange_n(
                addr, &expected, desired, /*weak=*/true,
                gnu::to_gnu_order(Success), gnu::to_gnu_order(Failure));
        #elif RAWR_COMPILER_MSVC
            // x64 MSVC intrinsics take no ordering argument: the LOCK-prefixed
            // instruction underneath is a full fence on this ISA regardless of
            // what was asked for (x86/x64 is TSO). Correct for every order,
            // including relaxed — just not as cheap as it could be for relaxed,
            // since these intrinsics are also full compiler barriers.
            using U = base::ruint_exact<sizeof(T)>;
            U prior;
                if constexpr (sizeof(T) == 1) prior = static_cast<U>(msvc::_InterlockedCompareExchange8 (reinterpret_cast<char     volatile*>(addr), static_cast<char>(desired),     static_cast<char>(expected)));
            else if constexpr (sizeof(T) == 2) prior = static_cast<U>(msvc::_InterlockedCompareExchange16(reinterpret_cast<short    volatile*>(addr), static_cast<short>(desired),    static_cast<short>(expected)));
            else if constexpr (sizeof(T) == 4) prior = static_cast<U>(msvc::_InterlockedCompareExchange  (reinterpret_cast<long     volatile*>(addr), static_cast<long>(desired),     static_cast<long>(expected)));
            else if constexpr (sizeof(T) == 8) prior = static_cast<U>(msvc::_InterlockedCompareExchange64(reinterpret_cast<__int64 volatile*>(addr), static_cast<__int64>(desired), static_cast<__int64>(expected)));
            else static_assert(sizeof(T) == 0, "rawr::arch::x64::cas: unsupported width");

            bool const ok = (prior == static_cast<U>(expected));
            if (!ok) expected = static_cast<T>(prior);
            return ok;
        #else
            static_assert(sizeof(T) == 0, "rawr::arch::x64::cas: no known compiler backend");
            return false;
        #endif
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto load(T const* addr) noexcept -> T
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return __atomic_load_n(addr, gnu::to_gnu_order(Order));
        #elif RAWR_COMPILER_MSVC
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
            _ReadWriteBarrier();
            return v;
        #else
            static_assert(sizeof(T) == 0, "rawr::arch::x64::cas: no known compiler backend");
            return T{};
        #endif
    }

    template <raint T, sync::memory_order Order = sync::memory_order::release>
    RAWR_ALWAYS_INLINE auto store(T* addr, T value) noexcept -> void
    {
        #if RAWR_COMPILER_FAMILY_GNU
            __atomic_store_n(addr, value, gnu::to_gnu_order(Order));
        #elif RAWR_COMPILER_MSVC
            // Conservative on purpose: route every order through the full-fence
            // exchange rather than trying to hand-tune relaxed/release on TSO.
            // Correct for all orders, costs a touch more than a bare relaxed
            // store would need — see the note on cas() above for the same tradeoff.
            using U = base::ruint_exact<sizeof(T)>;
                if constexpr (sizeof(T) == 1)  msvc::_InterlockedExchange8 (reinterpret_cast<char    volatile*>(addr), static_cast<char>(value));
            else if constexpr (sizeof(T) == 2) msvc::_InterlockedExchange16(reinterpret_cast<short   volatile*>(addr), static_cast<short>(value));
            else if constexpr (sizeof(T) == 4) msvc::_InterlockedExchange  (reinterpret_cast<long    volatile*>(addr), static_cast<long>(value));
            else if constexpr (sizeof(T) == 8) msvc::_InterlockedExchange64(reinterpret_cast<__int64 volatile*>(addr), static_cast<__int64>(value));
            else static_assert(sizeof(T) == 0, "rawr::arch::x64::store: unsupported width");
        #else
            static_assert(sizeof(T) == 0, "rawr::arch::x64::store: no known compiler backend");
        #endif
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto fetch_add(T* addr, T delta) noexcept -> T
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return __atomic_fetch_add(addr, delta, gnu::to_gnu_order(Order));
        #elif RAWR_COMPILER_MSVC
            using U = base::ruint_exact<sizeof(T)>;
                 if constexpr (sizeof(T) == 1) return static_cast<T>(msvc::_InterlockedExchangeAdd8 (reinterpret_cast<char    volatile*>(addr), static_cast<char>(delta)));
            else if constexpr (sizeof(T) == 2) return static_cast<T>(msvc::_InterlockedExchangeAdd16(reinterpret_cast<short   volatile*>(addr), static_cast<short>(delta)));
            else if constexpr (sizeof(T) == 4) return static_cast<T>(msvc::_InterlockedExchangeAdd  (reinterpret_cast<long    volatile*>(addr), static_cast<long>(delta)));
            else if constexpr (sizeof(T) == 8) return static_cast<T>(msvc::_InterlockedExchangeAdd64(reinterpret_cast<__int64 volatile*>(addr), static_cast<__int64>(delta)));
            else { static_assert(sizeof(T) == 0, "rawr::arch::x64::fetch_add: unsupported width"); return T{}; }
        #else
            static_assert(sizeof(T) == 0, "rawr::arch::x64::fetch_add: no known compiler backend");
            return T{};
        #endif
    }

    template <raint T, sync::memory_order Order = sync::memory_order::seq_cst>
    RAWR_ALWAYS_INLINE auto exchange(T* addr, T value) noexcept -> T
    {
        #if RAWR_COMPILER_FAMILY_GNU
            return __atomic_exchange_n(addr, value, gnu::to_gnu_order(Order));
        #elif RAWR_COMPILER_MSVC
                 if constexpr (sizeof(T) == 1) return static_cast<T>(msvc::_InterlockedExchange8 (reinterpret_cast<char    volatile*>(addr), static_cast<char>(value)));
            else if constexpr (sizeof(T) == 2) return static_cast<T>(msvc::_InterlockedExchange16(reinterpret_cast<short   volatile*>(addr), static_cast<short>(value)));
            else if constexpr (sizeof(T) == 4) return static_cast<T>(msvc::_InterlockedExchange  (reinterpret_cast<long    volatile*>(addr), static_cast<long>(value)));
            else if constexpr (sizeof(T) == 8) return static_cast<T>(msvc::_InterlockedExchange64(reinterpret_cast<__int64 volatile*>(addr), static_cast<__int64>(value)));
            else { static_assert(sizeof(T) == 0, "rawr::arch::x64::exchange: unsupported width"); return T{}; }
        #else
            static_assert(sizeof(T) == 0, "rawr::arch::x64::exchange: no known compiler backend");
            return T{};
        #endif
    }
}
