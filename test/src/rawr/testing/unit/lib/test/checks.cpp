#include "rawr/lib/test.pp"

// Meta-tests for rawr::lib::test — TU 1.
// Expected results in expected.hpp. Comments reference expected[] indices.

// ── dummy ─────────────────────────────────────────────────────────────────────
RAWR_TEST(dummy)
{
    RAWR_CHECK(true);           // 0: true
    RAWR_CHECK(false);          // 1: false — intentional, verifies false cond capture
    RAWR_CHECK(1 == 1);         // 2: true
    RAWR_CHECK(1 == 2);         // 3: false — intentional, verifies false arithmetic
    RAWR_CHECK(1 + 1 == 2);    // 4: true
}

// ── early_exit ────────────────────────────────────────────────────────────────
// Verifies that returning from run_checks() stops further check execution.
// The runner expects exactly 3 observed checks. Observing 4 or 5 means
// the early-exit did not fire. Observing fewer than 3 means it fired too early.
RAWR_TEST(early_exit)
{
    RAWR_CHECK(1 == 1);                         // 0: observed
    RAWR_CHECK(2 == 2);                         // 1: observed
    if (!check(false, "gate")) { return; }      // 2: observed, fails, exits
    RAWR_CHECK(3 == 3);                         // must NOT be observed
    RAWR_CHECK(4 == 4);                         // must NOT be observed
}

// ── no_expr ───────────────────────────────────────────────────────────────────
// Uses check(cond) directly — the single-argument overload that takes no
// expr string. Verifies the runner receives expr == nullptr for these checks.
// If the overload dispatch is broken, expr would be non-null (e.g. garbage).
RAWR_TEST(no_expr)
{
    check(true);    // 0: cond=true,  expr=nullptr
    check(false);   // 1: cond=false, expr=nullptr
}

// ── expressions ───────────────────────────────────────────────────────────────
// Verifies RAWR_CHECK stringifies non-trivial expressions correctly.
// Operator precedence (1 + 2 * 3), logical ops, unary NOT.
// A stringification defect shows as check_expr_mismatch, not check_cond_mismatch.
RAWR_TEST(expressions)
{
    RAWR_CHECK(1 + 2 * 3 == 7);    // 0: true,  "1 + 2 * 3 == 7"
    RAWR_CHECK(true && true);       // 1: true,  "true && true"
    RAWR_CHECK(false || false);     // 2: false, "false || false" — intentional
    RAWR_CHECK(!false);             // 3: true,  "!false"
}
