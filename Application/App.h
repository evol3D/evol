#ifndef EVOL_APP_H
#define EVOL_APP_H

struct App {
    const char *name;
    int (*start)();
};

extern struct App app;

#endif //EVOL_APP_H
