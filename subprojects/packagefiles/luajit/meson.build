project('luajit', ['c'], version: '2.0')

cc = meson.get_compiler('c')
fs = import('fs')

if build_machine.system() == 'windows'
  command = 'luajitbuild.bat'
  outfile = 'lua51.dll'
else
  # Assuming everything else is POSIX
  error('Find the name of the outfile on non-windows')
  command = 'make'
endif
if not fs.exists('src/'+outfile)
  run_command(command)
endif

luajit_dep = declare_dependency(
  dependencies: [
    cc.find_library('dl', required: false),
    cc.find_library('luajit', dirs: meson.current_source_dir() + '/src'),
    cc.find_library('lua51', dirs: meson.current_source_dir() + '/src', required: false),
    cc.find_library('m', required: false),
  ],
  include_directories: include_directories('src')
)

meson.override_dependency('luajit', luajit_dep)
