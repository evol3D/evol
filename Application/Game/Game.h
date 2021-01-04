#ifndef EVOL_GAME_H
#define EVOL_GAME_H

extern struct ev_Game {
    int (*init)();
    int (*deinit)();
    void (*loop)();
    void (*setPlayer)(unsigned long player);
} Game;

#endif //EVOL_GAME_H
