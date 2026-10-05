project(
    'rawr',
    'cpp',
    version: run_command('python3', 'scripts/rawrversion.py', check: true).stdout().strip(),
    meson_version: '>= 1.10.0',
    default_options: [ 'warning_level=3', 'cpp_std=c++20' ]
)

fs     = import('fs')
python = import('python').find_installation('python3')
cpp    = meson.get_compiler('cpp')
cpp_id = cpp.get_id()

dir_rawr    = meson.current_source_dir()
dir_src     = dir_rawr/'src'

dir_scripts    = dir_rawr/'scripts'
rawrscan       = dir_scripts/'rawrscan.py'
rawrgen        = dir_scripts/'rawrgen.py'
ensuredirs     = dir_scripts/'ensuredirs.py'
sync_generated = dir_scripts/'sync_generated.py'

dir_build             = meson.current_build_dir()
dir_bmi               = dir_build/'gcm.cache'
dir_generated         = dir_build/'generated'
dir_generated_include = dir_generated/'include'
dir_generated_hpp     = dir_generated_include/'hpp'
dir_generated_amalgam = dir_generated_include/'amalgam'

dir_cppm_pp = dir_rawr/'include'/'cppm'
dir_hpp     = dir_rawr/'include'/'hpp'
dir_amalgam = dir_rawr/'include'/'amalgam'

additional_cpp_args = []

#
# Build tracing.
#
trace_flags = {'clang': '-ftime-trace', 'gcc': '-ftime-report'}
if   get_option('time_trace') == 'global'
  add_project_arguments(trace_flags[cpp_id], language: 'cpp')
elif get_option('time_trace') == 'local'
  additional_cpp_args += [trace_flags[cpp_id]]
endif

# Convenience dependency for injecting freestanding flags.
freestanding_dep = cpp.get_id() == 'msvc' \
    ? declare_dependency(
        # no RTTI, no stack cookie
        # exceptions: build TUs with /EHs-c- and treat 'throw' as an error at review time;
        # MSVC has no clean single-flag "no exceptions" the way gcc/clang do.
        compile_args: ['/GR-', '/GS-'],
    ) \
    : declare_dependency(
        compile_args: cpp.get_supported_arguments([
            '-fno-exceptions',
            '-fno-rtti',
            '-fno-stack-protector',
            '-ffreestanding',
        ]),
        link_args: ['-nostdlib'],
    )

#
# BMI output directory.
#
bdirs_cmd = run_command(python, ensuredirs, dir_bmi, dir_generated_hpp, dir_generated_amalgam, check: false)
if bdirs_cmd.returncode() != 0
    error(bdirs_cmd.stderr())
endif

#
# module.manifest in the source tree is the checked-in cache.
#
module_manifest = run_command(
    python, rawrscan,
    '--root', meson.current_source_dir(),
    '--manifest', '--manifest-file', 'module.manifest',
    dir_src, dir_cppm_pp,
    check: false
)
if module_manifest.returncode() != 0
    error(module_manifest.stderr())
endif

