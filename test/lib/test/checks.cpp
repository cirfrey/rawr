#include "rawr/lib/test.pp"

// Meta-tests for rawr::lib::test infrastructure -- dummy suite.
//
// This suite intentionally includes both passing and failing checks.
// The custom runner knows the expected cond for each check by index and
// verifies the observed sequence matches. A cond=false here is not a bug:
// it tests that the framework correctly captures and reports false cond.
//
// Check index table (must stay in sync with runner.cpp dummy_expected[]):
//   0  true   -- RAWR_CHECK(true)         literal true
//   1  false  -- RAWR_CHECK(false)        literal false
//   2  true   -- RAWR_CHECK(1 == 1)       arithmetic identity
//   3  false  -- RAWR_CHECK(1 == 2)       arithmetic mismatch
//   4  true   -- RAWR_CHECK(1 + 1 == 2)  compound expression

RAWR_TEST(dummy)
{
    RAWR_CHECK(true);
    RAWR_CHECK(false);
    RAWR_CHECK(1 == 1);
    RAWR_CHECK(1 == 2);
    RAWR_CHECK(1 + 1 == 2);
}
