import rawr.lib.test;
#include "rawr/lib/main.pp"

// Custom runner for rawr::lib::test meta-tests.
//
// This runner cannot rely on the generic runner because the generic runner
// itself depends on the invariants being verified here. It verifies:
//   - Both suites (dummy, dummy2) appear in the linker section
//   - No unexpected suites appear
//   - Each suite produces exactly the expected number of checks
//   - Each check's cond matches the expected value at that index
//
// TODO: use rawr.lib.fmt for diagnostic output on mismatch.

import rawr.lib.integer.raw;
import rawr.platform.linux;
import rawr.lib.hash.fnv1a;

struct test
{
    rawr::ru64 name_hash = 0u;
    bool       executed  = false;

    struct check { bool cond; };

    rawr::ru32  expected_check_count = 0u;
    check*      expected_checks      = nullptr;
    rawr::ru32  observed_check_idx   = 0u;
    check*      observed_checks      = nullptr;
};

enum class status_type : rawr::ru8
{
    ok = 0,
    unexpected_test,                  // suite appeared with no entry in tests[]
    duplicated_test,
    not_all_expected_tests_executed,  // a registered test never ran
    check_count_mismatch,             // observed check count != expected
    check_cond_mismatch,              // observed cond != expected at some index
};

RAWR_MAIN_NOCTX
{
    using namespace rawr::lib::hash::fnv1a::literals;
    using namespace rawr::platform::linux::x64;

    // dummy (checks.cpp)
    constexpr rawr::ru32 dummy_check_count = 5u;
    test::check dummy_expected[dummy_check_count] = {
        {true},   // 0: RAWR_CHECK(true)
        {false},  // 1: RAWR_CHECK(false)       intentional fail
        {true},   // 2: RAWR_CHECK(1 == 1)
        {false},  // 3: RAWR_CHECK(1 == 2)      intentional fail
        {true},   // 4: RAWR_CHECK(1 + 1 == 2)
    };
    test::check dummy_observed[dummy_check_count];

    // dummy2 (checks2.cpp)
    constexpr rawr::ru32 dummy2_check_count = 3u;
    test::check dummy2_expected[dummy2_check_count] = {
        {true},   // 0: RAWR_CHECK(true)
        {true},   // 1: RAWR_CHECK(true)
        {false},  // 2: RAWR_CHECK(false)       intentional fail
    };
    test::check dummy2_observed[dummy2_check_count];

    test tests[] = {
        {
            .name_hash            = "dummy"_fnv1a64,
            .expected_check_count = dummy_check_count,
            .expected_checks      = dummy_expected,
            .observed_checks      = dummy_observed,
        },
        {
            .name_hash            = "dummy2"_fnv1a64,
            .expected_check_count = dummy2_check_count,
            .expected_checks      = dummy2_expected,
            .observed_checks      = dummy2_observed,
        },
    };

    status_type status = status_type::ok;

    for (auto const& suite : rawr::lib::test::section) {
        auto const suite_info = suite.get_info();

        auto* this_test = [&]() -> test* {
            auto const hashed_name = rawr::fnv1a::hash64(suite_info.name, suite_info.name_size);
            for (auto& t : tests) { if (t.name_hash == hashed_name) { return &t; } }
            return nullptr;
        }();

        if (this_test == nullptr)
        {
            syscall::write(stdout, "Unexpected test \"").discard();
            syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
            syscall::write(stdout, "\"\n").discard();

            if (status == status_type::ok) status = status_type::unexpected_test;
            continue;
        }

        if (this_test->executed)
        {
            syscall::write(stdout, "Duplicated test \"").discard();
            syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
            syscall::write(stdout, "\"\n").discard();

            if (status == status_type::ok) status = status_type::duplicated_test;
            continue;
        }

        suite.run([](rawr::lib::test::test_suite_check c, void* ud) noexcept -> void {
            auto& t = *static_cast<test*>(ud);
            if (t.observed_check_idx < t.expected_check_count)
            { t.observed_checks[t.observed_check_idx++] = { c.cond }; }
            else { ++t.observed_check_idx; } // overrun: keep counting, caught after run
        }, this_test);

        this_test->executed = true;

        // Count mismatch - catches both overrun and early-exit underrun.
        if (this_test->observed_check_idx != this_test->expected_check_count)
        {
            syscall::write(stdout, "waaaa\n").discard();
            if (status == status_type::ok) status = status_type::check_count_mismatch;
        }

        // Cond mismatch - compare only the checks that ran on both sides.
        auto const compare_up_to = this_test->observed_check_idx < this_test->expected_check_count
            ? this_test->observed_check_idx
            : this_test->expected_check_count;

        for (rawr::ru32 i = 0u; i < compare_up_to; ++i)
        {
            if (this_test->observed_checks[i].cond == this_test->expected_checks[i].cond) { continue; }

            syscall::write(stdout, "waaaa2\n").discard();
            if (status == status_type::ok) status = status_type::check_cond_mismatch;
        }
    }

    for (auto& t : tests)
    {
        if (t.executed) { continue; }

        syscall::write(stdout, "waaaa3\n").discard();
        if (status == status_type::ok) status = status_type::not_all_expected_tests_executed;
    }

    syscall::exit(static_cast<rawr::ru8>(status));
}
