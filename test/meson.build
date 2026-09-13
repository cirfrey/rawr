default_test_runner = files('runner.cpp')

test_groups = {
    'lib.test' : {
        'files': files('lib/test/checks.cpp', 'lib/test/checks2.cpp'),
        'runner': files('lib/test/runner.cpp'), # Requires a special runner.
    }
}

foreach name, group : test_groups
    # We actually build the test with -flto to prove the claims that linker_section survives it.
    # Lets put our money where our mouth is.
    test_cpp_args  = ['-fno-builtin', '-flto']
    test_link_args = ['-fno-builtin', '-flto']

    # For module tests we also only require the actually imported modules. Helps keeps us honest (instead of using rawr_dep directly):
    # - Every module really does declare its own requires. If it doesn't compile, there's a missing import in a .pp file.
    # - Build times scale better.
    # - If some other unrelated module fails to compile, we can still run this test.
    runner_srcs = group.get('runner', default_test_runner)
    test_srcs   = group['files']
    scan_tests = run_command(
        python, dir_scripts / 'testdeps.py',
        '--root', dir_include,
        test_srcs,
        check: true,
    )
    test_module_deps = []
    foreach m : scan_tests.stdout().strip().split('\n')
        if m != ''
            if not mod_deps.has_key(m)
                error('testdeps: unknown module @0@ from @1@'.format(m, ', '.join(test_srcs)))
            endif
            test_module_deps += mod_deps[m]
        endif
    endforeach

    runner_lib = ''
    scan_runner = run_command(
        python, dir_scripts / 'testdeps.py',
        '--root', dir_include,
        runner_srcs,
        check: true,
    )
    runner_module_deps = []
    foreach m : scan_runner.stdout().strip().split('\n')
        if m != ''
            runner_module_deps += mod_deps[m]
        endif
    endforeach

    test(name,
        executable(
            'test_module.' + name,
            link_whole: [
                static_library(
                    'test_module_runner.' + name,
                    sources: runner_srcs,
                    dependencies: runner_module_deps + [freestanding_dep],
                    cpp_args: ['-DRAWR_PP_TRANSITIVE_AS_MODULE', '-DRAWR_TESTMODE_MODULE'] + test_cpp_args,
                ),
                static_library(
                    'test_module_staticlib.' + name,
                    sources: test_srcs,
                    dependencies: test_module_deps + [freestanding_dep],
                    cpp_args: ['-DRAWR_PP_TRANSITIVE_AS_MODULE', '-DRAWR_TESTMODE_MODULE'] + test_cpp_args,
                ),
            ],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'module',
        timeout: 30,
    )

    test(name,
        executable(
            'test_header.' + name,
            link_whole: [
                static_library(
                    'test_header_runner.' + name,
                    sources: runner_srcs,
                    dependencies: runner_module_deps + [freestanding_dep],
                    cpp_args: ['-DRAWR_PP_TRANSITIVE_AS_HEADER', '-DRAWR_TESTMODE_HEADER'] + test_cpp_args,
                ),
                static_library(
                    'test_header_staticlib.' + name,
                    sources: test_srcs,
                    dependencies: [rawr_headeronly_dep, freestanding_dep],
                    cpp_args: ['-DRAWR_PP_TRANSITIVE_AS_HEADER', '-DRAWR_TESTMODE_HEADER'] + test_cpp_args,
                ),
            ],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'header',
        timeout: 30,
    )

    test(name,
        executable(
            'test_amalgam.' + name,
            link_whole: [
                static_library(
                    'test_amalgam_runner.' + name,
                    sources: runner_srcs,
                    dependencies: runner_module_deps + [freestanding_dep],
                    cpp_args: ['-DRAWR_TESTMODE_AMALGAM'] + test_cpp_args,
                ),
                static_library(
                    'test_amalgam_staticlib.' + name,
                    sources: test_srcs,
                    dependencies: [rawr_amalgam_dep, freestanding_dep],
                    cpp_args: ['-DRAWR_TESTMODE_AMALGAM'] + test_cpp_args,
                ),
            ],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'amalgam',
        timeout: 30,
    )
endforeach
