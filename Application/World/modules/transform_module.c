//TODO Comments / Logging
#include "transform_module.h"
#include "utils.h"
#include "World/World.h"

void TransformModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, TransformModule);

  ECS_COMPONENT(world, TransformComponent);

  ECS_EXPORT_COMPONENT(TransformComponent);
}

const ev_Matrix4* entity_getWorldTransform(unsigned int entt)
{
  ImportModule(TransformModule);
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->worldTransform);
}

ev_Matrix4* entity_getWorldTransformMut(unsigned int entt)
{
  ImportModule(TransformModule);
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  return &(tr->worldTransform);
}

