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
#define PRIMARY EVENT_DECLARE_PRIMARY
#define SECONDARY EVENT_DECLARE_SECONDARY
#include <meta/evmod.events>

#define PRIMARY EVENT_DEFINE_PRIMARY
#define SECONDARY EVENT_DEFINE_SECONDARY
#include <meta/evmod.events>

EV_CONSTRUCTOR;
EV_DESTRUCTOR;

void STATIC_INIT()
{
#define PRIMARY EVENT_INIT_PRIMARY
#define SECONDARY EVENT_INIT_SECONDARY
#include <meta/evmod.events>
  EventSystem.sync();
}

void STATIC_DEINIT() {}

const char MODULE_DATA[] =
#include <module.lua.h>
;

EVMODAPI const char * getMetadata() { return MODULE_DATA; }


# if defined(EV_OS_WINDOWS)
# include <windows.h>

EVCOREAPI evstore_t *GLOBAL_STORE;

BOOL __stdcall DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
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


#endif
