#ifndef EVOL_GLOBAL_UTILS_HEADER
#define EVOL_GLOBAL_UTILS_HEADER

#include <types.h>

// Sleep in ms
void sleep_ms(double milliseconds);

// ECS Related
extern inline ev_Vector3* entity_get_position_mut(unsigned int entt);
extern inline const ev_Vector3* entity_get_position(unsigned int entt);

extern inline ev_Vector3* entity_get_rotation_mut(unsigned int entt);
extern inline const ev_Vector3* entity_get_rotation(unsigned int entt);

#endif
