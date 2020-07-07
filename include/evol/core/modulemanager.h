#pragma once

#include <evol/common/ev_types.h>

#include <evol/core/module.h>

#define RES_PREFIX EV_MODULEMANAGER_
#define RES_FILE <evol/meta/results/modulemanagerresults.h>
#define RES_TYPE EvModuleManagerResult
#include <evol/meta/resdef.h>

EvModuleManagerResult
ev_modulemanager_detect(const char *module_dir);

evolmodule_t
ev_modulemanager_openmodule_w_name(const char *modname);

evolmodule_t
ev_modulemanager_openmodule_w_category(const char *modcategory);

evolmodule_t
ev_modulemanager_openmodule(const char *module_query);
