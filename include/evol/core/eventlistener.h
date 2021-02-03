#pragma once

#include <evol/core/event.h>

typedef struct ev_eventlistener {
  ev_eventtype_t type;
  FN_PTR handler;
} ev_eventlistener_t;

#define DECLARE_EVENT_LISTENER(listener_name, type, ...) \
  void listener_name##_function __VA_ARGS__ \
  ev_eventlistener_t lis_##handler_name = { \
    .type = EVENT_TYPE(type), \
    .handler = listener_name##_function \
  };

#define ACTIVATE_EVENT_LISTENER(handler_name) \
  EventSystem.subscribe(&lis_##handler_name);
