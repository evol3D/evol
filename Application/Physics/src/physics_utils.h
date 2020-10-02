
#ifndef EVOL_PHYSICS_UTILS_HEADER
#define EVOL_PHYSICS_UTILS_HEADER

#define ev2btVector3(vec_p) \
  (btVector3(vec_p->x, vec_p->y, vec_p->z))

#define ANG2RAD(ang) ((ang/180) * M_PI)
#define RAD2ANG(rad) ((rad/M_PI) * 180)


#endif
