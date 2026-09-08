#ifndef RAWR_NO_SOURCE_MAPPING
    #line 3 "rawr/lib/test.hpp"
#endif

#ifdef RAWR_MODULE
    export module rawr.lib.test;
    import rawr.lib.intrin;
    import rawr.lib.integer.raw;
    import rawr.lib.source_location;

    #include "rawr/lib/dist/module.pp"
#else
    #pragma once
    #include "rawr/lib/intrin.hpp"
    #include "rawr/lib/integer/raw.hpp"
    #include "rawr/lib/source_location.hpp"

    #include "rawr/lib/dist/header.pp"
#endif
#include "rawr/lib/detection.pp"
#include "rawr/lib/compiler.pp"
#include "rawr/lib/linker_section.pp"

RAWR_EXPORT namespace rawr::inline lib::inline test::msvc
{
    // MSVC: __declspec(noinline) creates an opaque call boundary without volatile
    // semantics or static initialisation. Safe in nostdlib test binaries.
    template<typename T>
    RAWR_DECLSPEC(noinline) auto no_fold(T v) noexcept -> T { return v; }
}

RAWR_EXPORT namespace rawr::inline lib::inline test
{
    RAWR_GNU(
        template<typename T>
        [[nodiscard]] RAWR_ALWAYS_INLINE
        auto no_fold(T val) -> T {
            asm volatile("" : "+r,m"(val) :: "memory");
            return val;
        }
    )
    RAWR_MSVC(
        template<typename T>
        [[nodiscard]] auto no_fold(T val) -> T
        { return msvc::no_fold(val); }
    )

    struct test_suite_check
    {
        bool cond;
        char const* expr;
        source_location loc;
    };
    using test_suite_check_callback = void(*)(test_suite_check, void* userdata);

    struct test_suite_info {
        char const* name;
        rst name_size;
    };

    struct test_section_entry
    {
        using run_t      = void(*)(test_suite_check_callback, void*);
        using get_info_t = test_suite_info(*)();

        run_t run;
        get_info_t get_info;
    };

    RAWR_LINKER_SECTION_DEFINE(rawr_lib_test_section, section, test_section_entry);

    template <typename T>
    concept TestSuite = requires(T t)
    {
        { T::name()      } -> intrin::ConvertibleTo<char const*>;
        { T::name_size() } -> intrin::ConvertibleTo<rst>;
        { t.run_checks() };
    };

    template <typename Suite>
    struct normal_test_suite
    {
    protected:
        constexpr auto check(
            bool cond,
            source_location const loc = source_location::current()
        ) {
            if(check_callback) check_callback(test_suite_check{
                .cond = cond,
                .expr = nullptr,
                .loc  = loc
            }, userdata);
        }

        template <decltype(sizeof(0)) Size>
        constexpr auto check(
            bool cond,
            char const (&expr)[Size],
            source_location const loc = source_location::current()
        ) {
            if(check_callback) check_callback(test_suite_check{
                .cond = cond,
                .expr = expr,
                .loc  = loc
            }, userdata);
        }

    public:
        test_suite_check_callback check_callback = nullptr;
        void* userdata                           = nullptr;

        static constexpr auto run(test_suite_check_callback callback, void* userdata = nullptr)
        requires TestSuite<Suite>
        {
            Suite suite{ callback, userdata };
            suite.run_checks();
        }

        static constexpr auto get_info() -> test_suite_info
        requires TestSuite<Suite>
        {
            return {
                .name      = Suite::name(),
                .name_size = Suite::name_size()
            };
        }
    };

    enum class meson_code : ru8 {
        ok    = 0,
        skip  = 77, // Test was skipped.
        error = 99, // Signals a hard setup failure.
        // Anything else = Fail.
    };
}
