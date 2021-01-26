lua_rootdir = '.'

if build_machine.system() == 'windows'
  buildscriptname = 'findWithExt.ps1'
  shellname = 'powershell.exe'
  path_delim = '\\'
else
  buildscriptname = 'findWithExt.sh'
  shellname = 'sh'
  path_delim = '/'
endif

scriptlist = run_command(shellname, meson.project_source_root() + '/buildscripts/' + buildscriptname, meson.current_source_dir(), 'lua')

newline = '''
'''

scripts = scriptlist.stdout().strip().split(newline)

foreach script : scripts
scriptname = script.split(path_delim)[-1]

message(scriptname)

configure_file(
  input: script,
  command: [meson.source_root() + '/subprojects/luajit/src/luajit', meson.project_source_root() + '/buildscripts/bin2c.lua', script],
  output: scriptname + '.h',
  capture: true
)

endforeach
