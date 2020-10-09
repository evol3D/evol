project('cgltf', 'c')

cgltf_src = files('cgltf.c')
cgltf_inc = include_directories('.')

cgltf_lib = library('cgltf', cgltf_src, include_directories: cgltf_inc)

cgltf_dep = declare_dependency(
  link_with: cgltf_lib,
  include_directories: cgltf_inc,
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('cgltf', cgltf_dep)
endif
