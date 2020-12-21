#pragma once

#include <evol/common/ev_macros.h>

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

#endif

#define EV_CONSTRUCTOR EV_EXPORT void EV_CONSTRUCTOR_FN_NAME()
#define EV_DESTRUCTOR  EV_EXPORT void EV_DESTRUCTOR_FN_NAME()
#define EV_START       EV_EXPORT void EV_START_FN_NAME()

#if defined(EV_MODULE_DEFINE)
EV_CONSTRUCTOR;
EV_DESTRUCTOR;

const char MODULE_DATA[] =
#include <module.lua.h>

const char * getMetadata() { return MODULE_DATA; }

# if defined(EV_OS_WINDOWS)
# include <windows.h>
BOOL DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    EV_CONSTRUCTOR_FN_NAME();
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    EV_DESTRUCTOR_FN_NAME();
    break;
  default:
    break;
  }
  return TRUE;
}


# elif defined(EV_OS_LINUX)

EV_CONSTRUCTOR_ATTR void evmod_constructor_fn() { EV_CONSTRUCTOR_FN_NAME(); }
EV_DESTRUCTOR_ATTR  void evmod_destructor_fn()  { EV_DESTRUCTOR_FN_NAME(); }

# else

# endif
#endif
