#include "MouseEvent.h"

EVENT_IMPL(MouseEvent)
EVENT_IMPL(MouseButtonPressedEvent)

EVENT_IMPL(MouseButtonReleasedEvent)
EVENT_IMPL(MouseMovedEvent)

void INIT_MOUSE_EVENTS()
{
  INIT_EVENT_PRIMARY(MouseEvent);
  INIT_EVENT_SECONDARY(MouseButtonPressedEvent, MouseEvent);
  INIT_EVENT_SECONDARY(MouseButtonReleasedEvent, MouseEvent);
  INIT_EVENT_SECONDARY(MouseMovedEvent, MouseEvent);
}

