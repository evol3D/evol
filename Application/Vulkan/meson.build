vulk_mod_src = [
    'Vulkan.c',
    'VulkanQueueManager.c',
]

vulk_mod_incdir = include_directories(
    '.',
    'include',
)

vulk_dep = dependency('Vulkan')
vma_dep = dependency('vma')

ev_vulk_lib = library('ev_vulk',
    vulk_mod_src,
    include_directories: [vulk_mod_incdir, global_incdir],
    dependencies: [
      vulk_dep,
      ev_window_dep,
      vma_dep,
    ]
)

ev_vulk_dep = declare_dependency(
    link_with: ev_vulk_lib,
    include_directories: vulk_mod_incdir,
    dependency: vma_dep
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('ev_vulk', ev_vulk_dep)
endif


