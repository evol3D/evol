#ifndef EVOL_PHYSICS_H
#define EVOL_PHYSICS_H

#include <physics_types.h>

#ifdef __cplusplus
extern "C" {
#endif



extern struct _ev_Physics {
  int (*init)();
  int (*deinit)();
  int (*step)();
  int (*step_dt)(real);
  CollisionShape (*createBox)(real, real, real);
  RigidBodyHandle (*addRigidBody)(RigidBody*);
  void (*setGravity)(real, real, real);
  CollisionShape (*createSphere)(real);
  void (*removeRigidBody)(RigidBodyHandle);
  void (*updateRigidBody)(RigidBodyHandle, RigidBody*);
  CollisionShape (*createStaticFromTriangleIndexVertex)(int, int *, int, int, real *, int);
  CollisionShape (*generateConvexHull)(int vertexCount, ev_Vector3* vertices);
} Physics;











#ifdef __cplusplus
};
#endif
#endif
