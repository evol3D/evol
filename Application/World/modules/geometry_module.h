#ifndef WORLD_GEOMETRY_MODULE_H
#define WORLD_GEOMETRY_MODULE_H

#include "flecs.h"
#include "Asset/Asset.h"
#include "types.h"

typedef struct
{
  ev_Vector3 *positionBuffer;
  ev_Vector3 *normalBuffer;
  ev_Vector2 *uvBuffer;

  unsigned int *indexBuffer;

  unsigned int indexCount;
  unsigned int vertexCount;
} MeshPrimitive;

typedef struct
{
  MeshPrimitive* primitives;
  unsigned int primitives_count;
} MeshComponent;

ECS_COMPONENT_EXTERN(MeshComponent);

// TODO: Auto free `MeshPrimitive* primitives` on component destroy

#define DEFINE_COMPONENTS_GEOMETRY(world) \
  ECS_COMPONENT_DEFINE(world, MeshComponent)

#define REGISTER_SYSTEMS_GEOMETRY(world) \




#endif
