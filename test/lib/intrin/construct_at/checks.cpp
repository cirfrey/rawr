// Tests for rawr::intrin::construct_at.
//
// Compile-time tests use a constexpr function + constexpr binding pattern:
//
//   constexpr auto fn() -> bool {
//       // ... do the thing ...
//       return result == expected && rawr::intrin::is_consteval();
//   }
//   RAWR_TEST(...) { constexpr bool r = fn(); RAWR_CHECK(r); }
//
// The constexpr binding forces compile-time evaluation of fn(). If construct_at
// cannot operate in a constant expression, this is a BUILD ERROR — caught before
// any test runs. If it can, is_consteval() returns true inside fn(), making the
// return value both the correctness check and the compile-time proof.
// RAWR_CHECK verifies the result at runtime as a constant.
//
// Runtime tests directly exercise the runtime path, including a volatile-input
// variant that defeats any residual constant folding.

#if defined(RAWR_TESTMODE_MODULE)
    import rawr.lib.test;
    import rawr.lib.intrin.base;
    import rawr.lib.intrin.mem;
    import rawr.lib.intrin.construct_at;
    import rawr.platform.linux;
    #include "rawr/lib/test.pp"
#elif defined(RAWR_TESTMODE_HEADER)
    #include "rawr/lib/test.hpp"
    #include "rawr/lib/intrin/base.hpp"
    #include "rawr/lib/intrin/mem.hpp"
    #include "rawr/lib/intrin/construct_at.hpp"
    #include "rawr/lib/test.pp"
#elif defined(RAWR_TESTMODE_AMALGAM)
    #include "rawr.amalgam.hpp"
#endif

namespace
{
    extern "C" void* memcpy(void* dst, const void* src, unsigned long n)
    {
        return rawr::intrin::mem::soft::memcpy(
            reinterpret_cast<unsigned char*>(dst),
            reinterpret_cast<unsigned char const*>(src),
            n
        );
    }

    // ── Test types ────────────────────────────────────────────────────────────────

    struct test_obj
    {
        int value;
        constexpr test_obj(int v) noexcept : value{v} {}
    };

    struct multi_obj
    {
        int x, y;
        constexpr multi_obj(int a, int b) : x{a}, y{b} {}
    };

    // Union storage that does not automatically construct or destroy its payload.
    // The user manages the lifetime explicitly via construct_at.
    template<typename T>
    union uninitialized
    {
        struct {} empty;
        T value;
        constexpr uninitialized() : empty{} {}
        constexpr ~uninitialized() {}
    };

} // namespace

// ── noexcept property ─────────────────────────────────────────────────────────
// test_obj(int) is noexcept; construct_at must be noexcept too.
// This is a type-system property — stays as static_assert, not a runtime check.
static_assert(noexcept(rawr::intrin::construct_at( static_cast<test_obj*>(nullptr), 42)));


// ── Compile-time tests ────────────────────────────────────────────────────────
// Each constexpr binding is simultaneously a compile-time assertion (build error
// if construct_at cannot be used in a constant expression) and a runtime check
// (RAWR_CHECK verifies the constant is what we expect).

RAWR_TEST(construct_at.ct_struct)
{
    constexpr bool result = []() -> bool {
        uninitialized<test_obj> s;
        auto* p = rawr::intrin::construct_at(&s.value, 42);
        return p->value == 42 && rawr::intrin::is_consteval();
    }();
    RAWR_CHECK(result);
}

RAWR_TEST(construct_at.ct_primitive)
{
    constexpr bool result = []() -> bool {
        uninitialized<int> s;
        auto* p = rawr::intrin::construct_at(&s.value, 123);
        return *p == 123 && rawr::intrin::is_consteval();
    }();
    RAWR_CHECK(result);
}

RAWR_TEST(construct_at.ct_pointer_identity)
{
    constexpr bool result = []() -> bool {
        uninitialized<test_obj> s;
        auto* p = rawr::intrin::construct_at(&s.value, 99);
        return p == &s.value && rawr::intrin::is_consteval();
    }();
    RAWR_CHECK(result);
}

RAWR_TEST(construct_at.ct_multi_arg)
{
    constexpr bool result = []() -> bool {
        uninitialized<multi_obj> s;
        auto* p = rawr::intrin::construct_at(&s.value, 10, 20);
        return p->x == 10 && p->y == 20 && rawr::intrin::is_consteval();
    }();
    RAWR_CHECK(result);
}

RAWR_TEST(construct_at.rt_struct)
{
    uninitialized<test_obj> s;
    auto* p = rawr::intrin::construct_at(&s.value, 42);
    RAWR_CHECK(p == &s.value);
    RAWR_CHECK(p->value == 42);
}

RAWR_TEST(construct_at.rt_primitive)
{
    uninitialized<int> s;
    auto* p = rawr::intrin::construct_at(&s.value, 123);
    RAWR_CHECK(p == &s.value);
    RAWR_CHECK(*p == 123);
}

// volatile input forces runtime evaluation — eliminates any residual folding.
RAWR_TEST(construct_at.rt_volatile)
{
    volatile int input = 42;
    uninitialized<int> s;
    auto* p = rawr::intrin::construct_at(&s.value, static_cast<int>(input));
    RAWR_CHECK(p == &s.value);
    RAWR_CHECK(*p == 42);
}

RAWR_TEST(construct_at.rt_multi_arg)
{
    uninitialized<multi_obj> s;
    auto* p = rawr::intrin::construct_at(&s.value, 10, 20);
    RAWR_CHECK(p->x == 10);
    RAWR_CHECK(p->y == 20);
}
