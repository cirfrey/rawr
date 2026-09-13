// Custom runner for rawr::lib::test meta-tests.
//
// Cannot rely on the generic runner — the generic runner depends on the
// invariants verified here. Data declarations live in expected.hpp.
// This file is pure validation logic.
//
// Verified invariants:
//   - All expected suites appear in the section (not_all_expected_tests_executed)
//   - No unexpected suites appear     (unexpected_test)
//   - No suite name appears twice     (duplicated_test)
//   - Observed check count == expected (check_count_mismatch)
//   - Observed cond == expected cond  (check_cond_mismatch)
//   - Observed expr == expected expr  (check_expr_mismatch)
//   - Observed loc.file is non-null, non-empty (check_loc_invalid)
//   - Observed loc.line is non-zero           (check_loc_invalid)

#if defined(RAWR_TESTMODE_MODULE)
    import rawr.lib.test;
    import rawr.lib.integer.raw;
    import rawr.platform.linux;
    import rawr.lib.hash.fnv1a;
    #include "rawr/lib/main.pp"
#elif defined(RAWR_TESTMODE_HEADER)
    #include "rawr/lib/test.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/platform/linux.hpp"
    #include "rawr/lib/hash/fnv1a.hpp"
    #include "rawr/lib/main.pp"
#elif defined(RAWR_TESTMODE_AMALGAM)
    #include "rawr.amalgam.hpp"
#endif

struct expected_check {
    bool        cond;
    const char* expr;   // nullptr means the no-expr overload was used
};

// What the callback actually records at runtime.
struct observed_check {
    bool        cond;
    const char* expr;
    const char* loc_file;   // loc.file_name() — validated non-null, non-empty
    unsigned    loc_line;   // loc.line()      — validated non-zero
};

// Array count helper — avoids sizeof boilerplate at definition sites.
template<typename T, unsigned N>
constexpr auto count_of(T const (&)[N]) noexcept -> unsigned { return N; }

// ── Suite: dummy (checks.cpp) ─────────────────────────────────────────────────
// Basic cond capture: literal true/false and arithmetic expressions.
// Intentional failures at indices 1 and 3 verify false cond is captured.
inline constexpr expected_check dummy_expected[] = {
    {true,  "true"},
    {false, "false"},
    {true,  "1 == 1"},
    {false, "1 == 2"},
    {true,  "1 + 1 == 2"},
};

// ── Suite: early_exit (checks.cpp) ───────────────────────────────────────────
// run_checks() returns early after index 2 via 'if (!check(...)) return;'.
// Indices 3 and 4 exist in the source but must not appear in observed data.
// Runner expected_count is 3, not 5 — verifies underrun is detected.
inline constexpr expected_check early_exit_expected[] = {
    {true,  "1 == 1"},
    {true,  "2 == 2"},
    {false, "gate"},
    // indices 3, 4 never observed — deliberately absent from this table
};

// ── Suite: no_expr (checks.cpp) ───────────────────────────────────────────────
// Uses check(cond) — the no-expr overload. Verifies expr == nullptr in the
// callback; if the overloads are wired incorrectly, expr would be non-null.
inline constexpr expected_check no_expr_expected[] = {
    {true,  nullptr},
    {false, nullptr},
};

// ── Suite: expressions (checks.cpp) ──────────────────────────────────────────
// Validates RAWR_CHECK expr stringification for non-trivial expressions:
// operator precedence, logical operators, unary NOT.
// A stringification bug would cause expr mismatch, not cond mismatch.
inline constexpr expected_check expressions_expected[] = {
    {true,  "1 + 2 * 3 == 7"},
    {true,  "true && true"},
    {false, "false || false"},
    {true,  "!false"},
};

// ── Suite: dummy2 (checks2.cpp) ───────────────────────────────────────────────
// Second TU, different name, different count (3 vs 5).
// A linker section collision between dummy and dummy2 shows as check_count_mismatch
// before cond_mismatch — the count difference is the early signal.
inline constexpr expected_check dummy2_expected[] = {
    {true,  "true"},
    {true,  "true"},
    {false, "false"},
};

// ── Suite: all_fail (checks2.cpp) ────────────────────────────────────────────
// All checks produce cond=false. Verifies that multiple consecutive failures
// are each independently captured with correct cond and distinct expr strings.
inline constexpr expected_check all_fail_expected[] = {
    {false, "false"},
    {false, "1 == 2"},
    {false, "2 > 5"},
};

// ── Suite: single_check (checks2.cpp) ────────────────────────────────────────
// Exactly one check. Edge case for loop bounds and count == 1.
inline constexpr expected_check single_check_expected[] = {
    {true, "true"},
};

enum class status_type : rawr::ru8
{
    ok = 0,
    unexpected_test,
    duplicated_test,
    not_all_expected_tests_executed,
    check_count_mismatch,
    check_cond_mismatch,
    check_expr_mismatch,
    check_loc_invalid,
};

struct suite_manifest {
    const char*           name;
    rawr::ru64            name_hash;
    bool                  executed       = false;
    rawr::ru32            expected_count;
    rawr::ru32            observed_idx   = 0u;
    expected_check const* expected;
    observed_check*       observed;
};

static auto streq(const char* a, const char* b) noexcept -> bool {
    if (a == b)   return true;   // also handles both-null
    if (!a || !b) return false;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == *b;
}

