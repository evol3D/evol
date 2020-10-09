#ifndef WORLD_PHYSICS_MODULE_H
#define WORLD_PHYSICS_MODULE_H

#include "flecs.h"
#include <flecs_meta.h>
#include "types.h"
#include "physics_types.h"

typedef struct
{
  ev_Vector3 linearVelocity;
  ev_Vector3 angularVelocity;

  real mass;
  real restitution;

  CollisionShape collisionShape;

} RigidBodyComponent;

ECS_UNUSED
static EcsMetaType __RigidBodyComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(RigidBodyComponent),
    .alignment = ECS_ALIGNOF(RigidBodyComponent),
    .descriptor = 
      "{"
      "float linearVelocity_x; float linearVelocity_y; float linearVelocity_z; float linearVelocity_w;"
      "float angularVelocity_x; float angularVelocity_y; float angularVelocity_z; float angularVelocity_w;"
      "float mass; float restitution;"
      "uint64_t collisionShape;"
      "}"
      ,
    .alias = NULL
};

typedef struct RigidBodyHandleComponent
{
  RigidBodyHandle handle;
} RigidBodyHandleComponent;

ECS_UNUSED
static EcsMetaType __RigidBodyHandleComponent__ = {
    .kind = EcsStructType,
    .size = sizeof(RigidBodyHandleComponent),
    .alignment = ECS_ALIGNOF(RigidBodyHandleComponent),
    .descriptor = "{uint64_t handle;}",
    .alias = NULL
};

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
