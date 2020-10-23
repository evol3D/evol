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
extern const ev_Matrix4* entity_getWorldTransform(unsigned int entt);
extern void entity_setWorldTransform(unsigned int entt, ev_Matrix4 newWorldTransform);



#endif
