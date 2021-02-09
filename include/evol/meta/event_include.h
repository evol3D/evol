/*!
 * \file event_include.h
 */
#include <evol/common/ev_macros.h>
#include <evol/core/event.h>
#include <evol/core/module.h>

#if !defined(EVENT_MODULE)
#error "EVENT_MODULE needs to be defined"
#endif

#if defined(PRIMARY)
#undef PRIMARY
#endif
#if defined(SECONDARY)
#undef SECONDARY
#endif

#define PRIMARY EVENT_DECLARE_PRIMARY
#define SECONDARY EVENT_DECLARE_SECONDARY
#include EV_STRINGIZE(EVENT_MODULE/meta/evmod.events)
#undef SECONDARY
#undef PRIMARY

#define EVENT_IMPORT_FN_NAME EV_CONCAT(IMPORT_EVENTS_, EVENT_MODULE)
void EVENT_IMPORT_FN_NAME(evolmodule_t mod)
{
#define PRIMARY(T, ...) \
  EVENT_TYPE(T) = *(ev_eventtype_t *)evol_getmodvar(mod, EV_STRINGIZE(EVENT_TYPE(T)));

#define SECONDARY(P, T, ...) \
  EVENT_TYPE(T) = *(ev_eventtype_t *)evol_getmodvar(mod, EV_STRINGIZE(EVENT_TYPE(T)));

#include EV_STRINGIZE(EVENT_MODULE/meta/evmod.events)
#undef SECONDARY
#undef PRIMARY
}

#undef EVENT_IMPORT_FN_NAME
#undef EVENT_MODULE
