#ifndef EVOL_PHYSICS_H
#define EVOL_PHYSICS_H
#ifdef __cplusplus
extern "C" {
#endif











#include "globals.h"
#include "types.h"

extern struct _ev_Physics {
  int (*init)();
  int (*deinit)();
  int (*step)();
  int (*step_dt)(real);
} Physics;











#ifdef __cplusplus
};
#endif
#endif
