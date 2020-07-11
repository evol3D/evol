#ifndef EVOL_MOUSEEVENT_H
#define EVOL_MOUSEEVENT_H

#include "Input\Input.h"

enum MouseEventVariant {
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
};

typedef struct {
    MousePosition position;

    MouseButton button;
    unsigned int mods;
} MouseEventData;

typedef struct {
    unsigned int type;
    enum MouseEventVariant variant;
    MouseEventData data;
} MouseEvent;

#define CreateMouseEvent(v,d) \
     { \
        .type = MOUSE_EVENT,\
        .variant = v,\
        .data = d,\
     }

#endif //EVOL_MOUSEEVENT_H
