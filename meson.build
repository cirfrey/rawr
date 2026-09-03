project(
    'rawr',
    'cpp',
    version: '0.0',
    meson_version: '>= 1.10.0',
    default_options: [ 'warning_level=3', 'cpp_std=c++20' ]
)

python = import('python').find_installation('python3')
cpp_id = meson.get_compiler('cpp').get_id()

dir_sources = meson.current_source_dir()/'src'
dir_include = meson.current_source_dir()/'include'
dir_scripts = meson.current_source_dir()/'scripts'
dir_build   = meson.current_build_dir()
dir_include_name = 'include'

manifest_script = dir_scripts/'manifest.py'

#
# module.manifest in the source tree is the checked-in cache.
#
# The build gets its own generated copy so a normal build never dirties the
# working tree.
#
module_manifest_path = dir_build/'module.manifest'
module_cache_path    = meson.current_source_dir()/'module.manifest'

module_manifest = run_command(
    python,
    manifest_script,

    cpp_id,

    cpp_id == 'msvc' ? '/std=c++20' : '-std=c++20',
    cpp_id == 'msvc' ? '/I' : '-I', dir_include,

    '-D', 'RAWR_MODULE',

    '--root', meson.current_source_dir(),
    '--cache', module_cache_path,
    '--manifest', module_manifest_path,

    '--modules--',
    dir_include/'rawr.hpp',
    dir_include/'rawr',

    check: true,
).stdout().strip()

#
# BMI output directory.
#
run_command(
    python,
    '-c',
    'import os; os.makedirs("' + (dir_build/'bmi') + '", exist_ok=True)',
    check: true,
)

#
# Dictionary to hold the declare_dependency() for each module.
#
mod_deps = {}

foreach o : module_manifest.split('\n')
    if o.strip() == '' or o.startswith('#')
        continue
    endif

    parts = o.split('|')

    file = parts[0]
    mod  = parts[1]
    deps_str = parts[2]

    if mod == ''
        continue
    endif

    #
    # Resolve dependencies for this module.
    #
    current_deps = []

    if deps_str != ''
        foreach d : deps_str.split(',')
            current_deps += mod_deps[d]
        endforeach
    endif

    target_args = ['-DRAWR_MODULE']
    usage_args = []

    if cpp_id == 'clang'
        bmi_file = dir_build/'bmi'/(mod + '.pcm')
        target_args += ['-fmodule-output=' + bmi_file]
        usage_args += ['-fmodule-file=' + mod + '=' + bmi_file]
    elif cpp_id == 'msvc'
        bmi_file = dir_build/'bmi'/(mod + '.ifc')
        target_args += ['/ifcOutput', bmi_file]
        usage_args += ['/reference', mod + '=' + bmi_file]
    elif cpp_id == 'gcc'
        target_args += ['-fmodules-ts']
        usage_args += ['-fmodules-ts']
    endif

    lib = library(
        mod,
        configure_file(
            input: file,
            output: mod + '.cppm',
            copy: true,
        ),
        include_directories: dir_include_name,
        implicit_include_directories: false,
        dependencies: current_deps,
        cpp_args: target_args,
    )

    #
    # Bridge target:
    #
    # Forces dependent modules to wait until this module's object compilation
    # has completed, which also means its BMI has been produced.
    #
    bridge = custom_target(
        mod + '.stamp',
        input: lib.extract_all_objects(recursive: false,),
        output: mod + '.stamp',
        command: [
            python,
            '-c',
            'import sys; open(sys.argv[1], "w").close()',
            '@OUTPUT@',
        ],
    )

    mod_deps += {mod + '.stamp': bridge}

    #
    # By passing bridge through sources:, dependents are sequenced after the
    # module compilation.
    #
    mod_deps += {
        mod: declare_dependency(
            link_with: lib,
            compile_args: usage_args,
            sources: bridge,
            dependencies: current_deps,
        )
    }
endforeach


#
# Amalgamation.
#
run_target(
    'amalgam',
    command: [
        python,
        dir_scripts/'amalgam.py',
        '-I', dir_include,
        dir_include/'rawr.hpp',
        dir_include/'rawr.amalgam.hpp',
    ],
    depends: mod_deps['rawr.stamp'],
)


clang_tidy = find_program('clang-tidy', required: false)
if clang_tidy.found()
    run_target('tidy',
        command: [python, dir_scripts/'run_tidy.py',
            '--build', meson.current_build_dir(),
            '--root',  meson.current_source_dir()/'include',
        ]
    )
    run_target('tidy-strict',
        command: [python, dir_scripts/'run_tidy.py',
            '--build', meson.current_build_dir(),
            '--root',  meson.current_source_dir()/'include',
            '--werror',
        ]
    )
endif

### TODO: sectioned off for now.

additional_cpp_args = []

trace_flag = ''
if cpp_id == 'clang'
  trace_flag = '-ftime-trace'
elif cpp_id == 'gcc'
  trace_flag = '-ftime-report'
endif
time_trace = get_option('time_trace')
if time_trace == 'global'
  add_project_arguments(trace_flag, language: 'cpp')
elif time_trace == 'local'
  additional_cpp_args += [trace_flag]
endif

cpp = meson.get_compiler('cpp')
generate_asm_flags = []
if cpp.get_id() == 'msvc'
    # /FAs: Outputs assembly with source code interleaved (.asm)
    generate_asm_flags += ['/FAs']
else
    # -save-temps=obj: Automatically saves .s (assembly) and .ii (preprocessed)
    # -masm=intel: Godbolt defaults to Intel syntax
    # -fno-asynchronous-unwind-tables: Strips out the massive blocks of .cfi_* clutter!
    # -fverbose-asm: Adds helpful C++ variable names as comments next to registers
    generate_asm_flags += [
        '-save-temps=obj',
        '-masm=intel',
        '-fno-asynchronous-unwind-tables',
        '-fverbose-asm'
    ]
endif

# Always build executables at the root level for easier access on the file tree
# regardless if its registered in a subdir or not.
executables = []

if get_option('scratchpad')
    executables += {
        'target_name': 'scratchpad',
        'sources': files(dir_sources/'scratchpad.cpp'),
        'dependencies': [rawr_dep],
        'include_directories': [ dir_include_name ],
        'cpp_args': ['-nostdlib', '-fno-exceptions', '-fno-rtti'] + generate_asm_flags,
        'link_args': ['-nostdlib'],
    }
endif

foreach e : executables
    t_name = e['target_name']
    t_sources = e.get('sources', [])
    t_includes = e.get('include_directories', [])
    t_cpp_args = e.get('cpp_args', []) + additional_cpp_args

    exe = executable(
        t_name,
        sources:             t_sources,
        dependencies:        e.get('dependencies', []),
        include_directories: t_includes,
        c_args:              e.get('c_args', []),
        cpp_args:            t_cpp_args,
        link_args:           e.get('link_args', []),
        install:             e.get('install', false),
        build_by_default:    e.get('build_by_default', true),
    )
endforeach
