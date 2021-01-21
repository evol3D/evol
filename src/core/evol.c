/*!
 * \file evol.c
 */
#include <evol/core/evol.h>
#include <cargs.h>
#include <string.h>
#include <stdlib.h>

#include <evol/common/ev_log.h>
#include <evol/common/ev_globals.h>

#include <evol/core/lualoader.h>
#include <evol/core/modulemanager.h>

#include <evol/evolmod.h>

#include <evolpthreads.h>

#include <evol/meta/configvars.h>
#include <time.h>

typedef struct evconfig {
  char        name[EV_ENGINE_NAME_MAXLEN + 1];
  U32         version_minor;
  U32         version_major;
  const char *configfile;
  char        module_directory[EV_ENGINE_MODDIR_MAXLEN + 1];
  sdsvec_t    startmods;
} evconfig_t;

#define EVCONFIG_DEFAULT                                                       \
  (evconfig_t)                                                                 \
  {                                                                            \
    .name = "Hello, World", .version_minor = 1, .version_major = 0,            \
    .configfile = NULL, .module_directory = "./modules", .startmods = NULL,    \
  }

struct evolengine {
  evconfig_t config;
  vec_t      startmods_handles;
  vec_t      startmods_threadhandles;
};

evolengine_t *
evol_create()
{
  evolengine_t *evengine = calloc(sizeof(evolengine_t), 1);
  if (evengine) {
    evengine->config = EVCONFIG_DEFAULT;
    evengine->config.startmods = sdsvec_init();
    evengine->startmods_handles = vec_init(evolmodule_t, 0, 0);
    evengine->startmods_threadhandles = vec_init(pthread_t, 0, 0);
  }
  return evengine;
}

void
evol_destroy(evolengine_t *evengine)
{
  ev_log_debug("Destroying evol instance");
  if (!evengine)
    return;

  if (evengine->config.configfile) {
    free((void *)evengine->config.configfile);
  }

  if (evengine->config.startmods) {
    vec_fini(evengine->config.startmods);
  }

  if (evengine->startmods_handles) {
    vec_fini(evengine->startmods_handles);
  }

  if (evengine->startmods_threadhandles) {
    vec_fini(evengine->startmods_threadhandles);
  }

  free(evengine);
  ev_log_trace("Free'd memory used by the instance");
}

EvEngineResult
evol_config_loadlua(evolengine_t *evengine)
{
  EvLuaLoaderResult lua_res = ev_lua_runfile(evengine->config.configfile);

  if (lua_res != EV_LUALOADER_SUCCESS) {
    ev_log_error("LuaLoader error: %s", RES_STRING(EvLuaLoaderResult, lua_res));
  }

  ev_log_trace("Reading config file");

  const char *name;
  ev_lua_getvar_s(CONFIGVAR_NAME, name);
  if (name) {
    strncpy(evengine->config.name, name, EV_ENGINE_NAME_MAXLEN);
    ev_log_info("Name: %s", name);
  }

  const char *module_directory;
  ev_lua_getvar_s(CONFIGVAR_MODDIR, module_directory);
  if (module_directory) {
    strncpy(evengine->config.module_directory,
            module_directory,
            EV_ENGINE_MODDIR_MAXLEN);
    ev_log_info("Module directory: %s", module_directory);
  }

  EvLuaLoaderResult startmods_res = ev_lua_getvar(CONFIGVAR_STARTMODS, evengine->config.startmods);
  if(startmods_res != EV_LUALOADER_SUCCESS) {
    // TODO do something
  }

  return EV_ENGINE_SUCCESS;
}

EvEngineResult
evol_init(evolengine_t *evengine)
{
  EvLuaLoaderResult luaLoader_init_result = ev_lua_init();
  if (luaLoader_init_result != EV_LUALOADER_SUCCESS) {
    ev_log_error("LuaLoader error: %s",
                 RES_STRING(EvLuaLoaderResult, luaLoader_init_result));
    return EV_ENGINE_ERROR_LUA;
  }

  if (evengine->config.configfile) {
    EvEngineResult luaConfig_load_result = evol_config_loadlua(evengine);
    if (luaConfig_load_result != EV_ENGINE_SUCCESS)
      return luaConfig_load_result;
  }

  EvModuleManagerResult modulemanager_det_result =
    ev_modulemanager_detect(evengine->config.module_directory);
  if (modulemanager_det_result != EV_MODULEMANAGER_SUCCESS) {
    ev_log_error("ModuleManager error: %s",
                 RES_STRING(EvModuleManagerResult, modulemanager_det_result));
    return EV_ENGINE_ERROR_MODULEMANAGER;
  }

  return EV_ENGINE_SUCCESS;
}

void
evol_deinit(evolengine_t *evengine)
{
  if (!evengine)
    return;

  size_t startmods_len = vec_len(evengine->startmods_handles);
  if(startmods_len > 0) {
    for(size_t i = 0; i < startmods_len; ++i) {
      pthread_join(((pthread_t*)evengine->startmods_threadhandles)[i], NULL);
      evol_unloadmodule(((evolmodule_t*)evengine->startmods_handles)[i]);
    }
  }

  if (ev_lua_isactive()) {
    ev_lua_deinit();
  }
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
      ev_log_debug("Command line option '%c' found", 'c');
      const char *passed_config = cag_option_get_value(&context);
      if (passed_config) {
        ev_log_debug("Option '%c' has value '%s'", 'c', passed_config);
        const char *configfile = strdup(passed_config);
        if (!configfile)
          return EV_ENGINE_ERROR_OOM;

        evengine->config.configfile = configfile;
      } else {
        ev_log_warn("No value found for option '%c'. Ignoring...", 'c');
      }
      break;
    }
    default:
      printf("Unhandled option, Ignoring...\n");
    }
  }

  return EV_ENGINE_SUCCESS;
}

inline evolmodule_t
evol_loadmodule(const char *modquery)
{
  return ev_modulemanager_openmodule(modquery);
}

inline void
evol_unloadmodule(evolmodule_t module)
{
  if (!module)
    return;
  ev_module_close(module);
}

inline FN_PTR
evol_getmodfunc(evolmodule_t module, const char *func_name)
{
  return ev_module_getfn(module, func_name);
}

EvEngineResult
evol_start(evolengine_t *engine)
{
  if(vec_len(engine->config.startmods) == 0)
    return EV_ENGINE_SUCCESS;

  sdsvec_t startmods = engine->config.startmods;
  for(size_t i = 0; i < vec_len(startmods); ++i) {
    sds mod = ((sds*)startmods)[i];
    evolmodule_t handle = evol_loadmodule(mod);

    if(!handle) {
      continue;
    }

    vec_push(&(engine->startmods_handles), &handle);
    FN_PTR start_fn = (FN_PTR)evol_getmodfunc(handle, EV_STRINGIZE(EV_START_FN_NAME));

    if(!start_fn) {
      continue;
    }

    pthread_t mod_thr;
    pthread_create(&mod_thr, NULL, (void*(*)(void*))start_fn, NULL);
    vec_push(&(engine->startmods_threadhandles), &mod_thr);
  }
}
