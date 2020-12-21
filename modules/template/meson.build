project('template', 'c',
version : '0.1',
default_options : ['warning_level=3'])

mod_src = [
  'src/template.c'
]

mod_incdir = [
  'include'
]

evol_sub = subproject('evol', default_options: ['build_demo=false'])
evolmod_dep = evol_sub.get_variable('evolmod_dep')

mod_deps = [
  evolmod_dep,
]

moduleconfig = get_option('moduleconfig')
configure_file(
  input: moduleconfig,
  command: [
    meson.project_source_root() + '/subprojects/luajit/src/luajit', 
    meson.project_source_root() + '/subprojects/evol/buildscripts/bin2cstring.lua', 
    meson.project_source_root() + '/' + moduleconfig
  ],
  output: moduleconfig + '.h',
  capture: true
)

module = shared_library(
  'template', mod_src,
  include_directories: mod_incdir,
  dependencies: mod_deps,
)



