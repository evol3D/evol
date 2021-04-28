project('evstr', 'c', 
  default_options: [ 
    'default_library=static', 
  ],
)

evstr_inc = include_directories('.') 

evstr_src = files('evstr.c')

evstr_lib = library(
  'evstr', 
  evstr_src, 
  include_directories: evstr_inc,
  c_args: ['-DEVSTR_DLL', '-DEVSTR_IMPL'],
)

evstr_dep = declare_dependency(
  link_with: evstr_lib, 
  include_directories: evstr_inc,
  compile_args: ['-DEVSTR_DLL'],
) 

meson.override_dependency('evstr', evstr_dep)
