project('evol', 'c', version: '0.1', default_options : ['default_library=static'])

cc = meson.get_compiler('c')

global_incdir = include_directories( './include', './Application', '.')

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'c')
  add_project_arguments('-DLOG_USE_COLOR', language : 'c')
endif

vulk_dep = dependency('Vulkan')

cmake = import('cmake')

flecs_proj = subproject('flecs')
flecs_dep = flecs_proj.get_variable('flecs_dep')

evol_eventsystem_proj = subproject('evol-eventsystem')
evol_eventsystem_dep = evol_eventsystem_proj.get_variable('eventsystem_dependency')

evol_threads_proj = subproject('evol_threads')
evol_thread_dep = evol_threads_proj.get_variable('evol_thread_dep')

dl_dep = cc.find_library('dl', required: false)

subdir('Application/Window')
subdir('Application/Vulkan')
subdir('Application/Physics')

srcfiles = files('src/main.c',
            'src/utils.c',
            'Application/App.c',
            'include/ev_log/rxi_logger/log.c',
            'Application/Input/Input.c',
            'Application/World/World.c',
            'Application/EventDebug/EventDebug.c',
            'Application/Game/Game.c',
            'Application/Renderer/Renderer.c',
            'Application/World/modules/transform_module.c',
            'Application/World/modules/script_module.c',
            'Application/World/modules/physics_module.c',
            )


evol_exe = executable( 'evol', srcfiles,
  include_directories: global_incdir,
  dependencies: [
    dl_dep,
    evol_eventsystem_dep,
    evol_thread_dep,
    flecs_dep,
    evphys_dep,
    ev_window_dep,
    ev_vulk_dep,
    ],
  )

custom_target(
  'env_setup',
  depends: evol_exe,
  output: 'dummy',
  command: [meson.current_source_dir() + '/baseshaders.sh', meson.current_source_dir(), meson.current_build_dir()],
  build_by_default: true,
  )
