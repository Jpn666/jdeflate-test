project('jdeflate-bench', 'c', default_options: ['c_std=c99', 'warning_level=3', 'buildtype=release'])


deps = []
deps += [dependency('', fallback: ['ctoolbox', 'ctoolbox_dep'], static: true)]
deps += [dependency('', fallback: ['jdeflate', 'jdeflate_dep'], static: true)]


cc = meson.get_compiler('c')
if get_option('buildtype').startswith('release')
  add_project_arguments('-DNDEBUG', language: 'c')
  if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
    add_project_arguments('-march=native', language: 'c')
  endif
endif

if target_machine.system() in ['android', 'darwin', 'dragonfly', 'freebsd', 'gnu', 'linux', 'netbsd', 'openbsd', 'sunos']
  add_project_arguments('-D_POSIX_C_SOURCE=199309L', language: 'c')
else
  if target_machine.system() in ['windows', 'cygwin']
    deps += [cc.find_library('winmm', required: false)]
  endif
endif

if cc.get_id() == 'msvc'
  add_project_arguments('-D__MSVC__', language: 'c')
else
  if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
    compilerflags = [
      '-Wconversion',
      '-Wcast-qual',
      '-Wunused-parameter',
      '-Wshadow',
      '-Wswitch',
      '-Wcast-align',
      '-Wdouble-promotion'
    ]
    add_project_arguments(compilerflags, language: 'c')
  endif
  add_project_arguments('-D_POSIX_C_SOURCE=199309L', language: 'c')
endif

executable('zstrm-bench', sources: ['zstrm-bench.c'], dependencies: deps)
executable('inflator-bench', sources: ['inflator-bench.c'], dependencies: deps)
