#ifndef WORLD_PHYSICS_MODULE_H
#define WORLD_PHYSICS_MODULE_H

#include "flecs.h"
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

ECS_COMPONENT_EXTERN(RigidBodyComponent);
ECS_COMPONENT_EXTERN(RigidBodyHandleComponent);

void SetRigidBody(ecs_iter_t *it);
void AddRigidBody(ecs_iter_t *it);
void RemoveRigidBody(ecs_iter_t *it);

#define DEFINE_COMPONENTS_PHYSICS(world) \
  ECS_COMPONENT_DEFINE(world, RigidBodyComponent); \
  ECS_COMPONENT_DEFINE(world, RigidBodyHandleComponent)

#define REGISTER_SYSTEMS_PHYSICS(world) \
  ECS_TRIGGER(world, AddRigidBody, EcsOnAdd, RigidBodyComponent); \
  ECS_TRIGGER(world, RemoveRigidBody, EcsOnRemove, RigidBodyComponent); \
  ECS_SYSTEM(world, SetRigidBody, EcsOnSet, RigidBodyComponent, RigidBodyHandleComponent, TransformComponent)

#endif
