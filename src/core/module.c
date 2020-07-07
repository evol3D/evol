#include <evol/core/module.h>

#if defined(EV_CC_GCC)
#include <dlfcn.h>
#endif

evolmodule_t
ev_module_open(const char *modpath)
{
  evolmodule_t res = 0;
#if defined(EV_CC_GCC)
  res = dlopen(modpath, RTLD_LAZY);

#elif defined(EV_CC_MSVC)
#error("ev_module_open(...) not implemented for MSVC")

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
#error("ev_module_close(...) not implemented for MSVC")

#else
#error("Unknown compiler. This shouldn't be reachable")

#endif
}

inline void *
ev_module_getfn(evolmodule_t module, const char *fn_name)
{
#if defined(EV_CC_GCC)
  return dlsym(module, fn_name);
#elif defined(EV_CC_MSVC)
#error("ev_module_getfn(...) not implemented for MSVC")

#else
#error("Unknown compiler. This shouldn't be reachable")

#endif
}
