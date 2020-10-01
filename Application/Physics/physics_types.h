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

typedef struct 
{
  unsigned int entt_id;

  CollisionShape collisionShape;
  real mass;
} RigidBody;

#define ANG2RAD(ang) ((ang/180) * M_PI)
#define RAD2ANG(rad) ((rad/M_PI) * 180)

#ifdef __cplusplus
}
#endif

#endif
