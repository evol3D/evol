#ifndef EVOL_PHYSICS_TYPES_H
#define EVOL_PHYSICS_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "engine_config.h"

typedef struct {
  real x, y, z;
} PhysicsPosition;

typedef struct {
  real x, y, z;
} PhysicsEulerRotation;

#ifdef BULLET_PHYSICS
   typedef void* CollisionShape;
#endif

typedef struct 
{
  PhysicsPosition position;
  PhysicsEulerRotation rotation;

  CollisionShape collisionShape;
  real mass;
} RigidBody;

#ifdef __cplusplus
}
#endif

#endif
