/*!
 * \file modulemanager.c
 */
#define EV_CORE_FILE
#include <evol/core/modulemanager.h>
#include <evol/core/lualoader.h>
#include <evol/core/evstore.h>
#include <evol/common/ev_log.h>
#include <evol/core/evol.h>

#include <evol/utils/filesystem.h>

// Should this check be for the compiler or the operating system?
// Or should we just change the library prefix/suffix from the
// buildsystem and pass them here through a configuration header
#if defined(EV_CC_MSVC)
#define LIB_EXTENSION ".dll"
#elif defined(EV_CC_GCC)
#define LIB_EXTENSION ".so"
#endif

EvModuleManagerResult
ev_modulemanager_detect(const char *module_dir)
{
  sdsvec_t modules = sdsvec_init();

  find_contains_r(module_dir, LIB_EXTENSION, &modules);

#include <src/lua/modulesystem.lua.h>

  sds *iter;
  vec_foreach(iter, modules) {
    int res = 0;

    evolmodule_t mod = ev_module_open(*iter);
    if(!mod) {
      ev_log_warn("Module %s could not be loaded", *iter);
      continue;
    }
    ev_log_debug("Module %s loaded successfully", *iter);
    char *(*meta)()  = (char *(*)())ev_module_getfn(mod, "getMetadata");
    if (meta) {
      ev_log_debug("Module %s is a valid evol module", *iter);
      ev_lua_callfn("add_module", "ss>i", *iter, meta(), &res);
    } else {
      ev_log_warn("Module %s is not a valid evol module", *iter);
    }
    ev_module_close(mod);

    if (res) {
      // Do something?
    }

    ev_lua_fnstack_pop();
  }

  vec_fini(modules);

  bool valid = false;
  ev_lua_callfn("validate_modulesystem", ">b", &valid);
  if (!valid)
    return EV_MODULEMANAGER_ERROR_DEPENDENCY_INVALID;

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
