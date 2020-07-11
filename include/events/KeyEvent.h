#ifndef EVOL_KEYEVENT_H
#define EVOL_KEYEVENT_H

#include "Input/Input.h"

enum KeyEventVariant {
    KeyPressed,
    KeyReleased,
    KeyRepeat,
};


typedef struct {
    KeyCode key;
    unsigned int mods;
} KeyEventData;

typedef struct {
    unsigned int type;
    enum KeyEventVariant variant;
    KeyEventData data;
} KeyEvent;

#define CreateKeyEvent(v,d) \
     { \
        .type = KEY_EVENT,\
        .variant = v,\
        .data = d,\
     }

#endif //EVOL_KEYEVENT_H
