/*!
 * \file evol.c
 */
#include <evol/core/evol.h>
/* #include <evol/core/eventsystem.h> */
/* #include <evol/core/configloader.h> */
/* #include <cargs.h> */
/* #include <string.h> */
/* #include <stdlib.h> */
/* #include <hashmap.h> */

/* #include <evol/common/ev_log.h> */
/* #include <evol/common/ev_globals.h> */

/* #include <evol/common/ev_profile.h> */

/* #include <evol/core/modulemanager.h> */
/* #include <evol/core/namespace.h> */

/* #include <evol/evolmod.h> */

/* #include <evol/threads/evolpthreads.h> */

/* #include <time.h> */

/* #include <evjson.h> */

/* evstore_t *GLOBAL_STORE = NULL; */

/* struct evolengine { */
  /* struct hashmap *namespaces; */
  /* Remotery *rmt; */
/* }; */

/* EV_UNUSED void */
/* dummy_linker_force_import( */
/*     void) */
/* { */
/*   evjson_t *_json = evjs_init(); */
/*   evjs_fini(_json); */
/* } */

/* evolengine_t * */
/* evol_create() */
/* { */
  /* evolengine_t *evengine = calloc(sizeof(evolengine_t), 1); */
  /* if (evengine) { */
  /*   GLOBAL_STORE = evstore_create(); */
  /*   evengine->namespaces = hashmap_new(sizeof(NS), 8, 0, 0, NS_hash, NS_cmp, NULL); */
  /*   rmtError error = rmt_CreateGlobalInstance(&(evengine->rmt)); */
  /*   if(error != RMT_ERROR_NONE) { */
  /*     evengine->rmt = NULL; */
  /*   } */
  /* } */

  /* evstore_entry_t evol_instance = { */
  /*   .key = "EVOL_INSTANCE", */
  /*   .type = EV_TYPE_PTR, */
  /*   .data = evengine, */
  /*   .free = NULL, */
  /* }; */
  /* evstore_set(GLOBAL_STORE, &evol_instance); */
  /* // NOTE should handle oom but this runs so early in the lifetime of the process that an oom is very unlikely */

  /* evstore_entry_t core_active; */
  /* core_active.key = "EV_CORE_ACTIVE"; */
  /* core_active.type = EV_TYPE_I8; */
  /* core_active.data = 0; */
  /* core_active.free = EV_TYPE_FREE_I8; */

  /* evstore_set(GLOBAL_STORE, &core_active); */

  /* return evengine; */
/* } */

/* void */
/* evol_destroy( */
  /* evolengine_t *evengine) */
/* { */
  /* ev_log_debug("Destroying evol instance"); */
  /* if (!evengine) */
  /*   return; */

  /* evstore_destroy(GLOBAL_STORE); */

  /* if(evengine->namespaces) { */
  /*   hashmap_scan(evengine->namespaces, NS_free, NULL); */
  /*   hashmap_free(evengine->namespaces); */
  /* } */

  /* if(evengine->rmt) { */
  /*   rmt_DestroyGlobalInstance(evengine->rmt); */
  /* } */

  /* free(evengine); */
  /* ev_log_trace("Free'd memory used by the instance"); */
/* } */

/* EvEngineResult */
/* evol_init( */
  /* evolengine_t *evengine) */
