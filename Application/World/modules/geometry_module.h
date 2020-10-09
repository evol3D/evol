#ifndef WORLD_GEOMETRY_MODULE_H
#define WORLD_GEOMETRY_MODULE_H

#include "flecs.h"
#include "flecs_meta.h"
#include "Asset/AssetLoader.h"

typedef struct
{
  BufferView positionBuffer;
  BufferView normalBuffer;
  BufferView indexBuffer;

  unsigned int indexCount;
} MeshPrimitive;

static EcsMetaType __MeshPrimitive__ = {
    .kind = EcsStructType,
    .size = sizeof(MeshPrimitive),
    .alignment = ECS_ALIGNOF(MeshPrimitive),
    .descriptor =
      "{"
      "uint32_t positionBuffer_idx; uint32_t positionBuffer_offset; uint32_t positionBuffer_size;"
      "uint32_t normalBuffer_idx; uint32_t normalBuffer_offset; uint32_t normalBuffer_size;"
      "uint32_t indexBuffer_idx; uint32_t indexBuffer_offset; uint32_t indexBuffer_size;"
      "uint32_t indexCount;"
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
