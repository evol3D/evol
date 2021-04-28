project('vec', 'c', 
  default_options: [ 
    'default_library=static', 
  ],
)

vec_inc = include_directories('.') 

vec_src = files('vec.c')

vec_lib = library(
  'vec', 
  vec_src, 
  include_directories: vec_inc,
  c_args: ['-DVEC_DLL', '-DVEC_IMPL'],
)

vec_dep = declare_dependency(
  link_with: vec_lib, 
  include_directories: vec_inc,
  compile_args: ['-DVEC_DLL'],
) 

meson.override_dependency('vec', vec_dep)
