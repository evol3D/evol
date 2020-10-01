#include "transform_module.h"

void TransformModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, TransformModule);

  ECS_COMPONENT(world, TransformComponent);

  ECS_EXPORT_COMPONENT(TransformComponent);
}
