#ifndef EVOL_KEYEVENT_H
#define EVOL_KEYEVENT_H

#include "Event.h"

typedef unsigned int KeyCode;

REGISTER_EVENT(KeyEvent, EVENT_STRUCT());
REGISTER_EVENT(KeyPressedEvent, EVENT_STRUCT(
      KeyCode keyCode;
      unsigned int mods;
));
REGISTER_EVENT(KeyReleasedEvent, EVENT_STRUCT(
      KeyCode keyCode;
      unsigned int mods;
));
REGISTER_EVENT(KeyRepeatEvent, EVENT_STRUCT(
      KeyCode keyCode;
      unsigned int mods;
));

#define INIT_KEY_EVENTS \
  INIT_EVENT_PRIMARY(KeyEvent); \
  INIT_EVENT_SECONDARY(KeyPressedEvent, KeyEvent); \
  INIT_EVENT_SECONDARY(KeyReleasedEvent, KeyEvent); \
  INIT_EVENT_SECONDARY(KeyRepeatEvent, KeyEvent);

#endif //EVOL_KEYEVENT_H
