// Tests for RAWR_TAGGED_UNION.
//
// Compile-time tests use the constexpr-bool pattern:
//   constexpr bool r = fn(); RAWR_CHECK(r);
// The constexpr binding forces compile-time evaluation of fn().
// fn() returns result && is_consteval() — so a false result means either
// the logic is wrong OR the function wasn't actually evaluated at compile time.
//
// Destructor tracking uses a type that increments an int* on destruction.
// The int lives on the constexpr evaluation stack (C++20), so destructor
// calls are observable within constant expressions.

import rawr.lib.intrin.base;
#include "rawr/lib/tagged_union.pp"
#include "rawr/lib/test.pp"
#include "rawr/lib/compiler.pp"

namespace
{

// ── Test types ────────────────────────────────────────────────────────────────

// Tracks destructor invocations via a pointer to an int counter.
// Move constructor nulls the pointer so the moved-from object's destructor
// is a no-op — this is the correct pattern for types held in tagged unions.
struct destructor_tracker {
    int* call_count = nullptr;
    constexpr explicit destructor_tracker(int* c = nullptr) noexcept : call_count{c} {}
    constexpr destructor_tracker(destructor_tracker const& o)  noexcept : call_count{o.call_count} {}
    constexpr destructor_tracker(destructor_tracker&&       o) noexcept : call_count{o.call_count} { o.call_count = nullptr; }
    constexpr destructor_tracker& operator=(destructor_tracker const& o) noexcept { call_count = o.call_count; return *this; }
    constexpr destructor_tracker& operator=(destructor_tracker&&      o) noexcept { call_count = o.call_count; o.call_count = nullptr; return *this; }
    constexpr ~destructor_tracker() noexcept { if (call_count) ++(*call_count); }
    constexpr bool operator==(destructor_tracker const& o) const noexcept { return call_count == o.call_count; }
};

// ── Test unions ───────────────────────────────────────────────────────────────

RAWR_TAGGED_UNION(simple,       (i, int),                (f, float)              );
RAWR_TAGGED_UNION(trio,         (a, int),  (b, float),   (c, long)               );
RAWR_TAGGED_UNION(solo,         (only, int)                                       );
RAWR_TAGGED_UNION(int_variants, (first, int),            (second, int)           );
RAWR_TAGGED_UNION(tracked,      (value, destructor_tracker), (other, int)        );

// ── Static assertions — type-level properties ─────────────────────────────────

// valueless is always the first enumerator (value 0).
static_assert(static_cast<int>(simple::tag_t::valueless) == 0);
static_assert(static_cast<int>(solo::tag_t::valueless)   == 0);

// make is noexcept when the target type's constructor is noexcept.
// int() is implicitly noexcept; float() likewise.
static_assert(noexcept(simple::make::i(0)));
static_assert(noexcept(simple::make::f(0.0f)));
static_assert(noexcept(solo::make::only(0)));

// ── Compile-time test functions ───────────────────────────────────────────────

constexpr auto ct_make_sets_tag() -> bool {
    auto v = simple::make::i(42);
    return v.tag == simple::tag_t::i && rawr::intrin::is_consteval();
}

constexpr auto ct_query_active_nonnull() -> bool {
    auto v = simple::make::i(42);
    return v.i() != nullptr && *v.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_query_inactive_null() -> bool {
    auto v = simple::make::i(42);
    return v.f() == nullptr && rawr::intrin::is_consteval();
}

constexpr auto ct_query_by_value_match() -> bool {
    auto v = simple::make::i(42);
    return v.i(42) && rawr::intrin::is_consteval();
}

constexpr auto ct_query_by_value_no_match() -> bool {
    auto v = simple::make::i(42);
    return !v.i(99) && rawr::intrin::is_consteval();
}

constexpr auto ct_query_by_value_wrong_tag() -> bool {
    auto v = simple::make::i(42);
    // tag is i, querying f — must return false regardless of value
    return !v.f(0.0f) && rawr::intrin::is_consteval();
}

constexpr auto ct_copy_construct() -> bool {
    auto v1 = simple::make::i(42);
    auto v2 = v1;
    return v2.tag == simple::tag_t::i && *v2.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_move_construct() -> bool {
    auto v1 = simple::make::i(42);
    auto v2 = static_cast<simple&&>(v1);
    return v2.tag == simple::tag_t::i && *v2.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_copy_assign_same_tag() -> bool {
    auto v1 = simple::make::i(42);
    auto v2 = simple::make::i(0);
    v2 = v1;
    return v2.tag == simple::tag_t::i && *v2.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_copy_assign_cross_tag() -> bool {
    auto src = simple::make::f(3.14f);
    auto dst = simple::make::i(42);
    dst = src; // i → f: destroys int, constructs float
    return dst.tag == simple::tag_t::f
        && dst.i() == nullptr
        && dst.f() != nullptr
        && rawr::intrin::is_consteval();
}

constexpr auto ct_move_assign_same_tag() -> bool {
    auto v1 = simple::make::i(42);
    auto v2 = simple::make::i(0);
    v2 = static_cast<simple&&>(v1);
    return v2.tag == simple::tag_t::i && *v2.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_move_assign_cross_tag() -> bool {
    auto dst = simple::make::i(42);
    dst = simple::make::f(1.0f); // i → f via move assign
    return dst.tag == simple::tag_t::f
        && dst.i() == nullptr
        && dst.f() != nullptr
        && rawr::intrin::is_consteval();
}

constexpr auto ct_self_copy_assign() -> bool {
    auto v = simple::make::i(42);
#if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif
    v = v;
#if RAWR_COMPILER_CLANG || RAWR_COMPILER_GCC
    #pragma GCC diagnostic pop
#endif
    return v.tag == simple::tag_t::i && *v.i() == 42 && rawr::intrin::is_consteval();
}

constexpr auto ct_self_move_assign() -> bool {
    auto v = simple::make::i(42);
    v = static_cast<simple&&>(v); // implementation returns *this immediately
    return v.tag == simple::tag_t::i && *v.i() == 42 && rawr::intrin::is_consteval();
}

// ── Destructor-tracking compile-time functions ────────────────────────────────
// In C++20, destructor calls during constexpr evaluation are observable
// through changes to local int variables via pointer.
// Important: local variables are destroyed AFTER the return value is computed,
// so returning an expression captures the counter state before scope-exit dtors.

constexpr auto ct_destructor_on_scope_exit() -> bool {
    int count = 0;
    {
        auto v = tracked::make::value(&count); (void)v;
    } // v destroyed here — destructor_tracker fires, count = 1
    return count == 1 && rawr::intrin::is_consteval();
}

constexpr auto ct_destructor_on_cross_assign() -> bool {
    int count = 0;
    auto v = tracked::make::value(&count);      // count = 0
    v = tracked::make::other(42);               // cross-tag: destroys tracked, count = 1
    // v now holds 'other'; its destructor does nothing to count
    return count == 1 && rawr::intrin::is_consteval();
    // After return: v (holding other) goes out of scope — int dtor, no effect
}

constexpr auto ct_no_destructor_on_same_assign() -> bool {
    int count = 0;
    auto v1 = tracked::make::value(&count);
    auto v2 = tracked::make::value(&count);
    v1 = v2; // same tag: copy-assign, no destroy() called, count stays 0
    return count == 0 && rawr::intrin::is_consteval();
    // After return: v2 destroyed (count=1), v1 destroyed (count=2)
    // But return value was already computed when count was 0
}

constexpr auto ct_destructor_count_on_copy_assign_cross() -> bool {
    // Cross-tag copy assignment destroys exactly one old value,
    // not two (destroy + reconstruct must only fire destroy once).
    int count = 0;
    auto v = tracked::make::value(&count);
    auto src = simple::make::i(0); (void)src; // unrelated, no effect on count
    // Manually test via tracked:
    auto t1 = tracked::make::value(&count);  // count = 0
    t1 = tracked::make::other(99);           // cross: count = 1
    t1 = tracked::make::other(100);          // same tag (other→other): count = 1
    return count == 1 && rawr::intrin::is_consteval();
}

} // namespace

// ── Compile-time tests ────────────────────────────────────────────────────────

RAWR_TEST(tagged_union.ct_make_sets_tag)
{
    constexpr bool r = ct_make_sets_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_query_active_nonnull)
{
    constexpr bool r = ct_query_active_nonnull();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_query_inactive_null)
{
    constexpr bool r = ct_query_inactive_null();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_query_by_value_match)
{
    constexpr bool r = ct_query_by_value_match();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_query_by_value_no_match)
{
    constexpr bool r = ct_query_by_value_no_match();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_query_by_value_wrong_tag)
{
    constexpr bool r = ct_query_by_value_wrong_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_copy_construct)
{
    constexpr bool r = ct_copy_construct();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_move_construct)
{
    constexpr bool r = ct_move_construct();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_copy_assign_same_tag)
{
    constexpr bool r = ct_copy_assign_same_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_copy_assign_cross_tag)
{
    constexpr bool r = ct_copy_assign_cross_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_move_assign_same_tag)
{
    constexpr bool r = ct_move_assign_same_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_move_assign_cross_tag)
{
    constexpr bool r = ct_move_assign_cross_tag();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_self_copy_assign)
{
    constexpr bool r = ct_self_copy_assign();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_self_move_assign)
{
    constexpr bool r = ct_self_move_assign();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_destructor_on_scope_exit)
{
    constexpr bool r = ct_destructor_on_scope_exit();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_destructor_on_cross_assign)
{
    constexpr bool r = ct_destructor_on_cross_assign();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_no_destructor_on_same_assign)
{
    constexpr bool r = ct_no_destructor_on_same_assign();
    RAWR_CHECK(r);
}

RAWR_TEST(tagged_union.ct_destructor_count_on_copy_assign_cross)
{
    constexpr bool r = ct_destructor_count_on_copy_assign_cross();
    RAWR_CHECK(r);
}

// ── Runtime tests ─────────────────────────────────────────────────────────────
// run_checks() is called at runtime via callback; these verify the
// runtime code paths without is_consteval() gating.

RAWR_TEST(tagged_union.rt_basic_construction)
{
    auto vi = simple::make::i(42);
    auto vf = simple::make::f(1.5f);

    RAWR_CHECK(vi.tag == simple::tag_t::i);
    RAWR_CHECK(vi.i() != nullptr);
    RAWR_CHECK(*vi.i() == 42);
    RAWR_CHECK(vi.f() == nullptr);

    RAWR_CHECK(vf.tag == simple::tag_t::f);
    RAWR_CHECK(vf.f() != nullptr);
    RAWR_CHECK(vf.i() == nullptr);
}

RAWR_TEST(tagged_union.rt_three_variants)
{
    auto va = trio::make::a(1);
    auto vb = trio::make::b(2.0f);
    auto vc = trio::make::c(3L);

    RAWR_CHECK(va.a() != nullptr); RAWR_CHECK(*va.a() == 1);
    RAWR_CHECK(va.b() == nullptr);
    RAWR_CHECK(va.c() == nullptr);

    RAWR_CHECK(vb.a() == nullptr);
    RAWR_CHECK(vb.b() != nullptr); RAWR_CHECK(*vb.b() == 2.0f);
    RAWR_CHECK(vb.c() == nullptr);

    RAWR_CHECK(vc.a() == nullptr);
    RAWR_CHECK(vc.b() == nullptr);
    RAWR_CHECK(vc.c() != nullptr); RAWR_CHECK(*vc.c() == 3L);
}

RAWR_TEST(tagged_union.rt_single_variant)
{
    // Edge case: only one variant besides valueless.
    // ruint_capable<2> = uint8_t for the enum — verify it compiles and works.
    auto v = solo::make::only(7);
    RAWR_CHECK(v.tag == solo::tag_t::only);
    RAWR_CHECK(v.only() != nullptr);
    RAWR_CHECK(*v.only() == 7);
}

RAWR_TEST(tagged_union.rt_same_type_variants)
{
    // Two variants of the same underlying type — distinguished only by tag.
    // A union collision would cause first == second or wrong null results.
    auto vf = int_variants::make::first(1);
    auto vs = int_variants::make::second(2);

    RAWR_CHECK(vf.tag != vs.tag);
    RAWR_CHECK(*vf.first()  == 1);
    RAWR_CHECK(vf.second()  == nullptr);
    RAWR_CHECK(*vs.second() == 2);
    RAWR_CHECK(vs.first()   == nullptr);
}

RAWR_TEST(tagged_union.rt_query_by_value)
{
    auto v = simple::make::i(42);
    RAWR_CHECK( v.i(42));   // active, correct value
    RAWR_CHECK(!v.i(0));    // active, wrong value
    RAWR_CHECK(!v.f(42.0f)); // wrong tag entirely
}

RAWR_TEST(tagged_union.rt_copy_assign_replaces_tag)
{
    auto dst = simple::make::i(42);
    auto src = simple::make::f(1.0f);

    dst = src;

    RAWR_CHECK(dst.tag == simple::tag_t::f);
    RAWR_CHECK(dst.i() == nullptr);
    RAWR_CHECK(dst.f() != nullptr);
}

RAWR_TEST(tagged_union.rt_destructor_tracking)
{
    // Verify destructor is called exactly once at scope exit, and exactly
    // once during cross-tag assignment (on the old value, not the new).
    int scope_count = 0;
    {
        auto v = tracked::make::value(&scope_count);
        RAWR_CHECK(scope_count == 0); // not yet destroyed
    }
    RAWR_CHECK(scope_count == 1); // destroyed at scope exit

    int assign_count = 0;
    {
        auto v = tracked::make::value(&assign_count);
        v = tracked::make::other(99); // cross-tag: old value destroyed
        RAWR_CHECK(assign_count == 1);
        // v now holds 'other'; scope exit destroys int, no effect on count
    }
    RAWR_CHECK(assign_count == 1); // still 1, not 2
}
