project('cute', 'c',
  default_options: [
    'default_library=static',
  ],
)

cute_include = include_directories('.')

# All other targets should follow the same template
cutefiles_src = files('cute_files.c')
cutefiles_lib = library('cute_files', cutefiles_src, include_directories: cute_include)
cutefiles_dep = declare_dependency(link_with: cutefiles_lib, include_directories: cute_include)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('cute_files', cutefiles_dep)
endif
