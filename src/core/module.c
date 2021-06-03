/*!
 * \file module.c
 */
#include <evol/core/module.h>
#include <evol/common/ev_log.h>

#if defined(EV_CC_GCC)
#include <dlfcn.h>
#elif defined(EV_CC_MSVC)
#include <Windows.h>
#endif

evolmodule_t
ev_module_open(
  CONST_STR modpath)
{
  evolmodule_t res = 0;
#if defined(EV_CC_GCC)
  res = dlopen(modpath, RTLD_LAZY);
  if(!res) {
    ev_log_error("Couldn't load %s. Error: %s", modpath, dlerror());
  }

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

PTR
ev_module_gethandle(
    CONST_STR modpath)
{
  evolmodule_t res = 0;
#if defined(EV_CC_GCC)
  res = dlopen(modpath, RTLD_NOLOAD | RTLD_LAZY);
  if(res) {
    dlclose(res);
  }
#elif defined(EV_CC_MSVC)
  res = GetModuleHandle(modpath);
#else
#error("Unknown compiler. Can't get module handle")
#endif
  return res;
}


void
ev_module_close(
  evolmodule_t module)
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
ev_module_getfn(
  evolmodule_t module, 
  CONST_STR fn_name)
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
ev_module_getvar(
  evolmodule_t module, 
  CONST_STR var_name)
{
#if defined(EV_CC_GCC)
  return (PTR)dlsym(module, var_name);
#elif defined(EV_CC_MSVC)
  return (PTR)GetProcAddress(module, var_name);
#else
#error("Unknown compiler. This shouldn't be reachable")
#endif
}
