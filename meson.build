project('evol', 'c',
version : '0.2',
default_options : [
  'warning_level=3',
  'default_library=both',
  'c_std=gnu11',
  ])

cc = meson.get_compiler('c')

# Subprojects
subproject('luajit')
luajit = dependency('luajit')

cargs_sub = subproject('cargs', default_options: ['default_library=static'])
cargs = cargs_sub.get_variable('cargs_dep')

evol_threads = dependency('evol_threads')
hashmap = dependency('hashmap')
dl = cc.find_library('dl', required: false)
cute_files = dependency('cute_files')

evol_src = []

evol_incdir = [
  'include',
]

subdir('src')

evol_c_args = []

evol_deps = [
  luajit,
  evol_threads,
  cargs,
  hashmap,
  dl,
  cute_files,
]

evol_link_libs = [
]

if get_option('colored_logging')
  evol_c_args += '-DLOG_USE_COLOR'
endif

evol_lib = shared_library('evol',
  evol_src,
  include_directories: evol_incdir,
  dependencies: evol_deps,
  link_with: evol_link_libs,
  c_pch: 'pch/evpch.h',
  c_args: evol_c_args,
  install : true,
)

evollib_dep = declare_dependency(
  link_with: evol_lib,
  include_directories: evol_incdir,
  dependencies: evol_deps,
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('evol', evollib_dep)
endif

if get_option('build_demo')
  subdir('demo')
endif
