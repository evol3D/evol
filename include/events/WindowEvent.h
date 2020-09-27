#ifndef EVOL_WINDOWEVENT_H
#define EVOL_WINDOWEVENT_H
#include <Event.h>

// Primary
REGISTER_EVENT(WindowEvent, EVENT_STRUCT());

// Secondary
REGISTER_EVENT(WindowResizedEvent, EVENT_STRUCT(
      unsigned int newWindowWidth, newWindowHeight
));

#define INIT_WINDOW_EVENTS \
  INIT_EVENT_PRIMARY(WindowEvent); \
  INIT_EVENT_SECONDARY(WindowResizedEvent, WindowEvent);

#endif //EVOL_WINDOWEVENT_H
