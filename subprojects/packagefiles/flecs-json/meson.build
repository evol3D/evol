project('flecs-json', 'c')

flecs_dep = dependency('flecs')
flecs_meta_dep = dependency('flecs-meta')

json_args = []

if get_option('default_library') == 'static'
  json_args = '-Dflecs_json_STATIC'
endif

json_inc = include_directories('include')

json_src = files(
  'src/main.c'
)

json_dependencies = [
    flecs_dep,
    flecs_meta_dep,
  ]

json_lib = library('flecs-json',
  json_src,
  dependencies: json_dependencies,
  include_directories: json_inc,
  c_args: [ '-Dflecs_json_EXPORTS', json_args ],
  implicit_include_directories : false
)

json_dep = declare_dependency(
  link_with: json_lib,
  compile_args: json_args,
  dependencies: json_dependencies,
  include_directories: json_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-json', json_dep)
endif
