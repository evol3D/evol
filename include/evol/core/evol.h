/*!
 * \file evol.h
 */
#pragma once

#include <evol/common/ev_types.h>
#include <evol/common/ev_macros.h>
#include <evol/core/evstore.h>
#include <evol/core/namespace.h>

typedef struct evolengine evolengine_t;

#define RES_PREFIX EV_ENGINE_
#define RES_FILE <evol/meta/results/engineresults.h>
#define RES_TYPE EvEngineResult
#include <evol/meta/resdef.h>

EVCOREAPI extern evstore_t *GLOBAL_STORE;

/*!
 * \brief Creates an engine instance and returns its reference.
 * \details The returned engine's config is the default config. (`EVCONFIG_DEFAULT`)
 *
 * \returns
 * - Pointer to new instance
 * - If creation failed, `NULL` is returned
 */
EVCOREAPI evolengine_t *
evol_create();

/*!
 * \brief Frees an `evolengine_t` instance
 * \details If the instance was initialized, then it should be deinitialized
 * before being destroyed.
 *
 * Sample Usage:
 * \code{.c}
 * evolengine_t *engine = evol_create();
 * evol_init(engine);
 * evol_deinit(engine);
 * evol_destroy(engine);
 * \endcode

  \param engine Pointer to the `evolengine_t` object to be destroyed
 */
EVCOREAPI void
evol_destroy(
  evolengine_t *engine);

/*!
  \brief Initializes an engine instance that was created using `evol_create()`

  \details Does the following:
  1. Initializes the lualoader
  2. If a config file was passed, it is loaded.
  3. Initializes the modulemanager (Detect modules in modules directory)

  Sample Usage:
  \code{.c}
  evolengine_t *engine = evol_create();
  EvEngineResult err = evol_init(engine);
  if(err) {
    // Error handling
  }
  \endcode

  \param engine Pointer to the `evolengine_t` object to initialize

  \returns
  - On success, returns `EV_ENGINE_SUCESS`
  - If a problem occured while initializing the lualoader, returns `EV_ENGINE_ERROR_LUA`
  - If a problem occured in the modulemanager, returns `EV_ENGINE_ERROR_MODULEMANAGER`
*/
EVCOREAPI EvEngineResult
evol_init(
  evolengine_t *engine);

/*!
 * \brief Parse commandline arguments into engine config
 *
 * \details Options for the arguments can be found at <evol/meta/cliargs.txt>
 *
 * \param engine Pointer to the `evolengine_t` object to edit using the passed args
 * \param argc Argument count
 * \param argv Array of string args
 *
 * \returns
 * - On success, returns `EV_ENGINE_SUCCESS`
 * - Currently the only way to fail is by an OOM error. In that case,
 *   `EV_ENGINE_ERROR_OOM` is returned
 */
EVCOREAPI EvEngineResult
evol_parse_args(
  evolengine_t *engine, 
  int argc, 
  char **argv);

/*!
 * \brief Deinitializes an `evolengine_t` instance
 *
 * \param engine Pointer to the `evolengine_t` object to deinitialized
 */
EVCOREAPI void
evol_deinit(
  evolengine_t *engine);

/*!
 * \brief Loads a module from the modules directory.
 *
 * \details The `modquery` can either be a category or a name. If it is the name
 * then the modulesystem will look for a module that has the same name. If it's
 * a category, then the modulesystem returns any module that matches the
 * category.
 *
 * Due to modules internally being dynamic libraries, the operating system keeps
 * a refcount for the number of places where they're used. Thus, calling this
 * function doesn't guarantee that that the module will be loaded into memory
 * as it may already be there, making the function simply return the same module
 * and increase its refcount.
 *
 * \param modquery Identifier for the module.
 *
 * \returns
 *   - If the module was found and successfully loaded, then it is returned.
 *   - If the process fails, `NULL` is returned.
 */
EVCOREAPI evolmodule_t
evol_loadmodule(
  const char *modquery);

/*!
 * \brief Unloads a module.
 *
 * \details Due to modules internally being dynamic libraries, the operating
 * system keeps a refcount for the number of places where they're used. So there
 * are no guarantees that calling this function will call the module's destructor
 * as it may be in use in other areas of the program.
 *
 * \remark It's highly recommended to call `evol_loadmodule(...)` and
 * `evol_unloadmodule(...)` in pairs and to never share the returned module
 * handles with external modules so that maintaining a defined behaviour is
 * easier.
 *
 * \param module The module to unload
 */
EVCOREAPI void
evol_unloadmodule(
  evolmodule_t module);

/*!
 * \brief Retrieve function from module
 *
 * \param module The module to search for the function
 * \param func_name Name of the function to load
 *
 * \returns
 * If the function was found, a function pointer is returned. Otherwise, `NULL`
 * is returned.
 */
EVCOREAPI FN_PTR
evol_getmodfunc(
  evolmodule_t module, 
  const char *func_name);

/*!
 * \brief Retrieve variable from module
 *
 * \param module The module to search for the variable
 * \param var_name Name of the variable to load
 *
 * \returns
 * If the variable was found, a pointer is returned. Otherwise, `NULL`
 * is returned.
 */
EVCOREAPI PTR
evol_getmodvar(
  evolmodule_t module, 
  const char *var_name);


EVCOREAPI U32
evol_registerNS(
  evolengine_t *evengine, NS *ns);

EVCOREAPI U32
evol_bindNSFunction(
  evolengine_t *evengine, 
  STR nsName, 
  STR fnName, 
  FN_PTR fnHandle);

EVCOREAPI FN_PTR
evol_getNSBinding(
  evolengine_t *evengine, 
  STR nsName, 
  STR fnName);

#define EV_CHECK_CORE_ACTIVE do {                                                \
  evstore_entry_t core_active;                                                   \
  EvStoreResult res = evstore_get(GLOBAL_STORE, "EV_CORE_ACTIVE", &core_active); \
  if(res == EV_STORE_ENTRY_NOTFOUND || core_active.data == 0)                    \
    return;                                                                      \
} while (0)
