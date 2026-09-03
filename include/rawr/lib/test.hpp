//// rawr/lib/test.hpp.

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

#if RAWR_COMPILER_MSVC
    namespace rawr::inline lib::inline test::msvc
    {
        // MSVC: __declspec(noinline) creates an opaque call boundary without volatile
        // semantics or static initialisation. Safe in nostdlib test binaries.
        template<typename T>
        __declspec(noinline) auto no_fold(T v) noexcept -> T { return v; }
    }
#endif

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

    struct test_suite_result {
        char const* name;
        ru32 total_check_count;
        ru32 check_count;
        test_suite_check* checks;
    };
    using test_suite_callback = void(*)(test_suite_result const&, void* userdata);

    template <typename T>
    concept test_suite = requires(T t)
    {
        { T::name() } -> intrin::convertible_to<char const*>;
        { t.run_checks() };
    };

    template <typename Suite>
    struct autosized_test_suite
    {
    private:
        RAWR_GCC_PRAGMA(GCC diagnostic push)
        RAWR_GCC_PRAGMA(GCC diagnostic ignored "-Wnon-template-friend")
        RAWR_GCC_PRAGMA(GCC diagnostic ignored "-Wsfinae-incomplete") // What's this and why is it needed?
        template<int N> struct Flag { friend constexpr auto adl_flag(Flag<N>); };
        template<int N> struct Writer { friend constexpr auto adl_flag(Flag<N>) { return true; } };
        RAWR_GCC_PRAGMA(GCC diagnostic pop)

    protected:
        template <int N, auto U>
        static constexpr int get_total_check_count() {
            if constexpr (requires { adl_flag(Flag<N>{}); }) return get_total_check_count<N + 1, U>();
            else return N;
        }

        template <int C, int Size>
        constexpr auto check(
            bool cond,
            char const (&expr)[Size],
            source_location const loc = source_location::current(),
            Writer<C> = {}
        ) {
            auto check_result = test_suite_check{
                .cond = cond,
                .expr = expr,
                .loc  = loc
            };
            if(check_callback) check_callback(check_result, userdata);
        }

    public:
        test_suite_check_callback check_callback = nullptr;
        void* userdata                           = nullptr;

        // If you want to get only the final counts and whatnot.
        static constexpr auto run(test_suite_callback callback, void* userdata = nullptr)
        requires test_suite<Suite>
        {
            constexpr auto total_check_count = get_total_check_count<0, []{}>();

            test_suite_check checks[total_check_count > 0 ? total_check_count : 1] = {};
            auto result = test_suite_result {
                .name = Suite::name(),
                .total_check_count = total_check_count,
                .check_count = 0,
                .checks = checks
            };
            Suite suite{
                [](auto check_result, void* result_){
                    auto& result = *(test_suite_result*)result_;
                    result.checks[result.check_count++] = check_result;
                },
                &result
            };
            suite.run_checks();

            callback(result, userdata);
        }

        // If you want per-check injection.
        static constexpr auto run_interactive(test_suite_check_callback callback, void* userdata = nullptr)
        requires test_suite<Suite>
        {
            Suite suite{ callback, userdata };
            suite.run_checks();
        }

        static constexpr auto get_info() -> test_suite_result
        requires test_suite<Suite>
        {
            return {
                .name = Suite::name(),
                .total_check_count = get_total_check_count<0, []{}>(),
                .check_count = 0,
                .checks = nullptr
            };
        }
    };
}
