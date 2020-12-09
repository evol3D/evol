
if build_machine.system() == 'windows'
  custom_target(
    'ShaderCompilationWin32',
    depends: evol_exe,
    output: 'Shaders',
    command: ['powershell.exe', meson.current_source_dir() + '/baseshaders.ps1', meson.project_source_root(), meson.project_build_root()],
    build_by_default: true,
  )

  # Environment Setup
  run_command('powershell.exe', meson.current_source_dir() + '/dllcopy.ps1', meson.project_source_root(), meson.project_build_root())
else
  custom_target(
    'ShaderCompilation',
    depends: evol_exe,
    output: 'Shaders',
    command: [ meson.current_source_dir() + '/baseshaders.sh', meson.project_source_root(), meson.project_build_root()],
    build_by_default: true,
  )
endif
