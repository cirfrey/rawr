//// rawr/lib/test.pp.
#pragma once

// TODO: import/include linker section once thats done.
#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.lib.test;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/lib/test.hpp"
#endif

#define RAWR_CHECK(cond) check<get_total_check_count<0, []{}>()>(cond, #cond)
#define RAWR_TEST(...) RAWR_AUTOSIZED_TEST(__VA_ARGS__)

// TODO: also register the test in a linker section.
#define RAWR_AUTOSIZED_TEST(Name) RAWR_AUTOSIZED_TEST_(Name, __COUNTER__)
#define RAWR_AUTOSIZED_TEST_(Name, Counter)                                                           \
    struct RAWR_TEST_CONCAT(rawr_autosized_test_, Counter)                                            \
        : ::rawr::lib::test::autosized_test_suite<RAWR_TEST_CONCAT(rawr_autosized_test_, Counter)>    \
    {                                                                                                 \
        static constexpr char const* name() { return #Name; }                                         \
        constexpr auto run_checks();                                                                  \
    };                                                                                                \
    /* This function is defined out-of-line so that the source location actually properly reflects */ \
    /* the file lines, as it would point to the beggining of the macro if the function body was    */ \
    /* just __VA_ARGS__ expanded after run_checks().                                               */ \
    constexpr auto RAWR_TEST_CONCAT(rawr_autosized_test_, Counter)::run_checks()
#define RAWR_TEST_CONCAT_(a, b) a##b
#define RAWR_TEST_CONCAT(a, b) RAWR_TEST_CONCAT_(a, b)
