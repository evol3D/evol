#ifndef EVOL_CONTROLEVENT_H
#define EVOL_CONTROLEVENT_H
#include <Event.h>

REGISTER_EVENT(ControlEvent, EVENT_STRUCT());

#define INIT_CONTROL_EVENTS \
  INIT_EVENT_PRIMARY(ControlEvent);


/* enum ControlEventVariant { */
/*     NewFrame, */
/*     WindowPollSignal, */
/* }; */

/* typedef struct { */
/*     double timeStep; */
/* } ControlEventData; */

/* typedef struct { */
/*     unsigned int type; */
/*     enum ControlEventVariant variant; */
/*     ControlEventData data; */
/* } ControlEvent; */

/* #define CreateControlEvent(v,d) \ */
/*     { \ */
/*         .type = CONTROL_EVENT,\ */
/*         .variant = v,\ */
/*         .data = d,\ */
/*     } */

#endif //EVOL_CONTROLEVENT_H
