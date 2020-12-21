project('evol', 'c',
version : '0.2',
default_options : [
  'warning_level=3',
  'default_library=static'
  ])

  if get_option('build_modules')
    subdir('modules')
  endif

  # Subprojects
  subproject('luajit')
  luajit = dependency('luajit')
  cargs  = dependency('cargs')

  evol_src = []

  evol_incdir = [
    'include',
  ]

  subdir('src')

  evol_c_args = []

  evol_deps = [
    luajit,
    cargs,
  ]

  if get_option('colored_logging')
    evol_c_args += '-DLOG_USE_COLOR'
  endif

  evol_lib = library('evol',
  evol_src,
  include_directories: evol_incdir,
  dependencies: evol_deps,
  c_pch: 'pch/evpch.h',
  c_args: evol_c_args,
  install : true)

  evollib_dep = declare_dependency(
    link_with: evol_lib,
    include_directories: evol_incdir,
    dependencies: evol_deps,
  )

  evolmod_dep = declare_dependency(
    include_directories: evol_incdir,
  )

  if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('evol', evollib_dep)
    meson.override_dependency('evolmod', evolmod_dep)
  endif

  if get_option('build_demo')
    subdir('demo')
  endif
