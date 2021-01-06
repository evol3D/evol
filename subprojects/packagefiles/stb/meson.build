project('stb', 'c')

# For all targets
stb_inc = include_directories('.')

# All other targets should follow the same template
stbimage_src = files('stb_image.c')
stbimage_lib = library('stb_image', stbimage_src, include_directories: stb_inc)
stbimage_dep = declare_dependency(link_with: stbimage_lib, include_directories: stb_inc)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('stbimage', stbimage_dep)
endif
