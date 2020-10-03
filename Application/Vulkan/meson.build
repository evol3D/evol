vulk_mod_src = [
    'Vulkan.c',
    'vk_mem_alloc.cpp'
]

vulk_mod_incdir = include_directories(
    '.'
)

ev_vulk_lib = library('ev_vulk', 
    vulk_mod_src, 
    include_directories: [vulk_mod_incdir, global_incdir], 
    dependencies: [ 
      vulk_dep, 
      ev_window_dep,
      ]
)

ev_vulk_dep = declare_dependency(
    link_with: ev_vulk_lib,
    include_directories: vulk_mod_incdir,
)
