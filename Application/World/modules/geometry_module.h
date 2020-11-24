#ifndef WORLD_GEOMETRY_MODULE_H
#define WORLD_GEOMETRY_MODULE_H

#include "flecs.h"
#include "Asset/Asset.h"
#include "types.h"

typedef struct
{
  ev_Vector3 *positionBuffer;
  ev_Vector3 *normalBuffer;

  unsigned int *indexBuffer;

  unsigned int indexCount;
  unsigned int vertexCount;
} MeshPrimitive;

typedef struct
{
  MeshPrimitive* primitives;
  unsigned int primitives_count;
} MeshComponent;

// TODO: Auto free `MeshPrimitive* primitives` on component destroy

typedef struct
{
  ECS_DECLARE_COMPONENT(MeshComponent);
} GeometryModule;

void GeometryModuleImport(ecs_world_t *world);

#define GeometryModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, MeshComponent);\

#endif
