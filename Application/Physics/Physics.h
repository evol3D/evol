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
  void (*addRigidBody)(RigidBody*);
} Physics;











#ifdef __cplusplus
};
#endif
#endif