RAWR_MAIN_NOCTX
{
    using namespace rawr::lib::hash::fnv1a::literals;
    using namespace rawr::platform::linux::x64;

    // ── Observed buffers ──────────────────────────────────────────────────────
    // Sized from expected arrays in expected.hpp via count_of().
    // Uninitialized — the callback fills exactly observed_idx entries.

    observed_check dummy_observed        [count_of(dummy_expected)];
    observed_check early_exit_observed   [count_of(early_exit_expected)];
    observed_check no_expr_observed      [count_of(no_expr_expected)];
    observed_check expressions_observed  [count_of(expressions_expected)];
    observed_check dummy2_observed       [count_of(dummy2_expected)];
    observed_check all_fail_observed     [count_of(all_fail_expected)];
    observed_check single_check_observed [count_of(single_check_expected)];

    // ── Manifest ──────────────────────────────────────────────────────────────
    // One entry per RAWR_TEST in checks.cpp / checks2.cpp.
    // Order here does not need to match link order — lookup is by hash.

    suite_manifest manifests[] = {
        { "dummy",        "dummy"_fnv1a64,
          false, rawr::ru32(count_of(dummy_expected)),        0u,
          dummy_expected,        dummy_observed        },
        { "early_exit",   "early_exit"_fnv1a64,
          false, rawr::ru32(count_of(early_exit_expected)),   0u,
          early_exit_expected,   early_exit_observed   },
        { "no_expr",      "no_expr"_fnv1a64,
          false, rawr::ru32(count_of(no_expr_expected)),      0u,
          no_expr_expected,      no_expr_observed      },
        { "expressions",  "expressions"_fnv1a64,
          false, rawr::ru32(count_of(expressions_expected)),  0u,
          expressions_expected,  expressions_observed  },
        { "dummy2",       "dummy2"_fnv1a64,
          false, rawr::ru32(count_of(dummy2_expected)),       0u,
          dummy2_expected,       dummy2_observed       },
        { "all_fail",     "all_fail"_fnv1a64,
          false, rawr::ru32(count_of(all_fail_expected)),     0u,
          all_fail_expected,     all_fail_observed     },
        { "single_check", "single_check"_fnv1a64,
          false, rawr::ru32(count_of(single_check_expected)), 0u,
          single_check_expected, single_check_observed },
    };

    status_type status = status_type::ok;

    // ── Run ───────────────────────────────────────────────────────────────────

    for (auto const& suite : rawr::lib::test::section) {
        auto const suite_info = suite.get_info();

        auto* m = [&]() -> suite_manifest* {
            auto const h = rawr::fnv1a::hash64(suite_info.name, suite_info.name_size);
            for (auto& m : manifests) { if (m.name_hash == h) { return &m; } }
            return nullptr;
        }();

        if (!m) {
            syscall::write(stdout, "unexpected: \"").discard();
            syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
            syscall::write(stdout, "\"\n").discard();
            if (status == status_type::ok) status = status_type::unexpected_test;
            continue;
        }

        if (m->executed) {
            syscall::write(stdout, "duplicated: \"").discard();
            syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
            syscall::write(stdout, "\"\n").discard();
            if (status == status_type::ok) status = status_type::duplicated_test;
            continue;
        }

        suite.run([](rawr::lib::test::test_suite_check c, void* ud) noexcept -> void {
            auto& m = *static_cast<suite_manifest*>(ud);
            if (m.observed_idx < m.expected_count)
                m.observed[m.observed_idx++] = {
                    c.cond,
                    c.expr,
                    c.loc.file,
                    static_cast<unsigned>(c.loc.line),
                };
            else
                ++m.observed_idx; // overrun: keep counting, caught below
        }, m);

        m->executed = true;

        // ── Count ─────────────────────────────────────────────────────────────

        if (m->observed_idx != m->expected_count) {
            syscall::write(stdout, "count mismatch: ").discard();
            syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
            syscall::write(stdout, "\n").discard();
            if (status == status_type::ok) status = status_type::check_count_mismatch;
        }

        // ── Per-check validation ──────────────────────────────────────────────
        // Compare only the checks present on both sides.

        auto const compare_up_to =
            m->observed_idx < m->expected_count
                ? m->observed_idx
                : m->expected_count;

        for (rawr::ru32 i = 0u; i < compare_up_to; ++i) {
            auto const& obs = m->observed[i];
            auto const& exp = m->expected[i];

            if (obs.cond != exp.cond) {
                syscall::write(stdout, "cond mismatch: ").discard();
                syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
                syscall::write(stdout, "\n").discard();
                if (status == status_type::ok) status = status_type::check_cond_mismatch;
            }

            if (!streq(obs.expr, exp.expr)) {
                syscall::write(stdout, "expr mismatch: ").discard();
                syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
                syscall::write(stdout, "\n").discard();
                if (status == status_type::ok) status = status_type::check_expr_mismatch;
            }

            if (!obs.loc_file || obs.loc_file[0] == '\0' || obs.loc_line == 0u) {
                syscall::write(stdout, "loc invalid: ").discard();
                syscall::write(stdout, suite_info.name, suite_info.name_size).discard();
                syscall::write(stdout, "\n").discard();
                if (status == status_type::ok) status = status_type::check_loc_invalid;
            }
        }
    }

    // ── All-executed check ────────────────────────────────────────────────────

    for (auto& m : manifests) {
        if (m.executed) { continue; }

        unsigned name_size = 0;
        while(m.name[name_size++]);
        syscall::write(stdout, "not executed: ").discard();
        syscall::write(stdout, m.name, name_size - 1).discard();
        syscall::write(stdout, "\n").discard();
        if (status == status_type::ok) status = status_type::not_all_expected_tests_executed;
    }

    syscall::exit(static_cast<rawr::ru8>(status));
}
