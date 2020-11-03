#ifndef WORLD_RENDERING_MODULE_H
#define WORLD_RENDERING_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
#include <Renderer/renderer_types.h>

typedef struct
{
  MeshRenderData meshRenderData;
} RenderingComponent;

typedef struct
{
  ECS_DECLARE_COMPONENT(RenderingComponent);
  /* ECS_DECLARE_COMPONENT(component); */
  /* ECS_DECLARE_TYPE(type); */
} RenderingModule;

static ECS_UNUSED EcsMetaType __RenderingComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(RenderingComponent),
    .alignment = ECS_ALIGNOF(RenderingComponent),
    .descriptor =
      "{"
      "uint64_t primitivesData;"
      "uint32_t length;"
      "uint32_t capacity;"
      "}"
      ,
    .alias = NULL
};

void RenderingModuleImport(ecs_world_t *world);

#define RenderingModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, RenderingComponent);\
/*   ECS_IMPORT_TYPE(module, type); */

#endif
