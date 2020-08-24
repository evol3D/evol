#ifndef EVOL_GAME_H
#define EVOL_GAME_H

#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"

extern struct ev_Game {
    int (*init)();
    int (*deinit)();
} Game;

#endif //EVOL_GAME_H
