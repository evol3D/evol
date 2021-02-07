/*!
 * \file strings.h
 */
#pragma once
#include "evol/common/ev_types.h"
#include "evol/core/evol.h"
#include <evol/common/ev_macros.h>
#include <evol/meta/strings.h>

#define DEFAULT_ENTRY(a_key, a_type, a_val) \
    evstore_set(GLOBAL_STORE, &(evstore_entry_t){ \
        .key  = a_key, \
        .type = a_type, \
        .data = a_val, \
    })

DEFAULT_ENTRY(EV_CORE_NAME                  , EV_TYPE_STR, "Hello, evol");

DEFAULT_ENTRY(EV_CORE_VERSION_MAJOR         , EV_TYPE_U8 , 0            );
DEFAULT_ENTRY(EV_CORE_VERSION_MINOR         , EV_TYPE_U8 , 1            );

DEFAULT_ENTRY(EV_CORE_MODULEDIR             , EV_TYPE_STR, "."  );

DEFAULT_ENTRY(EV_CORE_EVENTSYSTEM_UPDATERATE, EV_TYPE_I64, 500 );
