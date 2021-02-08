#pragma once

#include <evol/common/ev_types.h>

typedef U32 ev_eventtype_t;

#define TOTAL_BITS  sizeof(ev_eventtype_t) * 8
#define PRIMARY_BITS TOTAL_BITS / 2
#define SECONDARY_BITS  TOTAL_BITS - PRIMARY_BITS

#define PRIMARY_BITMASK   (~((ev_eventtype_t)0)) << SECONDARY_BITS
#define SECONDARY_BITMASK  ~PRIMARY_BITMASK

typedef struct {
  ev_eventtype_t type;
  PTR data;
} ev_event_t;

extern ev_eventtype_t EVENT_TYPE_COUNT;
extern ev_eventtype_t PRIMARY_EVENT_TYPE_COUNT;
extern ev_eventtype_t SECONDARY_EVENT_TYPE_COUNT;

#define EVENT_TYPE(type) EVENT_TYPE_##type

#define EVENT_DECLARE_PRIMARY_EXPORT(T, ...)                                                \
  typedef struct T {ev_eventtype_t type; struct __VA_ARGS__;} T;               \
  EV_EXPORT ev_eventtype_t EVENT_TYPE(T);                                       \
  EV_EXPORT ev_eventtype_t EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT);

#define EVENT_DECLARE_SECONDARY_EXPORT(P, T, ...)                                                \
  typedef struct T {struct P; struct __VA_ARGS__;} T;               \
  EV_EXPORT ev_eventtype_t EVENT_TYPE(T);

#define EVENT_DECLARE_PRIMARY_IMPORT(T, ...)                                                \
  typedef struct T {ev_eventtype_t type; struct __VA_ARGS__;} T;               \
  EV_IMPORT ev_eventtype_t EVENT_TYPE(T);                                       \
  EV_IMPORT ev_eventtype_t EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT);

#define EVENT_DECLARE_SECONDARY_IMPORT(P, T, ...)                                                \
  typedef struct T {struct P; struct __VA_ARGS__;} T;               \
  EV_IMPORT ev_eventtype_t EVENT_TYPE(T);                                       \

#define EVENT_DEFINE_PRIMARY(T, ...)                                                      \
  ev_eventtype_t EVENT_TYPE(T);                                              \
  ev_eventtype_t EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT);

#define EVENT_DEFINE_SECONDARY(P, T, ...)                                                      \
  ev_eventtype_t EVENT_TYPE(T);                                              \

#define EVENT_INIT_PRIMARY(T, ...) do {                                           \
  EVENT_TYPE(T) = (++PRIMARY_EVENT_TYPE_COUNT) << SECONDARY_BITS;            \
  EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT) = 0;                                 \
  EVENT_TYPE_COUNT++;                                                        \
} while (0);

#define EVENT_INIT_SECONDARY(P, T, ...) do {                                      \
  EVENT_TYPE(T) = (++EV_CONCAT(EVENT_TYPE(P), _CHILDCOUNT)) | EVENT_TYPE(P); \
  SECONDARY_EVENT_TYPE_COUNT ++;                                             \
  EVENT_TYPE_COUNT ++;                                                       \
} while (0);

#define GET_PRIMARY_TYPE(T) (T >> SECONDARY_BITS)
#define GET_SECONDARY_TYPE(T) (T & SECONDARY_BITMASK)

#define EVENT_CHILD_COUNT(T) (EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT))

#define EVENT_MATCH(A, B) \
   (((PRIMARY_BITMASK & A) == B) \
  ||((PRIMARY_BITMASK & B) == A) \
  || (A==B))

// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
//                                                                            //
// This macro should not be used directly by the user. They're only here for//
// other macros to use                                                        //

#define WRAP_EVENT(T, ...) \
  T *e = malloc(sizeof(T)); \
  *e = (T) __VA_ARGS__; \
  e->type = EVENT_TYPE(T); \
  ev_event_t wrapper = { \
    .type = EVENT_TYPE(T), \
    .data= (PTR) e \
  }

#define FREE_EVENT(wrapper) \
  free(wrapper.event)

// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
