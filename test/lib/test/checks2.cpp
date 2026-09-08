#include "rawr/lib/test.pp"

// Meta-tests for rawr::lib::test infrastructure -- dummy2 suite.
//
// Separate TU from checks.cpp. Verifies that two RAWR_TEST definitions
// in different translation units both appear in the linker section and do
// not overwrite each other. The intentionally different check count (3 vs 5)
// makes a collision immediately visible as a count mismatch in the runner.
//
// Check index table (must stay in sync with runner.cpp dummy2_expected[]):
//   0  true   -- RAWR_CHECK(true)   pass
//   1  true   -- RAWR_CHECK(true)   pass, independent from dummy checks
//   2  false  -- RAWR_CHECK(false)  intentional fail in second TU

RAWR_TEST(dummy2)
{
    RAWR_CHECK(true);
    RAWR_CHECK(true);
    RAWR_CHECK(false);
}
