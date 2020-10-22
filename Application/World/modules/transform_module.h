#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
#include <types.h>

typedef struct
{
  ev_Matrix4 localTransform EV_ALIGN(16);
  ev_Matrix4 worldTransform EV_ALIGN(16);
} TransformComponent EV_ALIGN(16);

static ECS_UNUSED EcsMetaType __TransformComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(TransformComponent),
    .alignment = 4, //ECS_ALIGNOF(TransformComponent),
    .descriptor = 
      "{"
      "float localTransform_00;float localTransform_01;float localTransform_02;float localTransform_03;"
      "float localTransform_10;float localTransform_11;float localTransform_12;float localTransform_13;"
      "float localTransform_20;float localTransform_21;float localTransform_22;float localTransform_23;"
      "float localTransform_30;float localTransform_31;float localTransform_32;float localTransform_33;"
      ""
      ""
      "float worldTransform_00;float worldTransform_01;float worldTransform_02;float worldTransform_03;"
      "float worldTransform_10;float worldTransform_11;float worldTransform_12;float worldTransform_13;"
      "float worldTransform_20;float worldTransform_21;float worldTransform_22;float worldTransform_23;"
      "float worldTransform_30;float worldTransform_31;float worldTransform_32;float worldTransform_33;"
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
