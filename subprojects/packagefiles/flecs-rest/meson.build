project('flecs-rest', 'c')

flecs_dep = dependency('flecs')
flecs_meta_dep = dependency('flecs-meta')
flecs_components_http_dep = dependency('flecs-components-http')
flecs_json_dep = dependency('flecs-json')

rest_args = []

if get_option('default_library') == 'static'
  rest_args = '-Dflecs_rest_STATIC'
endif

rest_inc = include_directories('include')

rest_src = files(
  'src/main.c'
)

rest_dependencies = [
    flecs_dep,
    flecs_meta_dep,
    flecs_components_http_dep,
    flecs_json_dep
  ]

rest_lib = library('flecs-rest',
  rest_src,
  dependencies: rest_dependencies,
  include_directories: rest_inc,
  c_args: [ '-Dflecs_rest_EXPORTS', rest_args ],
  implicit_include_directories : false
)

rest_dep = declare_dependency(
  link_with: rest_lib,
  compile_args: rest_args,
  dependencies: rest_dependencies,
  include_directories: rest_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-rest', rest_dep)
endif
