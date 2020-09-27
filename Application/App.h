#ifndef EVOL_APP_H
#define EVOL_APP_H

#include "Window/Window.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include "EventDebug/EventDebug.h"
#include "World/World.h"
#include "Scratchpad/Scratchpad.h"
#include "Input/Input.h"
#include "Game/Game.h"
/* #include "Physics/Physics.h" */

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
