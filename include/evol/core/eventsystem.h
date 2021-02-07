#pragma once

#include <evol/core/eventlistener.h>
#include <evol/core/event.h>

#include <evol/common/ev_types.h>
#include <evol/common/ev_macros.h>

#include <evol/common/ev_log.h>

#include <evolpthreads.h>
#include <evolthreadpool.h>

#define DISPATCH_EVENT(T, ...) \
  if(!EVENT_TYPE(T)) { \
    ev_log_error("EventType \"%s\" registered but not initalized\n", #T); \
  } \
  else { \
    if(GET_SECONDARY_TYPE(EVENT_TYPE(T))) { \
      WRAP_EVENT(T, __VA_ARGS__); \
      EventSystem.dispatch(wrapper); \
    } \
  }


extern struct ev_eventsystem {
  I32 (*dispatch)(ev_event_t);
  I32 (*init)(void);
  I32 (*deinit)(void);
  I32 (*subscribe)(ev_eventlistener_t *);
  I32 (*progress)(void);
  U32 (*sync)(void);


  vec(dvec(ev_event_t)) buffers;
  vec(vec(ev_eventlistener_t)) listeners;

  ev_tpool_t *handlers_tpool;
  vec(pthread_mutex_t) buffer_write_mutex;
} EventSystem;
