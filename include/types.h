#ifndef EVOL_TYPES_H
#define EVOL_TYPES_H

typedef float real;

typedef struct
{
  real x, y, z, w;
} ev_Vector3;

typedef struct
{
  real x, y;
} ev_Vector2;

typedef ev_Vector3 ev_Vector4;

#endif
