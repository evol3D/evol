//TODO Comments / Logging
#include "rendering_module.h"

void RenderingModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, RenderingModule);

  ECS_COMPONENT(world, RenderingComponent);

  /* ECS_TYPE(world, type, comp1, comp2, comp3); */

  /* ECS_EXPORT_ENTITY(Renderable); */
  ECS_EXPORT_COMPONENT(RenderingComponent);
  /* ECS_EXPORT_TYPE(type); */
}
