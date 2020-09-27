project('evol', 'c', version: '0.1', default_options : ['default_library=static'])

cc = meson.get_compiler('c')

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'c')
  add_project_arguments('-DLOG_USE_COLOR', language : 'c')
endif

cmake = import('cmake')

glfw_proj = cmake.subproject('glfw')
glfw_dep = glfw_proj.dependency('glfw')

flecs_proj = cmake.subproject('flecs')
flecs_dep = flecs_proj.dependency('flecs')

evol_eventsystem_proj = subproject('evol-eventsystem')
evol_eventsystem_dep = evol_eventsystem_proj.get_variable('eventsystem_dependency')

evol_threads_proj = subproject('evol_threads')
evol_thread_dep = evol_threads_proj.get_variable('evol_thread_dep')

dl_dep = cc.find_library('dl', required: false)

# physics_subproject = cmake.subproject('Physics')

srcfiles = files('src/main.c', 
            'Application/App.c', 
            'include/ev_log/rxi_logger/log.c', 
            'Application/Window/Window.c', 
            'Application/Input/Input.c', 
            'Application/World/World.c', 
            'Application/EventDebug/EventDebug.c',
            'Application/Game/Game.c', 
            'Application/World/modules/transform_module.c', 
            'Application/World/modules/script_module.c')

incdir = include_directories( './include', './Application', '.') 

evol_exe = executable( 'evol', srcfiles, include_directories: incdir, dependencies: [glfw_dep, dl_dep, evol_eventsystem_dep, evol_thread_dep, flecs_dep ])

# target_include_directories(['evol', 'PUBLIC', global_includes, evol_source_dir, evol_binary_dir, 'Application/Window/glfw/include', 'Application/EventSystem/include', 'Application/World/flecs/include'])
# target_include_directories(['event_system', 'PUBLIC', 'Application'])
# target_link_libraries(['evol', 'glfw', 'event_system', 'flecs', 'evphys'])
# configure_file([event_system_source_dir, '/extern/pthreads4w/dll/x64/pthreadVC2.dll', evol_binary_dir, '/pthreadVC2.dll', 'COPYONLY'])
