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


  cc = meson.get_compiler('c')
  cc_id = cc.get_id()

  if cc_id == 'gcc'
    add_project_arguments('-DEV_CC_GCC', language: 'c')
  elif cc_id == 'msvc'
    add_project_arguments('-DEV_CC_MSVC', language: 'c')
  else
    add_project_arguments('-DEV_CC_UNKNOWN', language: 'c')
  endif

  os = build_machine.system()
  if os == 'windows'
    add_project_arguments('-DEV_OS_WINDOWS', language: 'c')
  elif os == 'linux'
    add_project_arguments('-DEV_OS_LINUX', language: 'c')
  else
    add_project_arguments('-DEV_OS_UNKNOWN', language: 'c')
  endif

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
