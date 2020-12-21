project('evol', 'c',
version : '0.2',
default_options : [
  'warning_level=3',
  'default_library=static'
  ])

  # Build Options
  build_demo = true
  build_modules = true

  # Project Options
  colored_logging = true

  if build_modules
    subdir('modules')
  endif

  # Subprojects
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

  if colored_logging
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


  if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('evol', evollib_dep)
  endif

  if build_demo
    subdir('demo')
  endif
