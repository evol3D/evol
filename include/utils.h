#ifndef EVOL_GLOBAL_UTILS_HEADER
#define EVOL_GLOBAL_UTILS_HEADER

#include <types.h>

// Sleep in ms
void sleep_ms(double milliseconds);

// Get size of array that was declared in same scope
#ifndef ARRAYSIZE(x)
#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])
#endif

#ifndef MIN(a,b)
#define MIN(a,b) a>b?b:a
#endif
#ifndef MAX(a,b)
#define MAX(a,b) a<b?b:a
#endif

// ECS Related
extern inline ev_Vector3* entity_get_position_mut(unsigned int entt);
extern inline const ev_Vector3* entity_get_position(unsigned int entt);

extern inline ev_Vector3* entity_get_rotation_mut(unsigned int entt);
extern inline const ev_Vector3* entity_get_rotation(unsigned int entt);

#endif
