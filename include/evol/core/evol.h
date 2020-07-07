#pragma once

#include <evol/common/ev_types.h>

typedef struct evolengine evolengine_t;

#define RES_PREFIX EV_ENGINE_
#define RES_FILE <evol/meta/results/engineresults.h>
#define RES_TYPE EvEngineResult
#include <evol/meta/resdef.h>

evolengine_t *
evol_create();

void
evol_destroy(evolengine_t *engine);

EvEngineResult
evol_init(evolengine_t *engine);

EvEngineResult
evol_parse_args(evolengine_t *engine, int argc, char **argv);

void
evol_deinit(evolengine_t *engine);

evolmodule_t
evol_loadmodule(const char *modquery);

void
evol_unloadmodule(evolmodule_t module);

void *
evol_getmodfunc(evolmodule_t module, const char *func_name);
