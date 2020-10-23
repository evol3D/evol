#ifndef WORLD_NODE_MODULE_H
#define WORLD_NODE_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
#include <vec.h>
#include <types.h>
#include <World/World.h>

typedef vec_t(Entity) EntityVec;

typedef struct
{
  Entity parent;
  EntityVec children;
} NodeComponent EV_ALIGN(16);

static ECS_UNUSED EcsMetaType __NodeComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(NodeComponent),
    .alignment = 8, //ECS_ALIGNOF(NodeComponent),
    .descriptor = 
      "{"
      "  uint32_t parent;"
      "  uint64_t childrenData; uint32_t childrenCount; uint32_t childrenCapacity;"
      "}"
      ,
    .alias = NULL
};

typedef struct
{
  ECS_DECLARE_COMPONENT(NodeComponent);
} NodeModule;

void NodeModuleImport(ecs_world_t *world);

#define NodeModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, NodeComponent);

#endif