#
# Dictionary to hold the declare_dependency() for each module.
#
mod_deps = {}
all_src_files = []
foreach manifest_entry : module_manifest.stdout().strip().split('\n')
    if manifest_entry.strip() == '' or manifest_entry.startswith('#')
        continue
    endif

    parts      = manifest_entry.split('|')
    kind       = parts[0]
    file       = parts[1]
    mod        = parts[2]
    deps_str   = parts[3]
    ppdeps_str = parts[4]

    all_src_files += files(file)

    #
    # Resolve dependencies for this module.
    #
    current_deps = [freestanding_dep]

    if deps_str != ''
        foreach d : deps_str.split(',')
            if d not in mod_deps
                error('Did not find module ' + d)
            endif
            current_deps += mod_deps[d]
        endforeach
    endif
    if ppdeps_str != ''
        foreach d_ : ppdeps_str.split(',')
            d = d_.replace('/', '.')
            if d not in mod_deps
                error('Did not find module ' + d)
            endif
            current_deps += mod_deps[d]
        endforeach
    endif

    if kind == 'module'
        target_args = []
        usage_args = []

        if cpp_id == 'clang'
            bmi_file     = dir_bmi/(mod + '.pcm')
            target_args += ['-fmodule-output=' + bmi_file]
            usage_args  += ['-fmodule-file=' + mod + '=' + bmi_file]
        elif cpp_id == 'msvc'
            bmi_file     = dir_bmi/(mod + '.ifc')
            target_args += ['/ifcOutput', bmi_file]
            usage_args  += ['/reference', mod + '=' + bmi_file]
        elif cpp_id == 'gcc'
            target_args += ['-fmodules-ts']
            usage_args  += ['-fmodules-ts']
        endif

        lib = library(
            mod,
            file,
            include_directories: fs.relative_to(dir_cppm_pp, dir_rawr),
            implicit_include_directories: false,
            dependencies: current_deps,
            cpp_args: target_args + additional_cpp_args,
        )

        #
        # Bridge target:
        #
        # Forces dependent modules to wait until this module's object compilation
        # has completed, which also means its BMI has been produced.
        #
        bridge = custom_target(
            mod + '.stamp',
            input: lib.extract_all_objects(recursive: false),
            output: mod + '.stamp',
            command: [ # TODO: do we need this script here?
                python, '-c', 'import sys; open(sys.argv[1], "w").close()', '@OUTPUT@',
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
                include_directories: fs.relative_to(dir_cppm_pp, dir_rawr),
            )
        }
    elif kind == 'pp'
        mod_deps += {
            mod: declare_dependency(
                dependencies: current_deps,
                include_directories: fs.relative_to(dir_cppm_pp, dir_rawr),
            )
        }
    endif
endforeach

#
# The default dep is the core module.
#
rawr_dep = mod_deps['rawr']
#
# Header-only distribution.
#
# Generate into the build tree. The target is deliberately always stale:
# generation is cheap enough and we want the build tree to represent the
# canonical sources exactly, independently of configure-time execution.
#
hpp_gen = custom_target(
    'rawr.hpp',
    input: all_src_files + [rawrscan, rawrgen],
    output: 'rawr.hpp',
    build_subdir: 'generated/include/hpp',
    build_always_stale: true,
    build_by_default: true,
    command: [python, rawrgen, 'demodularize', dir_src,dir_cppm_pp, '--out', '@OUTDIR@'],
)
rawr_headeronly_dep = declare_dependency(
    include_directories: include_directories(fs.relative_to(dir_generated_hpp, dir_build)),
    sources: hpp_gen,
)
run_target(
    'commit-hpp',
    depends: [hpp_gen],
    command: [python, sync_generated, dir_generated_hpp, dir_rawr/dir_hpp],
)

#
# Amalgam distribution.
#
# Same model: canonical sources -> build/generated/include/amalgam/.
#
amalgam_raw_gen = custom_target(
    'rawr.amalgam.raw.hpp',
    input: all_src_files + [rawrscan, rawrgen],
    output: 'rawr.amalgam.raw.hpp',
    build_subdir: 'generated',
    build_always_stale: true,
    build_by_default: true,
    command: [python, rawrgen, 'amalgamate', dir_src, dir_cppm_pp, '--out', '@OUTDIR@', '--outname', 'rawr.amalgam.raw.hpp'],
)
amalgam_gen = custom_target(
    'rawr.amalgam.hpp',
    input: amalgam_raw_gen,
    output: 'rawr.amalgam.hpp',
    build_subdir: 'generated/include/amalgam',
    build_always_stale: true,
    build_by_default: true,
    command: [python, rawrgen, 'replace-includes', '@INPUT@', '--when', 'rawr/*', '--with', 'rawr.amalgam.hpp', '--out', '@OUTDIR@', '--outname', 'rawr.amalgam.hpp'],
)
rawr_amalgam_dep = declare_dependency(
    include_directories: include_directories(fs.relative_to(dir_generated_amalgam, dir_build)),
    sources: amalgam_gen,
)
run_target(
    'commit-amalgam',
    depends: [amalgam_gen],
    command: [python, sync_generated, dir_generated_amalgam, dir_rawr/dir_amalgam],
)

subdir('test')

### TODO: sectioned off for now.
subdir_done()

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
