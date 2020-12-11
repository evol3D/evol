#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"
#include <types.h>

typedef struct
{
  ev_Matrix4 localTransform EV_ALIGN(16);
  ev_Matrix4 worldTransform EV_ALIGN(16);
} TransformComponent EV_ALIGN(16);

ECS_COMPONENT_EXTERN(TransformComponent);

void SetTransform(ecs_iter_t *it);

#define DEFINE_COMPONENTS_TRANSFORM(world) \
  ECS_COMPONENT_DEFINE(world, TransformComponent)

#define REGISTER_SYSTEMS_TRANSFORM(world) \
  ECS_SYSTEM(world, SetTransform, EcsOnSet, CASCADE: TransformComponent, TransformComponent)

#endif
