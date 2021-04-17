/*!
 * \file lua_evutils.c
 */
#include <evol/utils/lua_evutils.h>
#include <evol/common/ev_log.h>

#include <stdarg.h>
#include <string.h>

#include <lautoc.h>
#include <lua.h>
#include <luajit.h>
#include <lualib.h>
#include <lauxlib.h>

// Helper for lua to be able to call luaA registered functions
I32 C(lua_State *L) { return luaA_call_name(L, lua_tostring(L, 1)); }

lua_State *
ev_lua_newState(
  bool stdlibs)
{
  lua_State *state = luaL_newstate();
  luaA_open(state);

  if(state && stdlibs) {
    luaL_openlibs(state);
    lua_register(state, "C", C);
  }

  return state;
}

EvLuaUtilsResult
ev_lua_destroyState(
  lua_State **state)
{
  luaA_close(*state);
  lua_close(*state);
  *state = NULL;
  return EV_LUAUTILS_SUCCESS;
}

EvLuaUtilsResult
ev_lua_loadfile(
  lua_State *state, 
  CONST_STR filePath)
{
  int result = luaL_loadfile(state, filePath);

  if (result) {
    ev_log_error(lua_tostring(state, -1));
    return EV_LUAUTILS_FILEACCESS_ERROR_FILELOADFAILED;
  }
  return EV_LUAUTILS_SUCCESS;
}

EvLuaUtilsResult
ev_lua_runloadedfile(
  lua_State *state)
{
  int result = lua_pcall(state, 0, 0, 0);

  if (result) {
    ev_log_error(lua_tostring(state, -1));
    return EV_LUAUTILS_EXECUTION_ERROR;
  }

  return EV_LUAUTILS_SUCCESS;
}

EvLuaUtilsResult
ev_lua_runfile(
  lua_State *state,
  CONST_STR filePath)
{
  EvLuaUtilsResult loadResult = ev_lua_loadfile(state, filePath);
  if (loadResult != EV_LUAUTILS_SUCCESS)
    return loadResult;

  return ev_lua_runloadedfile(state);
}

