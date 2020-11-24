#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"
#include <types.h>

typedef struct
{
  ev_Matrix4 localTransform EV_ALIGN(16);
  ev_Matrix4 worldTransform EV_ALIGN(16);
} TransformComponent EV_ALIGN(16);

typedef struct
{
  ECS_DECLARE_COMPONENT(TransformComponent);
} TransformModule;

void TransformModuleImport(ecs_world_t *world);

#define TransformModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, TransformComponent);

#endif
