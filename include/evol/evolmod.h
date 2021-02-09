#pragma once

#include <evol/evol.h>
#include <evol/core/evstore.h>
#include <evol/common/ev_types.h>
#include <evol/common/ev_macros.h>
#include <evol/core/event.h>
#include <evol/core/eventsystem.h>

#define EV_CONSTRUCTOR_FN_NAME ev_init
#define EV_DESTRUCTOR_FN_NAME  ev_fini
#define EV_START_FN_NAME       ev_start

#if defined(EV_OS_LINUX)
# define EV_CONSTRUCTOR_ATTR __attribute__((constructor))
# define EV_DESTRUCTOR_ATTR  __attribute__((destructor))
#elif defined(EV_OS_WINDOWS)
# define EV_CONSTRUCTOR_ATTR
# define EV_DESTRUCTOR_ATTR
#else
#error("Unsupported Operating System")
#endif

#define EV_CONSTRUCTOR EV_EXPORT I32 EV_CONSTRUCTOR_FN_NAME()
#define EV_DESTRUCTOR  EV_EXPORT I32 EV_DESTRUCTOR_FN_NAME()
#define EV_START       EV_EXPORT I32 EV_START_FN_NAME()

#if defined(EV_MODULE_DEFINE)
// Events

#if defined(PRIMARY)
#undef PRIMARY
#endif
#if defined(SECONDARY)
#undef SECONDARY
#endif

#if defined(TYPE)
#undef TYPE
#endif

#define PRIMARY EVENT_DECLARE_PRIMARY
#define SECONDARY EVENT_DECLARE_SECONDARY
#define TYPE(name, ...) typedef struct __VA_ARGS__ name;
#include <meta/evmod.events>
#undef TYPE
#undef SECONDARY
#undef PRIMARY

EV_CONSTRUCTOR;
EV_DESTRUCTOR;

void STATIC_INIT()
{
#define PRIMARY EVENT_INIT_PRIMARY
#define SECONDARY EVENT_INIT_SECONDARY
#define TYPE(name, ...)
#include <meta/evmod.events>
  EventSystem.sync();
}

#undef TYPE
#undef SECONDARY
#undef PRIMARY

void STATIC_DEINIT() {}

const char MODULE_DATA[] =
#include <module.lua.h>
;

EVMODAPI const char * getMetadata() { return MODULE_DATA; }


# if defined(EV_OS_WINDOWS)
# include <windows.h>

BOOL __stdcall DllMain( HMODULE _hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID _lpReserved)
{
  (void)_lpReserved,_hModule;
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    STATIC_INIT();
    EV_CONSTRUCTOR_FN_NAME();
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    EV_DESTRUCTOR_FN_NAME();
    STATIC_DEINIT();
    break;
  default:
    break;
  }
  return TRUE;
}
# else

EV_CONSTRUCTOR_ATTR void evmod_constructor_fn() { STATIC_INIT(); EV_CONSTRUCTOR_FN_NAME(); }
EV_DESTRUCTOR_ATTR  void evmod_destructor_fn()  { EV_DESTRUCTOR_FN_NAME(); STATIC_DEINIT(); }

# endif

#undef EV_MODULE_DEFINE
#endif
