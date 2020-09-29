#ifndef WORLD_PHYSICS_MODULE_H
#define WORLD_PHYSICS_MODULE_H

#include "flecs.h"

typedef float real;

typedef struct
{
  real x;
  real y;
  real z;
} LinearVelocity;

typedef struct
{
  real x;
  real y;
  real z;
} AngularVelocity;

typedef struct
{
  ECS_DECLARE_COMPONENT(LinearVelocity);
  ECS_DECLARE_COMPONENT(AngularVelocity);
  ECS_DECLARE_TYPE(Physics);
} PhysicsModule;

void PhysicsModuleImport(ecs_world_t *world);

#define PhysicsModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, LinearVelocity);\
  ECS_IMPORT_COMPONENT(module, AngularVelocity);\
  ECS_IMPORT_TYPE(module, Physics);

#endif
