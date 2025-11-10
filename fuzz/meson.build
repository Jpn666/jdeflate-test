project('jdeflate-fuzz', 'c', default_options: ['c_std=c99', 'warning_level=3', 'buildtype=debug'])


if meson.get_compiler('c').get_id() != 'clang'
  error('clang is required to build this project')
endif


common = [
  '-fsanitize=fuzzer', '-fsanitize=address', '-fsanitize=undefined'
]

compilerflags = [
  '-Wconversion',
  '-Wcast-qual',
  '-Wunused-parameter',
  '-Wshadow',
  '-Wswitch',
  '-Wcast-align',
  '-Wdouble-promotion'
] + common

add_project_arguments(compilerflags, language: 'c')


deps = []
deps += [dependency('', fallback: ['ctoolbox', 'ctoolbox_dep'], static: true)]
deps += [dependency('', fallback: ['jdeflate', 'jdeflate_dep'], static: true)]


executable('inflator-fuzz', sources: ['inflator-fuzz.c'], dependencies: deps, link_args: common)
executable('deflator-fuzz', sources: ['deflator-fuzz.c'], dependencies: deps, link_args: common)
executable(   'zstrm-fuzz', sources: [   'zstrm-fuzz.c'], dependencies: deps, link_args: common)
