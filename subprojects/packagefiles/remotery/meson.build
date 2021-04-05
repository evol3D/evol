project('remotery', 'c',
    default_options : [
    'warning_level=3',
    'default_library=static',
    ]
)

remotery_src = files('lib/Remotery.c')
remotery_inc = include_directories('lib')

remotery_lib = library(
    'remotery', 
    remotery_src, 
    include_directories: remotery_inc,
    c_args: ['-DRMT_DLL', '-DRMT_IMPL'],
)

remotery_dep = declare_dependency(
    link_with: remotery_lib,
    include_directories: remotery_inc,
    compile_args: ['-DRMT_DLL'],
)

meson.override_dependency('remotery', remotery_dep)