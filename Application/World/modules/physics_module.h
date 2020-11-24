#ifndef WORLD_PHYSICS_MODULE_H
#define WORLD_PHYSICS_MODULE_H

#include "flecs.h"
#include "types.h"
#include "physics_types.h"

typedef struct
{
  ev_Vector3 linearVelocity;
  ev_Vector3 angularVelocity;

  real mass;
  real restitution;

  CollisionShape collisionShape;

  //TODO remove this
  RigidBodyType type;

} RigidBodyComponent;


typedef struct RigidBodyHandleComponent
{
  RigidBodyHandle handle;
} RigidBodyHandleComponent;

typedef struct
{
  ECS_DECLARE_COMPONENT(RigidBodyComponent);
  ECS_DECLARE_COMPONENT(RigidBodyHandleComponent);
} PhysicsModule;

void PhysicsModuleImport(ecs_world_t *world);

#define PhysicsModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, RigidBodyComponent); \
  ECS_IMPORT_COMPONENT(module, RigidBodyHandleComponent);

#endif
