#ifndef EVOL_CONTROLEVENT_H
#define EVOL_CONTROLEVENT_H

enum ControlEventVariant {
    NewFrame,
    WindowPollSignal,
};

typedef struct {
    double timeStep;
} ControlEventData;

typedef struct {
    unsigned int type;
    enum ControlEventVariant variant;
    ControlEventData data;
} ControlEvent;

#define CreateControlEvent(v,d) \
    { \
        .type = CONTROL_EVENT,\
        .variant = v,\
        .data = d,\
    }

#endif //EVOL_CONTROLEVENT_H
