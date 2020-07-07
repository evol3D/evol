demo_src = [
  'src/main.c',
]

configure_file(input: 'config.lua', output: 'config.lua', copy: true)

demo_deps = [dependency('evol')]
demo_exe = executable( 'demo', demo_src, dependencies: demo_deps )
