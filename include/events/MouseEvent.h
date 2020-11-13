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

void INIT_MOUSE_EVENTS();

#endif //EVOL_MOUSEEVENT_H
