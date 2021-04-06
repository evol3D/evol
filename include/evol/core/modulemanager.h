/*!
 * \file modulemanager.h
 */
#pragma once

#include <evol/common/ev_types.h>

#include <evol/core/module.h>

#define RES_PREFIX EV_MODULEMANAGER_
#define RES_FILE <evol/meta/results/modulemanagerresults.h>
#define RES_TYPE EvModuleManagerResult
#include <evol/meta/resdef.h>

/*!
 * \brief Search module directory for valid modules and store them for future reference
 *
 * \details
 * Modules are considered valid if:
 * 1. They implement a `char *getMetadata()` function
 * 2. `getMetadata()` returns a string representing a valid Lua table
 *
 * After loading the metadata of all valid modules, dependencies are checked to
 * make sure that all module dependencies are fulfilled.
 *
 * \returns
 * - If successful, `EV_MODULEMANAGER_SUCCESS` is returned.
 * - If dependency validation fails, `EV_MODULEMANAGER_ERROR_DEPENDENCY_INVALID`
 *   is returned
 */
EvModuleManagerResult
ev_modulemanager_detect(
    const char *module_dir);

/*!
 * \brief Loads a module from the modules directory. Searches by name.
 *
 * \details
 * Due to modules internally being dynamic libraries, the operating system keeps
 * a refcount for the number of places where they're used. Thus, calling this
 * function doesn't guarantee that that the module will be loaded into memory
 * as it may already be there, making the function simply return the same module
 * and increase its refcount.
 *
 * \param modname Name of the module
 *
 * \returns
 *   - If the module was found and successfully loaded, then it is returned.
 *   - If the process fails, `NULL` is returned.
 */
evolmodule_t
ev_modulemanager_openmodule_w_name(
    const char *modname);

/*!
 * \brief Loads a module from the modules directory. Searches by category.
 *
 * \details
 * Due to modules internally being dynamic libraries, the operating system keeps
 * a refcount for the number of places where they're used. Thus, calling this
 * function doesn't guarantee that that the module will be loaded into memory
 * as it may already be there, making the function simply return the same module
 * and increase its refcount.
 *
 * \param modcategory Category wanted for the module
 *
 * \returns
 *   - If the module was found and successfully loaded, then it is returned.
 *   - If the process fails, `NULL` is returned.
 */
evolmodule_t
ev_modulemanager_openmodule_w_category(
    const char *modcategory);

/*!
 * \brief Loads a module from the modules directory. Searches by both category and name.
 *
 * \details
 * The `module_query` can either be a category or a name. If it is the name
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
 * \param module_query Identifier for the module.
 *
 * \returns
 *   - If the module was found and successfully loaded, then it is returned.
 *   - If the process fails, `NULL` is returned.
 */
evolmodule_t
ev_modulemanager_openmodule(
    const char *module_query);

EvModuleManagerResult
ev_modulemanager_init();

void
ev_modulemanager_deinit();