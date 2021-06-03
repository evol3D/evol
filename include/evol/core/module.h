/*! \file module.h */
#pragma once
#include <evol/common/ev_types.h>

typedef PTR evolmodule_t;

/*!
 * \brief Loads a module into memory
 *
 * Due to modules internally being dynamic libraries, the operating system keeps
 * a refcount for the number of places where they're used. Thus, calling this
 * function doesn't guarantee that that the module will be loaded into memory
 * as it may already be there, making the function simply return the same module
 * and increase its refcount.
 *
 * \param modpath Path to the module
 *
 * \returns
 *   - If the module was found and successfully loaded, then it is returned.
 *   - If the process fails, `NULL` is returned.
 */
evolmodule_t
ev_module_open(
    const char *modpath);

/*!
 * \brief Get handle to an already loaded module
 *
 * Mostly the same as `ev_module_open` but without the refcount
 *
 * \param modpath Path to the module
 *
 * \returns
 *   - If the module was found in memory, then its handle is returned.
 *   - If the process fails, `NULL` is returned.
 */
evolmodule_t
ev_module_gethandle(
    const char *modpath);

/*!
 * \brief Unloads a module.
 *
 * \details Due to modules internally being dynamic libraries, the operating
 * system keeps a refcount for the number of places where they're used. So there
 * are no guarantees that calling this function will call the module's destructor
 * as it may be in use in other areas of the program.
 *
 * \remark It's highly recommended to call `ev_module_open(...)` and
 * `ev_module_close(...)` in pairs and to never share the returned module
 * handles with external modules so that maintaining a defined behaviour is
 * easier.
 *
 * \param module The module to unload
 */
void
ev_module_close(
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
FN_PTR
ev_module_getfn(
    evolmodule_t module, 
    const char *fn_name);

/*!
 * \brief Retrieve global variable from module
 *
 * \param module The module to search for the variable
 * \param var_name Name of the variable to load
 *
 * \returns
 * If the variable was found, its pointer is returned. Otherwise, `NULL` is
 * returned.
 */
PTR
ev_module_getvar(
    evolmodule_t module, 
    const char *var_name);