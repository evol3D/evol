#ifndef EV_WORLDMODULES_HEADER
#define EV_WORLDMODULES_HEADER

#include "modules/rendering_module.h"
#include "modules/physics_module.h"
#include "modules/transform_module.h"
#include "modules/geometry_module.h"
#include "modules/script_module.h"
#include "modules/camera_module.h"

#define DEFINE_GLOBAL_COMPONENTS(world) \
  DEFINE_COMPONENTS_TRANSFORM(world); \
  DEFINE_COMPONENTS_SCRIPT(world); \
  DEFINE_COMPONENTS_PHYSICS(world); \
  DEFINE_COMPONENTS_CAMERA(world); \
  DEFINE_COMPONENTS_GEOMETRY(world); \
  DEFINE_COMPONENTS_RENDERING(world)

#define REGISTER_GLOBAL_SYSTEMS(world) \
  REGISTER_SYSTEMS_TRANSFORM(world); \
  REGISTER_SYSTEMS_SCRIPT(world); \
  REGISTER_SYSTEMS_PHYSICS(world); \
  REGISTER_SYSTEMS_CAMERA(world); \
  REGISTER_SYSTEMS_GEOMETRY(world); \
  REGISTER_SYSTEMS_RENDERING(world)


#endif
