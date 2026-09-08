// rawr test runner
// Iterates rawr_lib_test_section, runs each suite through its check callback,
// accumulates results, prints a summary, and exits with 0 (all pass) or 1.
//
// This binary is nostdlib. Output goes through rawr::platform::linux::syscall::write.
// TODO: platform dispatch once rawr supports more than Linux.

import rawr.lib.test;
#include "rawr/lib/main.pp"

// TODO: gate on RAWR_PLATFORM_LINUX once platform dispatch exists.
// For now this is the only output backend.
import rawr.platform.linux;
import rawr.lib.fmt;

struct suite_state {
    unsigned passed = 0u;
    unsigned failed = 0u;
};

RAWR_MAIN_NOCTX
{
    [[maybe_unused]] unsigned total_passed = 0u;
    [[maybe_unused]] unsigned total_failed = 0u;
    [[maybe_unused]] unsigned suites_run   = 0u;

    for (auto const& entry : rawr::lib::test::section) {
        suite_state state{};

        entry.run([](rawr::lib::test::test_suite_check c, void* ud) noexcept -> void {
            auto& s = *static_cast<suite_state*>(ud);
            if (c.cond) { ++s.passed; }
            else        { ++s.failed; }
        }, &state);

        total_passed += state.passed;
        total_failed += state.failed;
        ++suites_run;
    }

    rawr::platform::linux::x64::syscall::exit(total_passed == suites_run ? 0 : 1);
}
