#pragma once

#include <evol/common/ev_types.h>
#include <evol/common/ev_macros.h>

typedef U32 ev_eventtype_t;

#define TOTAL_BITS  (sizeof(ev_eventtype_t) * 8)
#define PRIMARY_BITS ((TOTAL_BITS) / 2)
#define SECONDARY_BITS  ((TOTAL_BITS) - (PRIMARY_BITS))

#define PRIMARY_BITMASK   ((~((ev_eventtype_t)0)) << (SECONDARY_BITS))
#define SECONDARY_BITMASK  (~(PRIMARY_BITMASK))

typedef struct {
  ev_eventtype_t type;
  PTR data;
} ev_event_t;

EVCOREAPI extern ev_eventtype_t EVENT_TYPE_COUNT;
EVCOREAPI extern ev_eventtype_t PRIMARY_EVENT_TYPE_COUNT;
EVCOREAPI extern ev_eventtype_t SECONDARY_EVENT_TYPE_COUNT;

#define EVENT_TYPE(type) EVENT_TYPE_##type

#define EVENT_DECLARE_PRIMARY(...) \
  EV_CONCAT(EVENT_DECLARE_PRIMARY, EV_VA_ARGS_NARG(__VA_ARGS__))(__VA_ARGS__)

#define EVENT_DECLARE_PRIMARY1(T)                                              \
  typedef struct T {                                                           \
    ev_eventtype_t type;                                                       \
  } T;                                                                         \
  EVMODAPI ev_eventtype_t EVENT_TYPE(T);                                       \
  EVMODAPI ev_eventtype_t EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT);
#define EVENT_DECLARE_PRIMARY2 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY3 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY4 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY5 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY6 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY7 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY8 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY9 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY10 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY11 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY12 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY13 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY14 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARY15 EVENT_DECLARE_PRIMARYN
#define EVENT_DECLARE_PRIMARYN(T, ...)                                         \
  typedef struct T {                                                           \
    ev_eventtype_t type;                                                       \
    struct __VA_ARGS__;                                                        \
  } T;                                                                         \
  EVMODAPI ev_eventtype_t EVENT_TYPE(T);                                       \
  EVMODAPI ev_eventtype_t EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT);

#define EVENT_DECLARE_SECONDARY(...)                                             \
  EV_CONCAT(EVENT_DECLARE_SECONDARY, EV_VA_ARGS_NARG(__VA_ARGS__))(__VA_ARGS__)
#define EVENT_DECLARE_SECONDARY2(P, T) \
  typedef struct T {struct P;}; \
  EVMODAPI ev_eventtype_t EVENT_TYPE(T);
#define EVENT_DECLARE_SECONDARY3 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY4 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY5 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY6 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY7 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY8 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY9 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY10 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY11 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY12 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY13 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY14 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARY15 EVENT_DECLARE_SECONDARYN
#define EVENT_DECLARE_SECONDARYN(P, T, ...)                                                \
  typedef struct T {struct P; struct __VA_ARGS__;} T;               \
  EVMODAPI ev_eventtype_t EVENT_TYPE(T);

#define EVENT_INIT_PRIMARY(T, ...) do {                                           \
  EVENT_TYPE(T) = ((++PRIMARY_EVENT_TYPE_COUNT) << (SECONDARY_BITS));            \
  EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT) = 0;                                 \
  EVENT_TYPE_COUNT++;                                                        \
} while (0);

#define EVENT_INIT_SECONDARY(P, T, ...) do {                                      \
  EVENT_TYPE(T) = (++EV_CONCAT(EVENT_TYPE(P), _CHILDCOUNT)) | EVENT_TYPE(P); \
  SECONDARY_EVENT_TYPE_COUNT ++;                                                       \
  EVENT_TYPE_COUNT ++;                                                       \
} while (0);

#define GET_PRIMARY_TYPE(T) ((T) >> (SECONDARY_BITS))

#define GET_SECONDARY_TYPE(T) ((T) & (SECONDARY_BITMASK))

#define EVENT_CHILD_COUNT(T) (EV_CONCAT(EVENT_TYPE(T), _CHILDCOUNT))

#define EVENT_MATCH(A, B) \
   ((((PRIMARY_BITMASK) & A) == B) \
  ||(((PRIMARY_BITMASK) & B) == A) \
  || (A==B))

// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
// ===========================================================================//
//                                                                            //
// This macro should not be used directly by the user. They're only here for//
// other macros to use                                                        //
#include <assert.h>
#define WRAP_EVENT(T, ...) \
  T *e = malloc(sizeof(T)); \
  assert(e || !"WRAP_EVENT malloc failed"); \
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
