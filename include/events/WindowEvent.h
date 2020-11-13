#ifndef EVOL_WINDOWEVENT_H
#define EVOL_WINDOWEVENT_H
#include <Event.h>

// Primary
REGISTER_EVENT(WindowEvent, EVENT_STRUCT());

// Secondary
REGISTER_EVENT(WindowResizedEvent, EVENT_STRUCT(
    unsigned int newWindowWidth, newWindowHeight;
));

void INIT_WINDOW_EVENTS();

#endif //EVOL_WINDOWEVENT_H
