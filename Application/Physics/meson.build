srcfiles = [
  'src/Physics.cpp',
  'src/BulletState.cpp',
  ]

incdir = include_directories('.', '../../include')

bullet_opt_var = cmake.subproject_options()

bullet_opt_var.add_cmake_defines({'BUILD_EXTRAS': false})

bullet3_proj = cmake.subproject('bullet3', options: bullet_opt_var)
bullet_dynamics_dep = bullet3_proj.dependency('BulletDynamics')
bullet_collision_dep = bullet3_proj.dependency('BulletCollision')
linear_math_dep = bullet3_proj.dependency('LinearMath')

physics_debug_proj = subproject('physics_debug')
physics_debug_dep = physics_debug_proj.get_variable('physics_debug_dep')

library_name_prefix = []
library_name_suffix = []

if (build_machine.system() == 'windows' and get_option('default_library') == 'static')
library_name_prefix = ''
library_name_suffix = 'lib'
endif

evphys_lib = library('evphys', srcfiles, include_directories: incdir,
  dependencies: [
    bullet_dynamics_dep, 
    bullet_collision_dep, 
    linear_math_dep,

    # Remove if not debugging
    physics_debug_dep,
    ],
  name_prefix: library_name_prefix,
  name_suffix: library_name_suffix,
  )

evphys_dep = declare_dependency(link_with: evphys_lib, include_directories: incdir)
