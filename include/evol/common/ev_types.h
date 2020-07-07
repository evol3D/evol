#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <evol/common/ev_macros.h>

#define RES_PREFIX EV_
#define RES_FILE <evol/meta/results/evolresults.h>
#define RES_TYPE EvResult
#include <evol/meta/resdef.h>

#include <evol/common/types/vec.h>
#include <evol/extern/sds.h>

typedef void *evolmodule_t;
