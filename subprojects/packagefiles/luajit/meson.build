project('luajit', ['c'], version: '2.0')

cc = meson.get_compiler('c')

if build_machine.system() == 'windows'
  run_command('luajitbuild.bat')
else
  # Assuming everything else is POSIX
  run_command('make')
endif

luajit_dep = declare_dependency(
  dependencies: cc.find_library('luajit', dirs: meson.current_source_dir() + '/src'),
  include_directories: include_directories('src'))

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('luajit', luajit_dep)
endif