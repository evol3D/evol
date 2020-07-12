#ifndef EVOL_APP_H
#define EVOL_APP_H

extern struct ev_app_struct{
    const char *name;
    int (*start)();
    int (*destroy)();
    double lastFrameTime;
    double lastWindowPollTime;
    double lastEventSystemUpdate;
    unsigned int framerate;
    unsigned int windowPollRate;
    unsigned int eventSystemUpdateRate;
} App;

#endif //EVOL_APP_H
