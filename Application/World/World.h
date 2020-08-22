#ifndef EVOL_WORLD_H
#define EVOL_WORLD_H

#include <flecs.h>

extern struct ev_World {
    int (*init)();
    int (*deinit)();
} World;


#endif //EVOL_WORLD_H
