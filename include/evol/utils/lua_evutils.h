/*!
 * \file lua_evutils.h
 */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <evol/common/ev_types.h>
#include <luajit.h>
#include <lautoc.h>

#define RES_PREFIX EV_LUAUTILS_
#define RES_FILE <evol/meta/results/luautilsresults.h>
#define RES_TYPE EvLuaUtilsResult
#include <evol/meta/resdef.h>

EVCOREAPI lua_State *
ev_lua_newState(
  bool stdlibs);

EVCOREAPI EvLuaUtilsResult
ev_lua_destroyState(
  lua_State **state);

EVCOREAPI EvLuaUtilsResult
ev_lua_loadfile(
  lua_State *state, 
  CONST_STR filePath);

EVCOREAPI EvLuaUtilsResult
ev_lua_runloadedfile(
  lua_State *state);

EVCOREAPI EvLuaUtilsResult
ev_lua_runfile(
  lua_State *state,
  CONST_STR filePath);

EVCOREAPI EvLuaUtilsResult
ev_lua_getstring(
  lua_State *state, 
  const char *globalName, 
  evstring *result);

EVCOREAPI EvLuaUtilsResult
ev_lua_getint(
  lua_State *state,
  CONST_STR globalName, 
  I64 *result);

EVCOREAPI EvLuaUtilsResult
ev_lua_getdouble(
  lua_State *state,
  CONST_STR globalName, 
  F64 *result);

EVCOREAPI EvLuaUtilsResult
ev_lua_getvar(
  lua_State *state,
  CONST_STR globalName,
  ev_type type,
  PTR result);

EVCOREAPI EvLuaUtilsResult
ev_lua_getstring_vec(
  lua_State *state,
  CONST_STR globalName, 
  vec(evstring) *result);

EVCOREAPI EvLuaUtilsResult
ev_lua_getlen(
  lua_State *state,
  CONST_STR globalName, 
  U32 *result);

EVCOREAPI EvLuaUtilsResult
ev_lua_runstring(
  lua_State *state,
  CONST_STR luaString);

EVCOREAPI void
ev_lua_callfn(
  lua_State *state,
  CONST_STR fn_name, 
  CONST_STR sig, 
  ...);

EVCOREAPI int
ev_lua_pcall(
    lua_State *L,
    int nargs,
    int nresults,
    int errfunc);

#if defined(__cplusplus)
}
#endif
