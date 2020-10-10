//TODO Comments / Logging
#include "geometry_module.h"

void GeometryModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, GeometryModule);

  ECS_COMPONENT(world, MeshComponent);

  ECS_EXPORT_COMPONENT(MeshComponent);
}
