fs = import('fs')

supported_langs = ['c', 'cpp']

if fs.is_file(meson.project_source_root() / 'meta/evmod.namespaces')
  add_project_arguments('-DEVMOD_NAMESPACES_DEF', language: supported_langs)
endif
if fs.is_file(meson.project_source_root() / 'meta/evmod.events')
  add_project_arguments('-DEVMOD_EVENTS_DEF', language: supported_langs)
endif
if fs.is_file(meson.project_source_root() / 'meta/evmod.types')
  add_project_arguments('-DEVMOD_TYPES_DEF', language: supported_langs)
endif
if fs.is_file(meson.project_source_root() / 'meta/evmod.configvars')
  add_project_arguments('-DEVMOD_CONFIGVARS_DEF', language: supported_langs)
endif

if build_machine.system() != 'windows'
  # This arg is only tested with gcc
  add_project_arguments('-fms-extensions', language: supported_langs)
else
  # For some reason, a non conformant preprocessor is the default for cpp in msvc
  add_project_arguments('/Zc:preprocessor', language: supported_langs)
endif

cc = meson.get_compiler('c')

evol_dep = dependency('evol')
dl_dep = cc.find_library('dl', required: false)

evmod_deps = [
  evol_dep,
  dl_dep,
]

moduleconfig = get_option('moduleconfig')

modconf_contents = fs.read(meson.project_source_root() / moduleconfig).strip()
modconf_oneline = ''.join(modconf_contents.split('\n'))
add_project_arguments('-DEVMOD_LUA='+modconf_oneline, language: supported_langs)

evmod_name_prefix = ''
evmod_name_suffix = 'evmod'
