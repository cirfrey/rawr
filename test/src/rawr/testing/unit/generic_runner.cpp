// Generic test runner — used by all test groups that do not specify a custom runner.
//
// Output format (print-as-you-go, no failure storage):
//
//   suite.name
//     FAIL: "expr"  file.cpp:42
//   [FAIL] 2 passed | 1 failed
//
//   other.suite
//   [PASS] 5 checks
//
//   ──────────────────────────────────────────────
//   2 suites | 7 passed | 1 failed
//
// Failures are printed by the callback as they occur. The result line
// follows after run() returns. No heap, no failure buffer.

#define RAWR_AMALGAM_NOSTDLIB 1
import rawr.lib.test;
import rawr.lib.integer.raw;
import rawr.platform.linux;
import rawr.nostdlib;
#include "rawr/lib/main.pp"


// ── Output helpers ────────────────────────────────────────────────────────────
// Minimal formatting until rawr::lib::fmt is ported.

namespace {

using namespace rawr::platform::linux::x64;

auto out(const char* s) noexcept -> void {
    if (!s) return;
    unsigned n = 0;
    while (s[n]) ++n;
    if (n) syscall::write(fd_stdout, s, rawr::ru32(n)).discard();
}

auto out_uint(unsigned n) noexcept -> void {
    char buf[10];
    int  i = 10;
    if (!n) buf[--i] = '0';
    else while (n) { buf[--i] = char('0' + n % 10u); n /= 10u; }
    syscall::write(fd_stdout, buf + i, rawr::ru32(10 - i)).discard();
}

// ── Suite runner state ────────────────────────────────────────────────────────

struct suite_state {
    unsigned passed = 0u;
    unsigned failed = 0u;
};

auto check_callback(rawr::lib::test::test_suite_check c, void* ud) noexcept -> void {
    auto& s = *static_cast<suite_state*>(ud);

    if (c.cond) { ++s.passed; return; }

    ++s.failed;
    out("  FAIL: ");
    if (c.expr) { out("\""); out(c.expr); out("\"  "); }
    else         { out("(no expr)  "); }
    out(c.loc.file);
    out(":");
    out_uint(static_cast<unsigned>(c.loc.line));
    out("\n");
}

} // namespace

// ── Entry point ───────────────────────────────────────────────────────────────

RAWR_MAIN_NOCTX
{
    unsigned total_passed = 0u;
    unsigned total_failed = 0u;
    unsigned suites_run   = 0u;

    for (auto const& entry : rawr::lib::test::section) {
        auto const info = entry.get_info();
        suite_state state{};

        syscall::write(fd_stdout, info.name, info.name_size).discard();
        out("\n");

        entry.run(&check_callback, &state);

        if (state.failed == 0u) {
            out("[PASS] ");
            out_uint(state.passed);
            out(" check");
            if (state.passed != 1u) out("s");
        } else {
            out("[FAIL] ");
            out_uint(state.passed);
            out(" passed | ");
            out_uint(state.failed);
            out(" failed");
        }
        out("\n\n");

        total_passed += state.passed;
        total_failed += state.failed;
        ++suites_run;
    }

    out("──────────────────────────────────────────────\n");
    out_uint(suites_run);
    out(suites_run == 1u ? " suite  | " : " suites | ");
    out_uint(total_passed);
    out(" passed | ");
    out_uint(total_failed);
    out(" failed\n");

    syscall::exit(static_cast<rawr::ru8>(total_failed == 0u ? 0u : 1u));
}
