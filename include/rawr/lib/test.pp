#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/test.pp"
#endif
#pragma once

#include "rawr/lib/dist/pp.pp"
#if RAWR_PP_TRANSITIVE_AS_MODULE
    import rawr.lib.test;
#endif
#if RAWR_PP_TRANSITIVE_AS_HEADER
    #include "rawr/lib/test.hpp"
#endif
#include "rawr/lib/linker_section.pp"

#define RAWR_TEST(...)   RAWR_NORMAL_TEST(__VA_ARGS__)
#define RAWR_CHECK(cond) check(cond, #cond)

#define RAWR_NORMAL_TEST(Name) RAWR_NORMAL_TEST_(Name, __COUNTER__)
#define RAWR_NORMAL_TEST_(Name, Counter)                                                                  \
    namespace                                                                                             \
    {                                                                                                     \
        struct RAWR_TEST_CONCAT(rawr_normal_test_, Counter)                                               \
            : ::rawr::lib::test::normal_test_suite<RAWR_TEST_CONCAT(rawr_normal_test_, Counter)>          \
        {                                                                                                 \
            static constexpr auto name()       -> char const* { return #Name; }                           \
            static constexpr auto name_size()                 { char n[] = #Name; return sizeof(n) - 1; } \
                constexpr auto run_checks() -> void;                                                      \
        };                                                                                                \
    }                                                                                                     \
    RAWR_LINKER_SECTION_REGISTER(rawr_lib_test_section, ::rawr::lib::test::section, {                     \
        .run      = RAWR_TEST_CONCAT(rawr_normal_test_, Counter)::run,                                    \
        .get_info = RAWR_TEST_CONCAT(rawr_normal_test_, Counter)::get_info                                \
    });                                                                                                   \
    /* This function is defined out-of-line so that the source location actually properly reflects */     \
    /* the file lines, as it would point to the beggining of the macro if the function body was    */     \
    /* just __VA_ARGS__ expanded after run_checks().                                               */     \
    constexpr auto RAWR_TEST_CONCAT(rawr_normal_test_, Counter)::run_checks() -> void

#define RAWR_TEST_CONCAT_(a, b) a##b
#define RAWR_TEST_CONCAT(a, b) RAWR_TEST_CONCAT_(a, b)
