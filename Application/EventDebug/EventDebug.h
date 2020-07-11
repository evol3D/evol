#ifndef EVOL_EVENTDEBUG_H
#define EVOL_EVENTDEBUG_H

#include "EventSystem.h"

extern struct ev_Event_Debug{
    struct EventListener listener;
    void (*init)();
    void (*deinit)();
} EventDebug;

#endif //EVOL_EVENTDEBUG_H
