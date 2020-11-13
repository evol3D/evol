#ifndef EVOL_ENGINEEVENT_H
#define EVOL_ENGINEEVENT_H
#include <Event.h>

// Primary
REGISTER_EVENT(EngineEvent, EVENT_STRUCT());

// Secondary
REGISTER_EVENT(SceneUpdatedEvent, EVENT_STRUCT());

void INIT_ENGINE_EVENTS();

#endif //EVOL_ENGINEEVENT_H
