project('luadbg', 'c', 
  default_options: [
    'default_library=static',
  ],
)

fs = import('fs')

if not fs.exists(meson.current_source_dir() / 'embed/debugger_lua.c')
  run_command(
    [ meson.source_root() + '/subprojects/luajit/src/luajit', 
      meson.current_source_dir() + '/embed/debugger_lua.c.lua',
      meson.current_source_dir() + '/debugger.lua'],
    check: true)
endif

luadbg_inc = include_directories('embed')
luadbg_src = files('embed/debugger_lua.c')

luadbg_lib = library(
  'luadbg',
  luadbg_src,
  include_directories: luadbg_inc,
  dependencies: [dependency('luajit')]
)

luadbg_dep = declare_dependency(
  link_with: luadbg_lib,
  include_directories: luadbg_inc,
)

meson.override_dependency('luadbg', luadbg_dep)
