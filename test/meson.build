default_test_runner = files('runner.cpp')

test_groups = {
    'lib.test' : {
        'files': files('lib/test/checks.cpp', 'lib/test/checks2.cpp'),
        'runner': files('lib/test/runner.cpp'), # Requires a special runner.
    }
}

foreach name, group : test_groups
    test(name,
        executable(
            'test.' + name,
            sources: [group.get('runner', default_test_runner), group['files']],
            dependencies: [mod_deps['rawr'], freestanding_dep],
            cpp_args: ['-DRAWR_PP_TRANSITIVE_AS_MODULE', '-fno-builtin'],
            link_args: ['-fno-builtin'],
        ),
        suite: 'unit',
        timeout: 30,
    )
endforeach
