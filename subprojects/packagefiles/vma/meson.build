project('vma', ['c','cpp'])

add_project_arguments('-Dc_std=c99', language: 'c')

vma_src = files('vk_mem_alloc.cpp')
vma_inc = include_directories('src')

vulk_dep = dependency('Vulkan')
vma_lib = library('vma', vma_src, include_directories: vma_inc, dependencies: vulk_dep)

vma_dep = declare_dependency(
  link_with: vma_lib,
  include_directories: vma_inc,
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('vma', vma_dep)
endif
