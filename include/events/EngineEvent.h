#ifndef EVOL_ENGINEEVENT_H
#define EVOL_ENGINEEVENT_H
#include <Event.h>

// Primary
REGISTER_EVENT(EngineEvent, EVENT_STRUCT());

// Secondary
REGISTER_EVENT(SceneUpdatedEvent, EVENT_STRUCT());

#define INIT_ENGINE_EVENTS \
  INIT_EVENT_PRIMARY(EngineEvent); \
  INIT_EVENT_SECONDARY(SceneUpdatedEvent, EngineEvent);

#endif //EVOL_ENGINEEVENT_H
