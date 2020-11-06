#include "EngineEvent.h"

EVENT_IMPL(EngineEvent);

EVENT_IMPL(SceneUpdatedEvent);

void INIT_ENGINE_EVENTS()
{
  INIT_EVENT_PRIMARY(EngineEvent);
  INIT_EVENT_SECONDARY(SceneUpdatedEvent, EngineEvent);
}
