#ifndef EVOL_TYPES_H
#define EVOL_TYPES_H

#ifdef __GNUC__
# define EV_UNUSED   __attribute__((unused))
# define EV_ALIGN(x) __attribute__((aligned(x)))
# define EV_PACKED   __attribute__((packed))
#elif _MSC_BUILD
# define EV_UNUSED   const // This is a hack with zero guarantees of actually working.
# define EV_ALIGN(x) __declspec(align(x))
# define EV_PACKED   __pragma(pack(1))
#else
# define EV_UNUSED
# define EV_ALIGN
# define EV_PACKED
#endif

typedef float real;

typedef struct
{
  real x, y, z;
  real padding;
} EV_PACKED ev_Vector3 EV_ALIGN(16);

typedef real ev_Matrix4[4][4];

typedef struct
{
  real x, y, z, w;
} EV_PACKED ev_Vector4 EV_ALIGN(16);

typedef struct
{
  real x, y;
} EV_PACKED ev_Vector2 EV_ALIGN(16);

#endif
