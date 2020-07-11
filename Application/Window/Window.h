#ifndef EVOL_WINDOW_H
#define EVOL_WINDOW_H

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_WINDOW_TITLE "Default Title"
#define MAX_WINDOW_TITLE_LENGTH 256

#include "globals.h"
#include "stdbool.h"

extern struct _ev_Window {
    int (*init)();
    int (*createWindow)();
    int (*setSize)(int, int);
    int (*deinit)();
    void *(*getWindowHandle)();
    int (*setTitle)(const char *);
    bool (*shouldClose)();
    void (*pollEvents)();
    bool (*isCreated)();
} Window;

#endif //EVOL_WINDOW_H