/* { */
  /* (void) evengine; */
  /* I32 res = 0; */

  /* EvConfigLoaderResult configLoader_init_result = ev_configloader_init(); */
  /* if (configLoader_init_result != EV_CONFIGLOADER_SUCCESS) { */
  /*   ev_log_error("ConfigLoader error: %s", */
  /*                 RES_STRING(EvConfigLoaderResult, configLoader_init_result)); */
  /*   DEBUG_ASSERT(configLoader_init_result == EV_CONFIGLOADER_SUCCESS); */
  /*   return EV_ENGINE_ERROR_CONFIGLOADER; */
  /* } */
  /* EvModuleManagerResult moduleManager_init_result = ev_modulemanager_init(); */
  /* if (moduleManager_init_result != EV_MODULEMANAGER_SUCCESS) { */
  /*   ev_log_error("ModuleManager error: %s", */
  /*                 RES_STRING(EvModuleManagerResult, moduleManager_init_result)); */
  /*   DEBUG_ASSERT(moduleManager_init_result == EV_MODULEMANAGER_SUCCESS); */
  /*   return EV_ENGINE_ERROR_MODULEMANAGER; */  
  /* } */

  /* evstore_entry_t configfile; */
  /* res = evstore_get(GLOBAL_STORE, "EV_CORE_CONFIGPATH", &configfile); */

  /* if (res == EV_STORE_ENTRY_FOUND) { */
  /*   EvConfigLoaderResult configLoader_load_result = ev_configloader_loadfile(configfile.data); */
  /*   if (configLoader_load_result != EV_ENGINE_SUCCESS) { */
  /*     ev_log_error("ConfigLoader loading error: %s", RES_STRING(EvConfigLoaderResult, configLoader_load_result)); */
  /*     return EV_ENGINE_ERROR_CONFIGLOADER; */
  /*   } */
  /* } */

  /* // Config loading should be the first thing to be done as multiple modules' */
  /* // initialization depends on variables initialized in the config file */

  /* //TODO Error checking */
  /* EventSystem.init(); */

  /* evstring module_dir = NULL; */ 
  /* EvConfigLoaderResult configLoader_getmoddir_result = ev_configloader_get("module_dir", EV_TYPE_STRING, &module_dir); */

  /* if(configLoader_getmoddir_result == EV_CONFIGLOADER_SUCCESS) { */
  /*   ev_log_info("Module Directory Found: %s", module_dir); */
  /*   EvModuleManagerResult modulemanager_det_result = ev_modulemanager_detect(module_dir); */
  /*   if (modulemanager_det_result != EV_MODULEMANAGER_SUCCESS) { */
  /*     ev_log_error("ModuleManager error: %s", */
  /*                 RES_STRING(EvModuleManagerResult, modulemanager_det_result)); */
  /*     return EV_ENGINE_ERROR_MODULEMANAGER; */
  /*   } */
  /* } else { */
  /*   ev_log_error("module_dir couldn't be retrieved: %s", RES_STRING(EvConfigLoaderResult, configLoader_getmoddir_result)); */
  /*   ev_log_error("Module directory not specified"); */
  /* } */

  /* if(module_dir) { */
  /*   evstring_free(module_dir); */
  /* } */

  /* evstore_entry_t core_active; */
  /* core_active.key = "EV_CORE_ACTIVE"; */
  /* core_active.type = EV_TYPE_I8; */
  /* core_active.data = (PTR)1; */
  /* core_active.free = EV_TYPE_FREE_I8; */

  /* evstore_set(GLOBAL_STORE, &core_active); */

  /* return EV_ENGINE_SUCCESS; */
/* } */

/* void */
/* evol_deinit( */
  /* evolengine_t *evengine) */
/* { */
  /* (void)evengine; */
  /* if (!evengine) */
  /*   return; */

  /* ev_modulemanager_deinit(); */
  /* ev_configloader_deinit(); */

  /* evstore_entry_t core_active; */
  /* core_active.key = "EV_CORE_ACTIVE"; */
  /* core_active.type = EV_TYPE_I8; */
  /* core_active.data = (PTR)0; */
  /* core_active.free = EV_TYPE_FREE_I8; */

  /* evstore_set(GLOBAL_STORE, &core_active); */

  /* EventSystem.deinit(); */
/* } */

/* EvEngineResult */
/* evol_parse_args( */
  /* evolengine_t *evengine, */ 
  /* int argc, */ 
  /* char **argv) */
/* { */
  /* EV_UNUSED_PARAMS(evengine, argc, argv); */
  /* if (argc <= 1) */
  /*   return EV_ENGINE_SUCCESS; */

  /* static struct cag_option options[] = { */
