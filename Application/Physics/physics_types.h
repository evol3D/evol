#ifndef EVOL_PHYSICS_TYPES_H
#define EVOL_PHYSICS_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "engine_config.h"

#ifdef BULLET_PHYSICS
# ifdef __cplusplus
/* #  include "btBulletDynamicsCommon.h" */
   /* typedef btCollisionShape*                                     CollisionShape; */
   typedef void* CollisionShape;
# else
   typedef void* CollisionShape;
# endif
#endif

typedef struct 
{
  CollisionShape collisionShape;
  real mass;
} RigidBody;

#ifdef __cplusplus
}
#endif

#endif
