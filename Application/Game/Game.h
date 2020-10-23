#ifndef EVOL_GAME_H
#define EVOL_GAME_H

extern struct ev_Game {
    int (*init)();
    int (*deinit)();
    void (*loop)();
} Game;

#endif //EVOL_GAME_H
