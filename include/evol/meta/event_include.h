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

#define PRIMARY EVENT_DECLARE_PRIMARY_IMPORT
#define SECONDARY EVENT_DECLARE_SECONDARY_IMPORT
#include EV_STRINGIZE(EVENT_MODULE/meta/evmod.events)

#define EVENT_IMPORT_FN_NAME EV_CONCAT(IMPORT_EVENTS_, EVENT_MODULE)
void EVENT_IMPORT_FN_NAME(evolmodule_t mod)
{
#undef PRIMARY
#define PRIMARY(T, ...) \
  EVENT_TYPE(T) = *(ev_eventtype_t *)ev_module_getvar(mod, EV_STRINGIZE(EVENT_TYPE(T)));

#undef SECONDARY
#define SECONDARY(P, T, ...) \
  EVENT_TYPE(T) = *(ev_eventtype_t *)ev_module_getvar(mod, EV_STRINGIZE(EVENT_TYPE(T)));

#include EV_STRINGIZE(EVENT_MODULE/meta/evmod.events)
}

#undef SECONDARY
#undef PRIMARY
#undef EVENT_IMPORT_FN_NAME
#undef EVENT_MODULE
