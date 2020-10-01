#ifndef EVOL_PHYSICS_TYPES_H
#define EVOL_PHYSICS_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "engine_config.h"

typedef void* CollisionShape;
typedef void* RigidBodyHandle;

typedef struct 
{
  ev_Vector3* position;
  ev_Vector3* rotation;

  CollisionShape collisionShape;
  real mass;
} RigidBody;

#define ANG2RAD(ang) ((ang/180) * M_PI)

#ifdef __cplusplus
}
#endif

#endif
