project('flecs-systems-civetweb', 'c')

flecs_dep = dependency('flecs')
flecs_components_http_dep = dependency('flecs-components-http')

systems_civetweb_args = []

if get_option('default_library') == 'static'
  systems_civetweb_args = '-Dflecs_systems_civetweb_STATIC'
endif

systems_civetweb_inc = include_directories('include')

systems_civetweb_src = files(
  'src/civet.c',
  'src/civetweb.c',
)

systems_civetweb_dependencies = [
    flecs_dep,
    flecs_components_http_dep,
  ]

systems_civetweb_lib = library('flecs-systems-civetweb',
  systems_civetweb_src,
  dependencies: systems_civetweb_dependencies,
  include_directories: systems_civetweb_inc,
  c_args: [ '-Dflecs_systems_civetweb_EXPORTS', systems_civetweb_args ],
  implicit_include_directories : false
)

systems_civetweb_dep = declare_dependency(
  link_with: systems_civetweb_lib,
  compile_args: systems_civetweb_args,
  dependencies: systems_civetweb_dependencies,
  include_directories: systems_civetweb_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-systems-civetweb', systems_civetweb_dep)
endif
