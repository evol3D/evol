/*!
 * \file ev_types.h
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define RES_PREFIX EV_ /*!< The prefix of all error types defined. */
#define RES_FILE <evol/meta/results/evolresults.h> /*!< The file in which the result types are defined. */
#define RES_TYPE EvResult /*!< The name of the result enum that this file defines. */
#include <evol/meta/resdef.h>

#include <evol/common/types/vec.h>
#include <evol/extern/sds.h>

typedef void *evolmodule_t;
