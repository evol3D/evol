#ifndef EVOL_APP_H
#define EVOL_APP_H

#include "Window/Window.h"
#include "Input/Input.h"

#include "Physics/Physics.h"

#include "Vulkan/Vulkan.h"
#include "Renderer/Renderer.h"

#include "World/World.h"
#include "Game/Game.h"

#include "Asset/Asset.h"
#include "evolpthreads.h"
#include "ev_log/ev_log.h"

#include "EventSystem.h"
#ifdef DEBUG
# include "EventDebug/EventDebug.h"
#endif

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
