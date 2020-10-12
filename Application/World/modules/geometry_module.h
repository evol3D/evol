#ifndef WORLD_GEOMETRY_MODULE_H
#define WORLD_GEOMETRY_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
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

static EcsMetaType __MeshPrimitive__ = {
    .kind = EcsStructType,
    .size = sizeof(MeshPrimitive),
    .alignment = ECS_ALIGNOF(MeshPrimitive),
    .descriptor =
      "{"
      "uint64_t positionBuffer;"
      "uint64_t normalBuffer;"
      "uint64_t indexBuffer;"
      "uint32_t indexCount;"
      "uint32_t vertexCount;"
      "}"
      ,
    .alias = NULL
};

typedef struct
{
  MeshPrimitive* primitives;
  unsigned int primitives_count;
} MeshComponent;

static EcsMetaType __MeshComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(MeshComponent),
    .alignment = ECS_ALIGNOF(MeshComponent),
    .descriptor =
      "{"
      "uint64_t primitives;"
      "uint32_t primitive_count;"
      "}"
      ,
    .alias = NULL
};

// TODO: Auto free `MeshPrimitive* primitives` on component destroy

typedef struct
{
  ECS_DECLARE_COMPONENT(MeshComponent);
} GeometryModule;

void GeometryModuleImport(ecs_world_t *world);

#define GeometryModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, MeshComponent);\

#endif
