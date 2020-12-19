#include <evol/core/lualoader.h>
#include <evol/common/ev_log.h>

#include <stdarg.h>
#include <string.h>

#include <lua.h>
#include <luajit.h>
#include <lualib.h>
#include <lauxlib.h>

struct ev_LuaLoaderData {
  lua_State *state;
  bool       isActive;
  uint32_t   fn_retstack;
} LuaLoaderData = {
  .state       = NULL,
  .isActive    = false,
  .fn_retstack = 0,
};

#define L LuaLoaderData.state

EvLuaLoaderResult
ev_lua_init()
{
  L = luaL_newstate();
  if (!L) {
    return EV_LUALOADER_INITIALIZATION_ERROR_STATECREATIONFAILED;
  }

  luaL_openlibs(L);
  LuaLoaderData.isActive = true;
  return EV_LUALOADER_SUCCESS;
}

EvLuaLoaderResult
ev_lua_deinit(void)
{
  lua_close(L);
  LuaLoaderData.isActive = false;
  return EV_LUALOADER_SUCCESS;
}

EvLuaLoaderResult
ev_lua_loadfile(const char *filePath)
{
  int result = luaL_loadfile(L, filePath);

  if (result) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    return EV_LUALOADER_FILEACCESS_ERROR_FILELOADFAILED;
  }
  return EV_LUALOADER_SUCCESS;
}

EvLuaLoaderResult
ev_lua_runloadedfile(void)
{
  int result = lua_pcall(L, 0, 0, 0);

  if (result) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    return EV_LUALOADER_EXECUTION_ERROR;
  }

  return EV_LUALOADER_SUCCESS;
}

EvLuaLoaderResult
ev_lua_runfile(const char *filePath)
{
  EvLuaLoaderResult loadResult = ev_lua_loadfile(filePath);
  if (loadResult != EV_LUALOADER_SUCCESS)
    return loadResult;

  return ev_lua_runloadedfile();
}

EvLuaLoaderResult
ev_lua_getstring(const char *globalName, const char **result)
{
  EvLuaLoaderResult res;

  lua_getglobal(L, globalName);

  if (lua_isstring(L, -1)) {
    *result = lua_tostring(L, -1);
    res     = EV_LUALOADER_SUCCESS;
  } else if (lua_isnil(L, -1)) {
    res = EV_LUALOADER_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUALOADER_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaLoaderResult
ev_lua_getint(const char *globalName, long *result)
{
  EvLuaLoaderResult res;

  lua_getglobal(L, globalName);

  if (lua_isnumber(L, -1)) {
    *result = lua_tointeger(L, -1);
    res     = EV_LUALOADER_SUCCESS;
  } else if (lua_isnil(L, -1)) {
    res = EV_LUALOADER_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUALOADER_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaLoaderResult
ev_lua_getdouble(const char *globalName, double *result)
{
  EvLuaLoaderResult res;

  lua_getglobal(L, globalName);

  if (lua_isnumber(L, -1)) {
    *result = lua_tonumber(L, -1);
    res     = EV_LUALOADER_SUCCESS;
  } else if (lua_isnil(L, -1)) {
    res = EV_LUALOADER_GLOBAL_ERROR_NOTFOUND;
  } else {
    res = EV_LUALOADER_GLOBAL_ERROR_WRONGTYPE;
  }

  return res;
}

EvLuaLoaderResult
ev_lua_runstring(const char *luaString)
{
  int result = luaL_dostring(L, luaString);
  if (result) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    return EV_LUALOADER_EXECUTION_ERROR;
  }

  return EV_LUALOADER_SUCCESS;
}

bool
ev_lua_isactive(void)
{
  return LuaLoaderData.isActive;
}

void
ev_lua_callfn(const char *fn_name, const char *sig, ...)
{
  va_list vl;

  int narg = 0;
  int nres = 0;

  va_start(vl, sig);
  lua_getglobal(L, fn_name);

  while (*sig) {
    switch (*sig) {
    case 'd':
      lua_pushnumber(L, va_arg(vl, double));
      break;
    case 'i':
      lua_pushnumber(L, va_arg(vl, int));
      break;
    case 's':
      lua_pushstring(L, va_arg(vl, char *));
      break;
    case 'b':
      lua_pushboolean(L, va_arg(vl, bool));
      break;
    case '>':
      sig++;
      goto endwhile;
    default:
      luaL_error(L, "invalid option (%c)", *(sig - 1));
    }
    narg++;
    sig++;
    luaL_checkstack(L, 1, "too many arguments");
  }
endwhile:

  nres = strlen(sig);
  if (lua_pcall(L, narg, nres, 0) != 0) {
    luaL_error(L, "error running function `%s': %s", fn_name, lua_tostring(L, -1));
  }

  LuaLoaderData.fn_retstack += nres;
  nres = -nres;

  while (*sig) {
    if (lua_isnil(L, nres)) {
      *va_arg(vl, void **) = NULL;
      goto next_res;
    }
    switch (*sig) {
    case 'd':
      if (!lua_isnumber(L, nres)) {
        luaL_error(L, "Wrong result type");
      }
      *va_arg(vl, double *) = lua_tonumber(L, nres);
      break;
    case 'i':
      if (!lua_isnumber(L, nres)) {
        luaL_error(L, "Wrong result type");
      }
      *va_arg(vl, int *) = lua_tonumber(L, nres);
      break;
    case 's':
      if (!lua_isstring(L, nres)) {
        luaL_error(L, "Wrong result type");
      }
      *va_arg(vl, const char **) = lua_tostring(L, nres);
      break;
    case 'b':
      if(!lua_isboolean(L, nres)) {
        luaL_error(L, "Wrong result type");
      }
      *va_arg(vl, bool *) = lua_toboolean(L, nres);
      break;
    default:
      luaL_error(L, "invalid option (%c)", *(sig - 1));
    }
  next_res:
    nres++;
    sig++;
  }
  va_end(vl);
}

void
ev_lua_fnstack_pop()
{
  lua_pop(L, LuaLoaderData.fn_retstack);
  LuaLoaderData.fn_retstack = 0;
}
