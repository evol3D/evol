project('flecs-components-http', 'c')

flecs_dep = dependency('flecs')
flecs_meta_dep = dependency('flecs-meta')

components_http_args = []

if get_option('default_library') == 'static'
  components_http_args = '-Dflecs_components_http_STATIC'
endif

components_http_inc = include_directories('include')

components_http_src = files(
  'src/http.c'
)

components_http_dependencies = [
    flecs_dep,
    flecs_meta_dep,
  ]

components_http_lib = library('flecs-components-http',
  components_http_src,
  dependencies: components_http_dependencies,
  include_directories: components_http_inc,
  c_args: [ '-Dflecs_components_http_EXPORTS', components_http_args ],
  implicit_include_directories : false
)

components_http_dep = declare_dependency(
  link_with: components_http_lib,
  compile_args: components_http_args,
  dependencies: components_http_dependencies,
  include_directories: components_http_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-components-http', components_http_dep)
endif
