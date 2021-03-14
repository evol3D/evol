/*!
 * \file evol.c
 */
#define EV_CORE_FILE
#include <evol/core/evol.h>
#include <evol/core/eventsystem.h>
#include <cargs.h>
#include <string.h>
#include <stdlib.h>
#include <hashmap.h>


#include <evol/common/ev_log.h>
#include <evol/common/ev_globals.h>

#include <evol/core/lualoader.h>
#include <evol/core/modulemanager.h>
#include <evol/core/namespace.h>

#include <evol/evolmod.h>

#include <evolpthreads.h>

#include <evol/meta/strings.h>
#include <evol/meta/configvars.h>
#include <time.h>

evstore_t *GLOBAL_STORE = NULL;

struct evolengine {
  struct hashmap *namespaces;
};

evolengine_t *
evol_create()
{
  evolengine_t *evengine = calloc(sizeof(evolengine_t), 1);
  if (evengine) {
    GLOBAL_STORE = evstore_create();
    evengine->namespaces = hashmap_new(sizeof(NS), 8, 0, 0, NS_hash, NS_cmp, NULL);
  }

  evstore_entry_t evol_instance = {
    .key = "EVOL_INSTANCE",
    .type = EV_TYPE_PTR,
    .data = evengine,
    .free = NULL,
  };
  evstore_set(GLOBAL_STORE, &evol_instance);
  // NOTE should handle oom but this runs so early in the lifetime of the process that an oom is very unlikely

  return evengine;
}

void
evol_destroy(evolengine_t *evengine)
{
  ev_log_debug("Destroying evol instance");
  if (!evengine)
    return;

  evstore_destroy(GLOBAL_STORE);

  if(evengine->namespaces) {
    hashmap_scan(evengine->namespaces, NS_free, NULL);
    hashmap_free(evengine->namespaces);
  }

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

  return EV_ENGINE_SUCCESS;
}

void evol_loadconfigvars(void)
{
  CONFIG_ENTRY(CONFIGVAR_NAME, EV_CORE_NAME, SDS, "evol-app")
  CONFIG_ENTRY(CONFIGVAR_MODDIR, EV_CORE_MODULEDIR, SDS, ".")
  CONFIG_ENTRY(CONFIGVAR_VERSION_MAJOR, EV_CORE_VERSION_MAJOR, U8, 0)
  CONFIG_ENTRY(CONFIGVAR_VERSION_MINOR, EV_CORE_VERSION_MINOR, U8, 1)

#ifdef EV_APP_CONFIG
#include EV_APP_CONFIG
#endif
}

EvEngineResult
evol_init(evolengine_t *evengine)
{
  I32 res = 0;

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

  evol_loadconfigvars();

  // Config loading should be the first thing to be done as multiple modules'
  // initialization depends on variables initialized in the config file

  //TODO Error checking
  EventSystem.init();

  evstore_entry_t moduledir;
  res = evstore_get_checktype(GLOBAL_STORE, EV_CORE_MODULEDIR, EV_TYPE_SDS, &moduledir);

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

PTR
evol_getmodvar(evolmodule_t module, CONST_STR var_name)
{
  return ev_module_getvar(module, var_name);
}

U32
evol_registerNS(evolengine_t *evengine, NS *ns)
{
  //TODO handle oom
  hashmap_set(evengine->namespaces, ns);
}

U32
evol_bindNSFunction(evolengine_t *evengine, STR nsName, STR fnName, FN_PTR fnHandle)
{
  NS  queryNS;
  queryNS.name = nsName;
  NS *ns = hashmap_get(evengine->namespaces, &queryNS);
  if(!ns)
    return -1;

  NSFn  queryFn;
  queryFn.name = fnName;
  NSFn *fn = hashmap_get(ns->functions, &queryFn);
  if(!fn)
    return -2;
  
  FunctionBind(fn, fnHandle);

  return 0;
}

FN_PTR
evol_getNSBinding(evolengine_t *evengine, STR nsName, STR fnName)
{
  NS  queryNS;
  queryNS.name = nsName;
  NS *ns = hashmap_get(evengine->namespaces, &queryNS);
  if(!ns)
    return -1;

  NSFn  queryFn;
  queryFn.name = fnName;
  NSFn *fn = hashmap_get(ns->functions, &queryFn);
  if(!fn)
    return -2;

  return fn->handle;
}
