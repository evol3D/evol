/*!
 * \file modulemanager.c
 */
#include <evol/core/modulemanager.h>
#include <evol/core/evstore.h>
#include <evol/common/ev_log.h>
#include <evol/core/evol.h>

#include <evol/utils/filesystem.h>
#include <evol/utils/lua_evutils.h>

// Should this check be for the compiler or the operating system?
// Or should we just change the library prefix/suffix from the
// buildsystem and pass them here through a configuration header
#if defined(EV_CC_MSVC)
#define LIB_EXTENSION ".dll"
#elif defined(EV_CC_GCC)
#define LIB_EXTENSION ".so"
#endif

struct ev_ModuleManagerData {
  lua_State *state;
} ModuleManagerData = {
  .state = NULL,
};

static const char modulesystem_script[] = 
#include <src/lua/modulesystem.lua.h>

EvModuleManagerResult
ev_modulemanager_init()
{
  ModuleManagerData.state = ev_lua_newState(true);
  assert(ModuleManagerData.state);

  ev_lua_runstring(ModuleManagerData.state, (CONST_STR) modulesystem_script);
}

void
ev_modulemanager_deinit()
{
  ev_lua_destroyState(&ModuleManagerData.state);
}

EvModuleManagerResult
ev_modulemanager_detect(
  CONST_STR module_dir)
{
  vec(evstring) modules = vec_init(evstring, evstring_veccopy, evstring_vecdestr);

  find_contains_r(module_dir, LIB_EXTENSION, &modules);

  evstring *iter;
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
      ev_lua_callfn(ModuleManagerData.state, "add_module", "ss>i", *iter, meta(), &res);
    } else {
      ev_log_warn("Module %s is not a valid evol module", *iter);
    }
    ev_module_close(mod);

    if (res) {
      // Do something?
    }
  }

  vec_fini(modules);

  bool valid = false;
  ev_lua_callfn(ModuleManagerData.state, "validate_modulesystem", ">b", &valid);
  if (!valid)
    return EV_MODULEMANAGER_ERROR_DEPENDENCY_INVALID;

  return EV_MODULEMANAGER_SUCCESS;
}

evolmodule_t
ev_modulemanager_openmodule_w_name(
  CONST_STR modname)
{
  evolmodule_t module = NULL;

  evstring modpath = NULL;
  ev_lua_callfn(ModuleManagerData.state, "get_module_with_name", "s>s", modname, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
  }

  evstring_free(modpath);

  return module;
}

evolmodule_t
ev_modulemanager_openmodule_w_category(
  CONST_STR modcategory)
{
  evolmodule_t module = NULL;

  evstring modpath = NULL;
  ev_lua_callfn(ModuleManagerData.state, "get_module_with_category", "s>s", modcategory, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
  }

  evstring_free(modpath);

  return module;
}

evolmodule_t
ev_modulemanager_openmodule(
  CONST_STR module_query)
{
  evolmodule_t module = NULL;
  evstring modpath = NULL;
  ev_lua_callfn(ModuleManagerData.state, "get_module", "s>s", module_query, &modpath);

  if (modpath) {
    module = ev_module_open(modpath);
    evstring_free(modpath);
  }

  return module;
}
