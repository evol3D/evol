#ifndef EVOL_APP_H
#define EVOL_APP_H

#include <stdio.h>
#include <evolpthreads.h>
#include <stdbool.h>

extern struct ev_app_struct{
    const char *name;
    bool closeSystem;
    pthread_t eventsystem_thread;
    pthread_t gameloop_thread;
    int (*start)(int argc, char** argv);
    int (*destroy)();
    double lastFrameTime;
    unsigned int framerate;
    unsigned int windowPollRate;
    unsigned int eventSystemUpdateRate;
    FILE *logs;
} App;

#endif //EVOL_APP_H
