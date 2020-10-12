project('flecs-player', 'c')

flecs_dep = dependency('flecs')
flecs_meta_dep = dependency('flecs-meta')

player_args = []

if get_option('default_library') == 'static'
  player_args = '-Dflecs_player_STATIC'
endif

player_inc = include_directories('include')

player_src = files(
  'src/main.c'
)

player_dependencies = [
    flecs_dep,
    flecs_meta_dep,
  ]

player_lib = library('flecs-player',
  player_src,
  dependencies: player_dependencies,
  include_directories: player_inc,
  c_args: [ '-Dflecs_player_EXPORTS', player_args ],
  implicit_include_directories : false
)

player_dep = declare_dependency(
  link_with: player_lib,
  compile_args: player_args,
  dependencies: player_dependencies,
  include_directories: player_inc
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('flecs-player', player_dep)
endif
