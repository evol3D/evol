#ifndef WORLD_TRANSFORM_MODULE_H
#define WORLD_TRANSFORM_MODULE_H

#include "flecs.h"

typedef float real;

typedef struct
{
  real x;
  real y;
  real z;
} Position;

typedef struct
{
  real x;
  real y;
  real z;
} Rotation;

typedef struct
{
  real x;
  real y;
  real z;
} Scale;

typedef struct
{
  ECS_DECLARE_COMPONENT(Position);
  ECS_DECLARE_COMPONENT(Rotation);
  ECS_DECLARE_COMPONENT(Scale);
  ECS_DECLARE_TYPE(Transform);
} TransformModule;

void TransformModuleImport(ecs_world_t *world);

#define TransformModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, Position);\
  ECS_IMPORT_COMPONENT(module, Rotation);\
  ECS_IMPORT_COMPONENT(module, Scale);\
  ECS_IMPORT_TYPE(module, Transform);

#endif
