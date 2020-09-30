#ifndef EVOL_GAME_H
#define EVOL_GAME_H

#include "assert.h"
#include "App.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include "types.h"

extern struct ev_Game {
    int (*init)();
    int (*deinit)();
    void (*loop)();
} Game;

#endif //EVOL_GAME_H
