project('flecs-dash', 'c')

flecs_dep = dependency('flecs')
flecs_meta_dep = dependency('flecs-meta')
flecs_components_http_dep = dependency('flecs-components-http')
flecs_player_dep = dependency('flecs-player')
flecs_rest_dep = dependency('flecs-rest')

dash_args = []

if get_option('default_library') == 'static'
  dash_args = '-Dflecs_dash_STATIC'
endif

dash_inc = include_directories('include')

dash_src = files(
  'src/main.c',
  'src/monitor.c'
)

dash_dependencies = [
    flecs_dep,
    flecs_meta_dep,
    flecs_components_http_dep,
    flecs_player_dep,
    flecs_rest_dep
  ]

dash_lib = library('flecs-dash',
  dash_src,
  dependencies: dash_dependencies,
  include_directories: dash_inc,
  c_args: [ '-Dflecs_dash_EXPORTS', dash_args ],
  implicit_include_directories : false
)

dash_dep = declare_dependency(
  link_with: dash_lib,
  compile_args: dash_args,
  dependencies: dash_dependencies,
  include_directories: dash_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-dash', dash_dep)
endif
