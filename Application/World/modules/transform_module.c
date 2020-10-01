#include "transform_module.h"
#include "utils.h"
#include "World/World.h"

void TransformModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, TransformModule);

  ECS_COMPONENT(world, TransformComponent);

  ECS_EXPORT_COMPONENT(TransformComponent);
}

ev_Vector3* entity_get_position_mut(unsigned int entt)
{
  ImportModule(TransformModule);
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  return &(tr->position);
}

const ev_Vector3* entity_get_position(unsigned int entt)
{
  ImportModule(TransformModule);
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->position);
}

ev_Vector3* entity_get_rotation_mut(unsigned int entt)
{
  ImportModule(TransformModule);
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  return &(tr->rotation);
}

const ev_Vector3* entity_get_rotation(unsigned int entt)
{
  ImportModule(TransformModule);
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->rotation);
}
