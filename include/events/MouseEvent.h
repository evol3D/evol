#ifndef EVOL_MOUSEEVENT_H
#define EVOL_MOUSEEVENT_H
#include "Event.h"

typedef unsigned int MouseButton;
typedef struct {
  double x, y;
} MousePosition;

REGISTER_EVENT(MouseEvent, EVENT_STRUCT());
REGISTER_EVENT(MouseButtonPressedEvent, EVENT_STRUCT(
      MouseButton button;
      unsigned int mods;
      ));
REGISTER_EVENT(MouseButtonReleasedEvent, EVENT_STRUCT(
      MouseButton button;
      unsigned int mods;
      ));
REGISTER_EVENT(MouseMovedEvent, EVENT_STRUCT(
      MousePosition cursorPosition;
      ));

#define INIT_MOUSE_EVENTS \
  INIT_EVENT_PRIMARY(MouseEvent); \
  INIT_EVENT_SECONDARY(MouseButtonPressedEvent, MouseEvent); \
  INIT_EVENT_SECONDARY(MouseButtonReleasedEvent, MouseEvent); \
  INIT_EVENT_SECONDARY(MouseMovedEvent, MouseEvent);

#endif //EVOL_MOUSEEVENT_H
