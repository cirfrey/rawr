cppm_testdir = 'src'/'rawr/testing'

test_generated_dir = meson.current_build_dir() / 'generated' / 'testing'
hpp_testdir        = test_generated_dir / 'hpp'
amalgam_testdir    = test_generated_dir / 'amalgam'

default_unit_test_runner = [cppm_testdir/'unit/generic_runner.cpp']

unit_test_groups = {
    'lib.tagged_union':        { 'files': [cppm_testdir/'unit/lib/tagged_union/checks.cpp'] },
    'lib.intrin.construct_at': { 'files': [cppm_testdir/'unit/lib/intrin/construct_at/checks.cpp'] },
    'lib.test' : {
        'files': [
            cppm_testdir/'unit/lib/test/checks.cpp',
            cppm_testdir/'unit/lib/test/checks2.cpp'
        ],
        'runner': [cppm_testdir/'unit/lib/test/runner.cpp'], # Requires a special runner.
    },
}

test_hpp_gen     = {}
test_amalgam_gen = {}

foreach name, group : unit_test_groups
    # Generate the header-mode version of every test source into the build tree.
    test_cppm_srcs   = group['files']
    runner_cppm_srcs = group.get('runner', default_unit_test_runner)

    foreach t : runner_cppm_srcs + test_cppm_srcs
        if test_hpp_gen.has_key(t)
            continue
        endif

        rel = t.replace(cppm_testdir, '').strip('/')
        rel_parts = rel.split('/')
        basename = rel_parts[-1]

        rel_dir = ''
        if rel_parts.length() > 1
            rel_dir = '/'.join(rel_parts.slice(0, rel_parts.length() - 1))
        endif

        build_subdir = 'generated/testing/hpp'
        if rel_dir != ''
            build_subdir += '/' + rel_dir
        endif

        test_hpp_gen += {
            t: custom_target(
                'test.hpp.' + rel.replace('/', '_'),
                input: t,
                output: basename,
                build_subdir: build_subdir,
                build_always_stale: true,
                command: [
                    python,
                    rawrgen,
                    'replace-imports',
                    '@INPUT@',
                    '--when', 'rawr.*',
                    '--out', '@OUTDIR@',
                ],
            )
        }
    endforeach

    # Generate the amalgam-mode version from the generated header-mode source.
    foreach t : runner_cppm_srcs + test_cppm_srcs
        if test_amalgam_gen.has_key(t)
            continue
        endif

        rel = t.replace(cppm_testdir, '').strip('/')
        rel_parts = rel.split('/')
        basename = rel_parts[-1]

        rel_dir = ''
        if rel_parts.length() > 1
            rel_dir = '/'.join(rel_parts.slice(0, rel_parts.length() - 1))
        endif

        build_subdir = 'generated/testing/amalgam'
        if rel_dir != ''
            build_subdir += '/' + rel_dir
        endif

        test_amalgam_gen += {
            t: custom_target(
                'test.amalgam.' + rel.replace('/', '_'),
                input: test_hpp_gen[t],
                output: basename,
                build_subdir: build_subdir,
                build_always_stale: true,
                command: [
                    python,
                    rawrgen,
                    'replace-includes',
                    '@INPUT@',
                    '--when', 'rawr/*',
                    '--with', 'rawr.amalgam.hpp',
                    '--out', '@OUTDIR@',
                ],
            )
        }
    endforeach

    # For module tests we only require the actually imported modules.
    # This helps keep us honest instead of using rawr_dep directly:
    #
    # - Every module really does declare its own requires.
    # - Build times scale better.
    # - If some unrelated module fails to compile, we can still run this test.
    test_cpp_args  = ['-fno-builtin', '-flto']
    test_link_args = ['-fno-builtin', '-flto']

    testing_manifest = run_command(
        python, rawrscan, '--root', meson.current_source_dir(), '--manifest',
        runner_cppm_srcs, test_cppm_srcs,
        check: false,
    )
    if testing_manifest.returncode() != 0
        error(testing_manifest.stderr())
    endif

    test_libs   = []
    runner_libs = []

    foreach manifest_entry : testing_manifest.stdout().strip().split('\n')
        if manifest_entry.strip() == '' or manifest_entry.startswith('#')
            continue
        endif

        parts      = manifest_entry.split('|')
        kind       = parts[0]
        file       = parts[1]
        deps_str   = parts[3]
        ppdeps_str = parts[4]

        if kind != 'cpp'
            error('test files and runners must be cpp')
        endif

        deps = []
        foreach d : deps_str.split(',')
            if d == ''
                continue
            endif
            if not mod_deps.has_key(d)
                error('unknown module [@0@] from entry [@1@]'.format(d, ', '.join(manifest_entry)))
            endif
            deps += mod_deps[d]
        endforeach
        foreach d_ : ppdeps_str.split(',')
            d = d_.replace('/', '.')
            if d == ''
                continue
            endif
            if not mod_deps.has_key(d)
                error('unknown module [@0@] from entry [@1@]'.format(d, ', '.join(manifest_entry)))
            endif
            deps += mod_deps[d]
        endforeach

        if file in test_cppm_srcs
            test_libs += static_library(
                'test.unit.cppm.files.' + name + '.' + file.replace(cppm_testdir, '').replace('/', '_'),
                sources: file,
                dependencies: deps + [freestanding_dep],
                cpp_args: test_cpp_args,
                implicit_include_directories: false,
            )
        elif file in runner_cppm_srcs
            runner_libs += static_library(
                'test.unit.cppm.runners.' + name + '.' + file.replace(cppm_testdir, '').replace('/', '_'),
                sources: file,
                dependencies: deps + [freestanding_dep],
                cpp_args: test_cpp_args,
                implicit_include_directories: false,
            )
        else
            error('error')
        endif
    endforeach

    test(name,
        executable(
            'test.unit.cppm.exe.' + name,
            link_whole: [test_libs, runner_libs],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'cppm',
        timeout: 30,
    )

    runner_hpp_srcs = []
    test_hpp_srcs   = []

    foreach t : runner_cppm_srcs
        runner_hpp_srcs += [test_hpp_gen[t]]
    endforeach

    foreach t : test_cppm_srcs
        test_hpp_srcs += [test_hpp_gen[t]]
    endforeach

    test(name,
        executable(
            'test.unit.hpp.exe.' + name,
            link_whole: [
                static_library(
                    'test.unit.hpp.runners.' + name,
                    sources: runner_hpp_srcs,
                    dependencies: [rawr_headeronly_dep, freestanding_dep],
                    cpp_args: test_cpp_args,
                    implicit_include_directories: false,
                ),
                static_library(
                    'test.unit.hpp.files.' + name,
                    sources: test_hpp_srcs,
                    dependencies: [rawr_headeronly_dep, freestanding_dep],
                    cpp_args: test_cpp_args,
                    implicit_include_directories: false,
                ),
            ],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'hpp',
        timeout: 30,
    )

    runner_amalgam_srcs = []
    test_amalgam_srcs   = []

    foreach t : runner_cppm_srcs
        runner_amalgam_srcs += [test_amalgam_gen[t]]
    endforeach

    foreach t : test_cppm_srcs
        test_amalgam_srcs += [test_amalgam_gen[t]]
    endforeach

    test(name,
        executable(
            'test.unit.amalgam.exe.' + name,
            link_whole: [
                static_library(
                    'test.unit.amalgam.runners.' + name,
                    sources: runner_amalgam_srcs,
                    dependencies: [rawr_amalgam_dep, freestanding_dep],
                    cpp_args: test_cpp_args,
                    implicit_include_directories: false,
                ),
                static_library(
                    'test.unit.amalgam.files.' + name,
                    sources: test_amalgam_srcs,
                    dependencies: [rawr_amalgam_dep, freestanding_dep],
                    cpp_args: test_cpp_args,
                    implicit_include_directories: false,
                ),
            ],
            dependencies: [freestanding_dep],
            link_args: test_link_args,
        ),
        suite: 'amalgam',
        timeout: 30,
    )
endforeach
