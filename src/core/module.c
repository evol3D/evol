/*!
 * \file module.c
 */
#define EV_CORE_FILE
#include <evol/core/module.h>

#if defined(EV_CC_GCC)
#include <dlfcn.h>
#elif defined(EV_CC_MSVC)
#include <Windows.h>
#endif

evolmodule_t
ev_module_open(const char *modpath)
{
  evolmodule_t res = 0;
#if defined(EV_CC_GCC)
  res = dlopen(modpath, RTLD_LAZY);

#elif defined(EV_CC_MSVC)
  res = LoadLibrary(modpath);
  if (!res) {
    printf("%s couldn't be loaded, Error: %d\n", modpath, GetLastError());
  }

#else
#error("Unknown compiler. Can't open module")

#endif
  return res;
}

void
ev_module_close(evolmodule_t module)
{
#if defined(EV_CC_GCC)
  dlclose(module);
#elif defined(EV_CC_MSVC)
  FreeLibrary(module);
#else
#error("Unknown compiler. This shouldn't be reachable")
#endif
}

FN_PTR
ev_module_getfn(evolmodule_t module, const char *fn_name)
{
#if defined(EV_CC_GCC)
  return (FN_PTR)dlsym(module, fn_name);
#elif defined(EV_CC_MSVC)
  return (FN_PTR)GetProcAddress(module, fn_name);
#else
#error("Unknown compiler. This shouldn't be reachable")
#endif
}

PTR
ev_module_getvar(evolmodule_t module, const char *var_name)
{
#if defined(EV_CC_GCC)
  return (PTR)dlsym(module, var_name);
#elif defined(EV_CC_MSVC)
  return (PTR)GetProcAddress(module, var_name);
#else
#error("Unknown compiler. This shouldn't be reachable")
#endif
}
