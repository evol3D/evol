#ifndef EVOL_TYPES_H
#define EVOL_TYPES_H

#ifdef EV_CC_GCC
# define EV_UNUSED   __attribute__((unused))
# define EV_ALIGN(x) __attribute__((aligned(x)))
# define EV_PACKED   __attribute__((packed))
#elif EV_CC_MSVC
# define EV_UNUSED
# define EV_ALIGN(x) __declspec(align(x))
# define EV_PACKED
#else
# define EV_UNUSED
# define EV_ALIGN(x)
# define EV_PACKED
#endif

typedef float real;

typedef struct
{
  real x, y, z;
  real padding;
} EV_PACKED EV_ALIGN(16) ev_Vector3;

typedef real EV_ALIGN(16) ev_Matrix4[4][4];

typedef struct
{
  real x, y, z, w;
} EV_PACKED EV_ALIGN(16) ev_Vector4;

typedef struct
{
  real x, y;
} EV_PACKED EV_ALIGN(16) ev_Vector2;

#endif
