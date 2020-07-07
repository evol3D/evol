lua_rootdir = '.'

if build_machine.system() == 'windows'
  error('findWithExt.sh not implemented for Windows')
endif

scriptlist = run_command(meson.project_source_root() + '/buildscripts/findWithExt.sh', meson.current_source_dir(), 'lua')

newline = '''
'''

scripts = scriptlist.stdout().strip().split(newline)

foreach script : scripts
scriptname = script.split('/')[-1]

configure_file(
  input: script,
  command: [meson.project_source_root() + '/subprojects/luajit/src/luajit', meson.project_source_root() + '/buildscripts/bin2c.lua', script],
  output: scriptname + '.h',
  capture: true
)

endforeach

