#ifndef EVOL_GLOBAL_UTILS_HEADER
#define EVOL_GLOBAL_UTILS_HEADER

#include <types.h>

// Sleep in ms
void sleep_ms(double milliseconds);

// Get size of array that was declared in same scope
#ifndef ARRAYSIZE
#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])
#endif

#ifndef MIN
#define MIN(a,b) a>b?b:a
#endif
#ifndef MAX
#define MAX(a,b) a<b?b:a
#endif

// ECS Related
extern ev_Vector4* entity_get_position_mut(unsigned int entt);
extern const ev_Vector4* entity_get_position(unsigned int entt);

extern ev_Vector4* entity_get_rotation_mut(unsigned int entt);
extern const ev_Vector4* entity_get_rotation(unsigned int entt);

#endif
