#include "evol/common/types/vec.h"
#include <evol/core/modulemanager.h>
#include <evol/core/lualoader.h>

#include <evol/utils/filesystem.h>

// Should this check be for the compiler or the operating system?
// Or should we just change the library prefix/suffix from the
// buildsystem and pass them here through a configuration header
#if defined(EV_CC_MSVC)
#define LIB_EXTENSION ".dll"
#elif defined(EV_CC_GCC)
#define LIB_EXTENSION ".so"
#endif

void
sdsveccopy(sds *dst, const sds *src)
{
  *dst = sdsnew(*src);
}

void
sdsvecdestr(sds *elem)
{
  sdsfree(*elem);
}

EvModuleManagerResult
ev_modulemanager_detect(const char *module_dir)
{
  vec_t modules = vec_init(sds, (elem_copy)sdsveccopy, (elem_destr)sdsvecdestr);

  find_contains_r(module_dir, LIB_EXTENSION, &modules);

#include <src/lua/modulesystem.lua.h>

  for (sds *iter = vec_iter_begin(modules); iter != vec_iter_end(modules);
       vec_iter_next(modules, (void **)&iter)) {
    int res = 0;

    evolmodule_t mod = ev_module_open(*iter);
    char *(*meta)() = (char*(*)())ev_module_getfn(mod, "getMetadata");
    if(meta) {
      ev_lua_callfn("add_module", "ss>i", *iter, meta(), &res);
    }
    ev_module_close(mod);

    if (res) {
      // Do something?
    }

    ev_lua_fnstack_pop();
  }

  vec_fini(modules);
  return EV_MODULEMANAGER_SUCCESS;
}

evolmodule_t
ev_modulemanager_openmodule_w_name(const char *modname)
{
  evolmodule_t module = NULL;

  char *modpath;
  ev_lua_callfn("get_module_with_name", "s>s", modname, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
  }

  ev_lua_fnstack_pop();
  return module;
}

evolmodule_t
ev_modulemanager_openmodule_w_category(const char *modcategory)
{
  evolmodule_t module = NULL;

  char *modpath;
  ev_lua_callfn("get_module_with_category", "s>s", modcategory, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
  }

  ev_lua_fnstack_pop();
  return module;
}

evolmodule_t
ev_modulemanager_openmodule(const char *module_query)
{
  evolmodule_t module = NULL;
  char *       modpath;
  ev_lua_callfn("get_module", "s>s", module_query, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
  }

  ev_lua_fnstack_pop();
  return module;
}
