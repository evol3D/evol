#ifndef WORLD_NODE_MODULE_H
#define WORLD_NODE_MODULE_H

#include "flecs.h"
#include <vec.h>
#include <types.h>
#include <World/World.h>

typedef vec_t(Entity) EntityVec;

typedef struct
{
  Entity parent;
  EntityVec children;
} NodeComponent EV_ALIGN(16);

typedef struct
{
  ECS_DECLARE_COMPONENT(NodeComponent);
} NodeModule;

void NodeModuleImport(ecs_world_t *world);

#define NodeModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, NodeComponent);

#endif
