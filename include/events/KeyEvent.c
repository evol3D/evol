#include "KeyEvent.h"

EVENT_IMPL(KeyEvent);

EVENT_IMPL(KeyPressedEvent);
EVENT_IMPL(KeyReleasedEvent);
EVENT_IMPL(KeyRepeatEvent);

void INIT_KEY_EVENTS()
{
  INIT_EVENT_PRIMARY(KeyEvent);
  INIT_EVENT_SECONDARY(KeyPressedEvent, KeyEvent);
  INIT_EVENT_SECONDARY(KeyReleasedEvent, KeyEvent);
  INIT_EVENT_SECONDARY(KeyRepeatEvent, KeyEvent);
}
