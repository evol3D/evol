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

// Returns a reference to a mutable version of the TransformComponent.position
ev_Vector4* entity_get_position_mut(unsigned int entt)
{
  ImportModule(TransformModule);
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  return &(tr->position);
}

// Returns a reference to an immutable version of the TransformComponent.position
const ev_Vector4* entity_get_position(unsigned int entt)
{
  ImportModule(TransformModule);
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->position);
}

// Returns a reference to a mutable version of the TransformComponent.rotation
ev_Vector4* entity_get_rotation_mut(unsigned int entt)
{
  ImportModule(TransformModule);
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  return &(tr->rotation);
}

// Returns a reference to an immutable version of the TransformComponent.rotation
const ev_Vector4* entity_get_rotation(unsigned int entt)
{
  ImportModule(TransformModule);
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->rotation);
}
