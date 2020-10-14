window_mod_src = [
    'Window.c'
]

window_mod_incdir = include_directories(
    '.',
    '..',
)

glfw_proj = cmake.subproject('glfw')
glfw_dep = glfw_proj.dependency('glfw')

vma_dep = dependency('vma')

ev_window_lib = library('ev_window',
    window_mod_src,
    include_directories: [ window_mod_incdir, global_incdir ],
    dependencies: [glfw_dep, evol_eventsystem_dep, evol_thread_dep, vma_dep]
)


ev_window_dep = declare_dependency(
    link_with: ev_window_lib,
    include_directories: window_mod_incdir,
    dependencies: [glfw_dep],
)
