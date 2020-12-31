#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"
#include <types.h>

typedef struct
{
  ev_Matrix4 EV_ALIGN(16) localTransform ;
  ev_Matrix4 EV_ALIGN(16) worldTransform ;
} EV_ALIGN(16) TransformComponent ;

typedef struct
{
  ECS_DECLARE_COMPONENT(TransformComponent);
} TransformModule;

void TransformModuleImport(ecs_world_t *world);

#define TransformModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, TransformComponent);

#endif
