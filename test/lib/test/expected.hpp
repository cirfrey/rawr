#pragma once

// Compile-time oracle for rawr::lib::test meta-tests.
//
// The runner validates observed callback data against these tables.
// Every entry must stay in sync with the corresponding RAWR_TEST body.
// The canonical source of truth for what each check should produce is the
// check itself — this table is the independently-maintained assertion that
// they agree. Any divergence is a bug in the framework, not in the table.

namespace rawr::inline lib::inline test::testcase_0 {

// ── Shared types ──────────────────────────────────────────────────────────────

// What the runner expects at each check index. Compile-time known.
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

} // namespace rawr::lib::test::meta
