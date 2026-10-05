#include "rawr/lib/test.pp"

// Meta-tests for rawr::lib::test — TU 2.
// Expected results in expected.hpp. Comments reference expected[] indices.
//
// All four suites in this file must appear in the linker section alongside
// the four from checks.cpp. A section collision between TUs would manifest
// as duplicated_test or check_count_mismatch before any cond comparison.

// ── dummy2 ────────────────────────────────────────────────────────────────────
// Different name and check count (3) from dummy (5).
// The count difference makes a collision immediately visible.
RAWR_TEST(dummy2)
{
    RAWR_CHECK(true);           // 0: true
    RAWR_CHECK(true);           // 1: true
    RAWR_CHECK(false);          // 2: false — intentional
}

// ── all_fail ──────────────────────────────────────────────────────────────────
// Every check produces cond=false. Verifies that multiple consecutive failures
// are captured independently with correct per-check expr strings.
// If only the first failure is recorded and subsequent ones are dropped,
// check_count_mismatch or check_expr_mismatch catches it.
RAWR_TEST(all_fail)
{
    RAWR_CHECK(false);          // 0: false, "false"
    RAWR_CHECK(1 == 2);         // 1: false, "1 == 2"
    RAWR_CHECK(2 > 5);          // 2: false, "2 > 5"
}

// ── single_check ──────────────────────────────────────────────────────────────
// One check. Edge case for runner loop bounds and count == 1.
RAWR_TEST(single_check)
{
    RAWR_CHECK(true);           // 0: true
}
