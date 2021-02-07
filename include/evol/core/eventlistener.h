#pragma once

#include <evol/core/event.h>

typedef struct ev_eventlistener {
  ev_eventtype_t type;
  FN_PTR handler;
} ev_eventlistener_t;

#define HANDLER_NAME(L) L##_eventsystem_handler_name

#define DECLARE_EVENT_LISTENER(listener_name, ...) \
  ev_eventlistener_t lis_##listener_name; \
  void HANDLER_NAME(listener_name) __VA_ARGS__

#define ACTIVATE_EVENT_LISTENER(listener, T) \
  lis_##listener.type = EVENT_TYPE(T); \
  lis_##listener.handler = HANDLER_NAME(listener); \
  EventSystem.subscribe(&lis_##listener);
