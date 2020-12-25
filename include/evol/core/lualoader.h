/*!
 * \file lualoader.h
 */
#pragma once

#include <evol/common/ev_types.h>

#define RES_PREFIX EV_LUALOADER_
#define RES_FILE <evol/meta/results/lualoaderresults.h>
#define RES_TYPE EvLuaLoaderResult
#include <evol/meta/resdef.h>

/*
 * \brief Returns whether the lualoader is initialized or not
 *
 * \returns
 * - `true` if initialized
 * - `false` otherwise
 */
bool
ev_lua_isactive(void);

/*
 * \brief Initializes the lualoader
 *
 * \details
 * Creates a new lua state and loads the base lua libraries in it.
 *
 * \returns
 * If the lua state creation was successful, returns `EV_LUALOADER_SUCCESS`,
 * otherwise returns `EV_LUALOADER_INITIALIZATION_ERROR_STATECREATIONFAILED`
 */
EvLuaLoaderResult
ev_lua_init(void);

EvLuaLoaderResult
ev_lua_deinit(void);

EvLuaLoaderResult
ev_lua_loadfile(const char *filePath);

EvLuaLoaderResult
ev_lua_runloadedfile(void);

EvLuaLoaderResult
ev_lua_runfile(const char *filePath);

EvLuaLoaderResult
ev_lua_runstring(const char *luaString);

EvLuaLoaderResult
ev_lua_getstring(const char *globalName, const char **result);

EvLuaLoaderResult
ev_lua_getint(const char *globalName, long *result);

EvLuaLoaderResult
ev_lua_getdouble(const char *globalName, double *result);

EvLuaLoaderResult
ev_lua_getlen(const char *globalName, uint32_t *result);

EvLuaLoaderResult
ev_lua_getsdsvec(const char *globalName, sdsvec_t *result);

void
ev_lua_callfn(const char *fn_name, const char *sig, ...);

void
ev_lua_fnstack_pop();

#define ev_lua_getvar(varname, var)                                            \
  _Generic((var),                                                              \
      double  : ev_lua_getdouble,                                              \
      int32_t : ev_lua_getint,                                                 \
      sdsvec_t: ev_lua_getsdsvec,                                              \
      default : ev_lua_getstring)                                              \
                                                                (varname, &var)

#define ev_lua_getvar_s(varname, var)                                          \
  if (ev_lua_getvar(varname, var) != EV_LUALOADER_SUCCESS) {                   \
    var = NULL;                                                                \
  }
