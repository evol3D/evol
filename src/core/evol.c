/*!
 * \file evol.c
 */
#include <evol/core/evol.h>
#include <evol/core/eventsystem.h>
#include <cargs.h>
#include <string.h>
#include <stdlib.h>

#include <evol/common/ev_log.h>
#include <evol/common/ev_globals.h>

#include <evol/core/lualoader.h>
#include <evol/core/modulemanager.h>

#include <evol/evolmod.h>

#include <evolpthreads.h>

#include <evol/meta/strings.h>
#include <evol/meta/configvars.h>
#include <time.h>

evstore_t *GLOBAL_STORE = NULL;

struct evolengine {
  U32 dummy;
};

evolengine_t *
evol_create()
{
  evolengine_t *evengine = calloc(sizeof(evolengine_t), 1);
  if (evengine) {

    GLOBAL_STORE = evstore_create();

#include <evol/meta/defaults.h>
  }

  return evengine;
}

void
evol_destroy(evolengine_t *evengine)
{
  ev_log_debug("Destroying evol instance");
  if (!evengine)
    return;

  evstore_destroy(GLOBAL_STORE);

  free(evengine);
  ev_log_trace("Free'd memory used by the instance");
}

EvEngineResult
evol_config_loadlua(CONST_STR configfile)
{
  EvLuaLoaderResult lua_res = ev_lua_runfile(configfile);

  if (lua_res != EV_LUALOADER_SUCCESS) {
    ev_log_error("LuaLoader error: %s", RES_STRING(EvLuaLoaderResult, lua_res));
  }

  ev_log_trace("Reading config file");

  const char *name;
  ev_lua_getvar_s(CONFIGVAR_NAME, name);
  if (name) {
    evstore_set(GLOBAL_STORE, &(evstore_entry_t) {
        .key  = EV_CORE_NAME,
        .type = EV_TYPE_STR,
        .data = sdsnew(name),
        .free = sdsfree,
    });
  }

  const char *module_directory;
  ev_lua_getvar_s(CONFIGVAR_MODDIR, module_directory);
  if (module_directory) {
    evstore_set(GLOBAL_STORE, &(evstore_entry_t) {
        .key  = EV_CORE_MODULEDIR,
        .type = EV_TYPE_STR,
        .data = sdsnew(module_directory),
        .free = sdsfree,
    });
  }
  return EV_ENGINE_SUCCESS;
}

EvEngineResult
evol_init(evolengine_t *evengine)
{
  I32 res = 0;

  //TODO Error checking
  EventSystem.init();

  EvLuaLoaderResult luaLoader_init_result = ev_lua_init();
  if (luaLoader_init_result != EV_LUALOADER_SUCCESS) {
    ev_log_error("LuaLoader error: %s",
                 RES_STRING(EvLuaLoaderResult, luaLoader_init_result));
    return EV_ENGINE_ERROR_LUA;
  }

  evstore_entry_t configfile;
  res = evstore_get(GLOBAL_STORE, EV_CORE_CONFIGPATH, &configfile);

  if (res == EV_STORE_ENTRY_FOUND) {
    EvEngineResult luaConfig_load_result = evol_config_loadlua(configfile.data);
    if (luaConfig_load_result != EV_ENGINE_SUCCESS)
      return luaConfig_load_result;
  }

  evstore_entry_t moduledir;
  res = evstore_get_checktype(GLOBAL_STORE, EV_CORE_MODULEDIR, EV_TYPE_STR, &moduledir);

  if(res == EV_STORE_ENTRY_FOUND) {
    EvModuleManagerResult modulemanager_det_result =
      ev_modulemanager_detect(moduledir.data);
    if (modulemanager_det_result != EV_MODULEMANAGER_SUCCESS) {
      ev_log_error("ModuleManager error: %s",
                  RES_STRING(EvModuleManagerResult, modulemanager_det_result));
      return EV_ENGINE_ERROR_MODULEMANAGER;
    }
  } else {
    ev_log_error("Module directory not specified");
  }

  return EV_ENGINE_SUCCESS;
}

void
evol_deinit(evolengine_t *evengine)
{
  if (!evengine)
    return;

  if (ev_lua_isactive()) {
    ev_lua_deinit();
  }

  EventSystem.deinit();
}

EvEngineResult
evol_parse_args(evolengine_t *evengine, int argc, char **argv)
{
  if (argc <= 1)
    return EV_ENGINE_SUCCESS;

  static struct cag_option options[] = {
#include <evol/meta/cliargs.txt>
  };

  cag_option_context context;
  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

  while (cag_option_fetch(&context)) {
    switch (cag_option_get(&context)) {
    case 'c': {
      CONST_STR passed_config = cag_option_get_value(&context);
      if (passed_config) {
        ev_log_debug("Option '%c' has value '%s'", 'c', passed_config);

        evstore_set(GLOBAL_STORE, &(evstore_entry_t){
            .key  = EV_CORE_CONFIGPATH,
            .type = EV_TYPE_STR,
            .data = sdsnew(passed_config),
            .free = sdsfree,
        });
      } else {
        ev_log_warn("No value found for option '%c'. Ignoring...", 'c');
      }
      break;
    }
    default:
      ev_log_warn("Unhandled option, Ignoring...");
    }
  }

  return EV_ENGINE_SUCCESS;
}

evolmodule_t
evol_loadmodule(CONST_STR modquery)
{
  return ev_modulemanager_openmodule(modquery);
}

void
evol_unloadmodule(evolmodule_t module)
{
  if (!module)
    return;
  ev_module_close(module);
}

FN_PTR
evol_getmodfunc(evolmodule_t module, CONST_STR func_name)
{
  return ev_module_getfn(module, func_name);
}
