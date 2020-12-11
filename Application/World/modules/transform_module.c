//TODO Comments / Logging
#include "transform_module.h"
#include "utils.h"
#include "World/World.h"

#include <cglm/cglm.h>

ECS_COMPONENT_DECLARE(TransformComponent);

void SetTransform(ecs_iter_t *it) 
{
  TransformComponent *parentTransform = ecs_column(it, TransformComponent, 1);
  TransformComponent *transform = ecs_column(it, TransformComponent, 2);

  if(!parentTransform)
  {
    for(int i = 0; i < it->count; ++i)
      glm_mat4_dup(transform->worldTransform, transform->localTransform);
    return;
  }

  for(int i = 0; i < it->count; ++i)
  {
    glm_mat4_inv(parentTransform->worldTransform, transform[i].localTransform);
    glm_mat4_mul(transform[i].localTransform, transform[i].worldTransform, transform[i].localTransform);
  }
}

const ev_Matrix4* entity_getWorldTransform(unsigned int entt)
{
  const TransformComponent *tr = Entity_GetComponent(entt, TransformComponent);
  return &(tr->worldTransform);
}

void entity_setWorldTransform(unsigned int entt, ev_Matrix4 newWorldTransform)
{
  TransformComponent *tr = Entity_GetComponent_mut(entt, TransformComponent);
  glm_mat4_dup(newWorldTransform, tr->worldTransform);
  ecs_modified(World.getInstance(), entt, TransformComponent);
}
