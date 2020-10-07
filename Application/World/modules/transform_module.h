#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
#include <types.h>

typedef struct
{
  ev_Vector4 position;
  ev_Vector4 rotation;
  ev_Vector4 scale;
} TransformComponent;

static EcsMetaType __TransformComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(TransformComponent),
    .alignment = ECS_ALIGNOF(TransformComponent),
    .descriptor = 
      "{"
      "float pos_x; float pos_y; float pos_z; float pos_w;"
      "float rot_x; float rot_y; float rot_z; float rot_w;"
      "float scale_x; float scale_y; float scale_z; float scale_w;"
      "}"
      ,
    .alias = NULL
};

typedef struct
{
  ECS_DECLARE_COMPONENT(TransformComponent);
} TransformModule;

void TransformModuleImport(ecs_world_t *world);

#define TransformModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, TransformComponent);

#endif
