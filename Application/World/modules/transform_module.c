#include "transform_module.h"

void TransformModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, TransformModule);

  ECS_COMPONENT(world, Position);
  ECS_COMPONENT(world, Rotation);
  ECS_COMPONENT(world, Scale);

  ECS_TYPE(world, Transform, Position, Rotation, Scale);

  ECS_EXPORT_COMPONENT(Position);
  ECS_EXPORT_COMPONENT(Rotation);
  ECS_EXPORT_COMPONENT(Scale);
  ECS_EXPORT_TYPE(Transform);
}
