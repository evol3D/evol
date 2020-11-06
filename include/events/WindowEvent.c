#include "WindowEvent.h"
// Primary
EVENT_IMPL(WindowEvent)

// Secondary
EVENT_IMPL(WindowResizedEvent)

void INIT_WINDOW_EVENTS()
{
  INIT_EVENT_PRIMARY(WindowEvent);
  INIT_EVENT_SECONDARY(WindowResizedEvent, WindowEvent);
}
