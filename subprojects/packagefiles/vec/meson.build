project('vec', 'c')

vec_src = files('src/vec.c')
vec_inc = include_directories('src')

vec_lib = library('vec', vec_src, include_directories: vec_inc)

vec_dep = declare_dependency(
  link_with: vec_lib,
  include_directories: vec_inc,
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('vec', vec_dep)
endif
