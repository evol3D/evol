#pragma once

#define RES_PREFIX EV_CONFIGLOADER_
#define RES_FILE <evol/meta/results/configloaderresults.h>
#define RES_TYPE EvConfigLoaderResult
#include <evol/meta/resdef.h>

EvConfigLoaderResult
ev_configloader_init();

EvConfigLoaderResult
ev_configloader_loadfile(
    CONST_STR filePath);

EVCOREAPI EvConfigLoaderResult
ev_configloader_get(
    CONST_STR varname, 
    ev_type var_type, 
    PTR res);

void
ev_configloader_deinit();