EvLuaUtilsResult
ev_lua_getsds(
  lua_State *state, 
  CONST_STR globalName, 
  SDS *result)
{
  EvLuaUtilsResult res;

  lua_getglobal(state, globalName);

  if (lua_isstring(state, -1)) {
    *result = sdsnew(lua_tostring(state, -1));
    res     = EV_LUAUTILS_SUCCESS;
  } else if (lua_isnil(state, -1)) {
    res = EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUAUTILS_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaUtilsResult
ev_lua_getstring(
  lua_State *state, 
  CONST_STR globalName, 
  STR *result)
{
  EvLuaUtilsResult res;

  lua_getglobal(state, globalName);

  if (lua_isstring(state, -1)) {
    *result = lua_tostring(state, -1);
    res     = EV_LUAUTILS_SUCCESS;
  } else if (lua_isnil(state, -1)) {
    res = EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUAUTILS_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaUtilsResult
ev_lua_getint(
  lua_State *state,
  CONST_STR globalName, 
  I64 *result)
{
  EvLuaUtilsResult res;

  lua_getglobal(state, globalName);

  if (lua_isnumber(state, -1)) {
    *result = lua_tointeger(state, -1);
    res     = EV_LUAUTILS_SUCCESS;
  } else if (lua_isnil(state, -1)) {
    res = EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUAUTILS_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaUtilsResult
ev_lua_getdouble(
  lua_State *state,
  CONST_STR globalName, 
  F64 *result)
{
  EvLuaUtilsResult res;

  lua_getglobal(state, globalName);

  if (lua_isnumber(state, -1)) {
    *result = lua_tonumber(state, -1);
    res     = EV_LUAUTILS_SUCCESS;
  } else if (lua_isnil(state, -1)) {
    res = EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUAUTILS_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaUtilsResult
ev_lua_getsdsvec(
  lua_State *state,
  CONST_STR globalName, 
  sdsvec_t *result)
{
  U32 vecsize = 0;
  EvLuaUtilsResult getlen_res = ev_lua_getlen(state, globalName, &vecsize);
  if(getlen_res != EV_LUAUTILS_SUCCESS || vecsize == 0) {
    return getlen_res;
  }

  lua_getglobal(state, globalName);
  for(U32 i = 1; i <= vecsize; ++i) {
    lua_pushnumber(state, i); // Push index
    lua_gettable(state, -2); // Replaces the index at the top of the stack with the result
    CONST_STR val = lua_tostring(state, -1); // This should be checked but there is currently no need for that.
    vec_push(result, &val);
    lua_pop(state, 1); // Pop result
  }
}

EvLuaUtilsResult
ev_lua_getlen(
  lua_State *state,
  CONST_STR globalName, 
  U32 *result)
{
  EvLuaUtilsResult res;
  lua_getglobal(state, globalName);
  if(!lua_istable(state, -1)) {
    res = EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND;
  } else {
    *result = lua_objlen(state, -1);
    res = EV_LUAUTILS_SUCCESS;
  }

  return res;
}

EvLuaUtilsResult
ev_lua_runstring(
  lua_State *state,
  CONST_STR luaString)
{
  I32 result = luaL_dostring(state, luaString);
  if (result) {
    ev_log_error("Lua: Error: %s", lua_tostring(state, -1));
    return EV_LUAUTILS_EXECUTION_ERROR;
  }

  return EV_LUAUTILS_SUCCESS;
}

void
ev_lua_callfn(
  lua_State *state,
  CONST_STR fn_name, 
  CONST_STR sig, 
  ...)
{
  va_list vl;

  int narg = 0;
  int nres = 0;

  va_start(vl, sig);
  lua_getglobal(state, fn_name);

  while (*sig) {
    switch (*sig) {
    case 'd':
      lua_pushnumber(state, va_arg(vl, double));
      break;
    case 'i':
      lua_pushnumber(state, va_arg(vl, int));
      break;
    case 's':
      lua_pushstring(state, va_arg(vl, char *));
      break;
    case 'b':
      lua_pushboolean(state, va_arg(vl, int));
      break;
    case '>':
      sig++;
      goto endwhile;
    default:
      luaL_error(state, "invalid option (%c)", *(sig - 1));
    }
    narg++;
    sig++;
    luaL_checkstack(state, 1, "too many arguments");
  }
endwhile:

  nres = strlen(sig);
  if (lua_pcall(state, narg, nres, 0) != 0) {
    luaL_error(state, "error running function `%s': %s", fn_name, lua_tostring(state, -1));
  }

  U32 pop_count = nres;
  nres = -nres;

  while (*sig) {
    if (lua_isnil(state, nres)) {
      *va_arg(vl, void **) = NULL;
      goto next_res;
    }

    switch (*sig) {
    case 'd':
      if (!lua_isnumber(state, nres)) {
        luaL_error(state, "Wrong result type");
      }
      *va_arg(vl, double *) = lua_tonumber(state, nres);
      break;
    case 'i':
      if (!lua_isnumber(state, nres)) {
        luaL_error(state, "Wrong result type");
      }
      *va_arg(vl, int *) = lua_tonumber(state, nres);
      break;
    case 's':
      if (!lua_isstring(state, nres)) {
        luaL_error(state, "Wrong result type");
      }
      *va_arg(vl, sds*) = sdsnew(lua_tostring(state, nres));
      break;
    case 'b':
      if (!lua_isboolean(state, nres)) {
        luaL_error(state, "Wrong result type");
      }
      *va_arg(vl, bool *) = lua_toboolean(state, nres);
      break;
    default:
      luaL_error(state, "invalid option (%c)", *(sig - 1));
    }
  next_res:
    nres++;
    sig++;
  }
  va_end(vl);
  lua_pop(state, pop_count);
}

EvLuaUtilsResult
ev_lua_getvar(
  lua_State *state,
  CONST_STR globalName,
  ev_type type,
  PTR result)
{
  EvLuaUtilsResult res;
  switch(type) {
    case EV_TYPE_I64:
      res = ev_lua_getint(state, globalName, result);
      break;
    case EV_TYPE_F64:
      res = ev_lua_getdouble(state, globalName, result);
      break;
    case EV_TYPE_STR:
    case EV_TYPE_CONST_STR:
      // WARNING, THIS SHOULD NEVER BE USED. THE LUA GARBAGE COLLECTOR
      // WILL SCREW YOU UP IF YOU'RE NOT CAREFUL. SDS IS A MUCH BETTER
      // CHOICE FOR MOST CASES
      ev_log_warn("Usage of one of (STR, CONST_STR) types detected in config. It is recommended to use SDS instead.");
      res = ev_lua_getstring(state, globalName, result);
      break;
    case EV_TYPE_SDS:
      res = ev_lua_getsds(state, globalName, result);
      break;
    default:
      ev_log_error("Unsupported Config type. Please pick a supported type (I64, F64, STR, CONST_STR, SDS)");
      break;
  }
  return res;
}
