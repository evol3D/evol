#ifndef WORLD_RENDERING_MODULE_H
#define WORLD_RENDERING_MODULE_H

#include "flecs.h"

typedef struct
{
  /* ECS_DECLARE_COMPONENT(component); */
  /* ECS_DECLARE_TYPE(type); */
} RenderingModule;

void RenderingModuleImport(ecs_world_t *world);

/* #define RenderingModuleImportHandles(module)\ */
/*   ECS_IMPORT_COMPONENT(module, component);\ */
/*   ECS_IMPORT_TYPE(module, type); */

#endif
