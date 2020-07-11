#ifndef EVOL_WINDOWEVENT_H
#define EVOL_WINDOWEVENT_H

enum WindowEventVariant {
    WindowResized,
};

typedef struct {
    int width;
    int height;
} WindowEventData;

typedef struct {
    unsigned int type;
    enum WindowEventVariant variant;
    WindowEventData data;
} WindowEvent;

#define CreateWindowEvent(v,d) \
     { \
        .type = WINDOW_EVENT,\
        .variant = v,\
        .data = d,\
     }

#endif //EVOL_WINDOWEVENT_H
