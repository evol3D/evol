#include <evol/core/configloader.h>

#include <evol/utils/lua_evutils.h>

struct ev_ConfigLoaderData {
    lua_State *state;
} ConfigLoaderData = {
    .state = NULL,
};

EvConfigLoaderResult
ev_configloader_init()
{
    ConfigLoaderData.state = ev_lua_newState(true);
    assert(ConfigLoaderData.state);
    return EV_CONFIGLOADER_SUCCESS;
}

EvConfigLoaderResult
ev_configloader_loadfile(
    CONST_STR filePath)
{
    EvLuaUtilsResult luaload_result = ev_lua_runfile(ConfigLoaderData.state, filePath);
    if(luaload_result != EV_LUAUTILS_SUCCESS) {
        ev_log_error("LuaLoader error: %s", RES_STRING(EvLuaUtilsResult, luaload_result));
        return EV_CONFIGLOADER_FILEACCESS_ERROR_FILELOADFAILED;
    }
    return EV_CONFIGLOADER_SUCCESS;
}

EvConfigLoaderResult
ev_configloader_get(
    CONST_STR varname, 
    ev_type var_type, 
    PTR res)
{
    EvConfigLoaderResult ret;
    switch(ev_lua_getvar(ConfigLoaderData.state, varname, var_type, res)) {
    case EV_LUAUTILS_SUCCESS:
        ret = EV_CONFIGLOADER_SUCCESS;
        break;
    case EV_LUAUTILS_GLOBAL_ERROR_NOTFOUND:
        ret = EV_CONFIGLOADER_GET_ERROR_NOTFOUND;
        break;
    case EV_LUAUTILS_GLOBAL_ERROR_WRONGTYPE:
        ret = EV_CONFIGLOADER_GET_ERROR_WRONGTYPE;
        break;
    default:
        ret = -1;
    }
    return ret;
}

void
ev_configloader_deinit()
{
    ev_lua_destroyState(&ConfigLoaderData.state);
}
