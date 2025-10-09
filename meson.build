project('jdeflate-test', 'c', default_options: ['c_std=c99', 'warning_level=3'])


deps = []
deps += [dependency('', fallback: ['ctoolbox', 'ctoolbox_dep'], static: true)]
deps += [dependency('', fallback: ['jdeflate', 'jdeflate_dep'], static: true)]


if get_option('buildtype').startswith('release')
    add_project_arguments('-DNDEBUG', language : 'c')
endif

cc = meson.get_compiler('c')
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
endif

executable('inflator', sources: ['inflator.c'], dependencies: deps)
executable('deflator', sources: ['deflator.c'], dependencies: deps)
executable('zstrm',    sources: [   'zstrm.c'], dependencies: deps)