/* #include <evol/meta/cliargs.txt> */
  /* }; */

  /* cag_option_context context; */
  /* cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv); */

  /* while (cag_option_fetch(&context)) { */
  /*   switch (cag_option_get(&context)) { */
  /*   case 'c': { */
  /*     CONST_STR passed_config = cag_option_get_value(&context); */
  /*     if (passed_config) { */
  /*       ev_log_debug("Option '%c' has value '%s'", 'c', passed_config); */

  /*       evstore_set(GLOBAL_STORE, &(evstore_entry_t){ */
  /*           .key  = "EV_CORE_CONFIGPATH", */
  /*           .type = EV_TYPE_STRING, */
  /*           .data = evstring_new(passed_config), */
  /*           .free = evstring_free, */
  /*       }); */
  /*     } else { */
  /*       ev_log_warn("No value found for option '%c'. Ignoring...", 'c'); */
  /*     } */
  /*     break; */
  /*   } */
  /*   default: */
  /*     ev_log_warn("Unhandled option, Ignoring..."); */
  /*   } */
  /* } */

  /* return EV_ENGINE_SUCCESS; */
/* } */

/* evolmodule_t */
/* evol_loadmodule( */
  /* CONST_STR modquery) */
/* { */
  /* EV_UNUSED_PARAM(modquery); */
  /* return ev_modulemanager_openmodule(modquery); */
  /* return 0; */
/* } */

/* EVCOREAPI evolmodule_t */
/* evol_loadmodule_weak( */
  /* const char *modquery) */
/* { */
  /* EV_UNUSED_PARAM(modquery); */
  /* return ev_modulemanager_getmodulehandle(modquery); */
  /* return 0; */
/* } */

/* void */
/* evol_unloadmodule( */
  /* evolmodule_t module) */
/* { */
  /* EV_UNUSED_PARAM(module); */
  /* if (!module) */
  /*   return; */
  /* ev_module_close(module); */
/* } */

/* FN_PTR */
/* evol_getmodfunc( */
  /* evolmodule_t module, */ 
  /* CONST_STR func_name) */
/* { */
  /* EV_UNUSED_PARAMS(module, func_name); */
  /* return ev_module_getfn(module, func_name); */
  /* return 0; */
/* } */

/* PTR */
/* evol_getmodvar( */
  /* evolmodule_t module, */ 
  /* CONST_STR var_name) */
/* { */
  /* EV_UNUSED_PARAMS(module, var_name); */
  /* return ev_module_getvar(module, var_name); */
  /* return 0; */
/* } */

/* U32 */
/* evol_registerNS( */
  /* evolengine_t *evengine, */ 
  /* NS *ns) */
/* { */
  /* EV_UNUSED_PARAMS(evengine, ns); */
  //TODO handle oom
  /* hashmap_set(evengine->namespaces, ns); */
/* } */

/* U32 */
/* evol_bindNSFunction( */
  /* evolengine_t *evengine, */ 
  /* STR nsName, */ 
  /* STR fnName, */ 
  /* FN_PTR fnHandle) */
/* { */
  /* EV_UNUSED_PARAMS(evengine, nsName, fnName, fnHandle); */
  /* NS  queryNS; */
  /* queryNS.name = nsName; */
  /* NS *ns = hashmap_get(evengine->namespaces, &queryNS); */
  /* if(!ns) */
  /*   return -1; */

  /* NSFn  queryFn; */
  /* queryFn.name = fnName; */
  /* NSFn *fn = hashmap_get(ns->functions, &queryFn); */
  /* if(!fn) */
  /*   return -2; */
  
  /* FunctionBind(fn, fnHandle); */

  /* return 0; */
/* } */

/* FN_PTR */
/* evol_getNSBinding( */
  /* evolengine_t *evengine, */ 
  /* STR nsName, */ 
  /* STR fnName) */
/* { */
  /* EV_UNUSED_PARAMS(evengine, nsName, fnName); */
  /* NS  queryNS; */
  /* queryNS.name = nsName; */
  /* NS *ns = hashmap_get(evengine->namespaces, &queryNS); */
  /* if(!ns) */
  /*   return -1; */

  /* NSFn  queryFn; */
  /* queryFn.name = fnName; */
  /* NSFn *fn = hashmap_get(ns->functions, &queryFn); */
  /* if(!fn) */
  /*   return -2; */

  /* return fn->handle; */
  /* return 0; */
/* } */
