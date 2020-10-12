#ifndef EVOL_EVENTS_HEADER
#define EVOL_EVENTS_HEADER

#include "MouseEvent.h"
#include "ControlEvent.h"
#include "KeyEvent.h"
#include "WindowEvent.h"
#include "EngineEvent.h"

#define INIT_EVOL_EVENTS() \
  INIT_WINDOW_EVENTS \
  INIT_CONTROL_EVENTS \
  INIT_KEY_EVENTS \
  INIT_MOUSE_EVENTS \
  INIT_ENGINE_EVENTS \


#endif

