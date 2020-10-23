#ifndef EVOL_PHYSICS_TYPES_H
#define EVOL_PHYSICS_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "utils.h"
#include "engine_config.h"

typedef void* CollisionShape;
typedef void* RigidBodyHandle;

typedef enum
{
  EV_RIGIDBODY_DYNAMIC,
  EV_RIGIDBODY_KINEMATIC,
  EV_RIGIDBODY_STATIC,
} RigidBodyType;

typedef struct 
{
  unsigned int entt_id;

  CollisionShape collisionShape;

  RigidBodyType type;
  real mass;
  real restitution;
} RigidBody;

#ifdef __cplusplus
}
#endif

#endif
