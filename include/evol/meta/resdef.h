/*!
 * \file resdef.h
 */

#include <evol/common/ev_macros.h>

#ifndef RES_PREFIX
#define RES_PREFIX
#endif

#ifndef RES_TYPE
#error("RES_TYPE not specified")
#endif

#ifndef RES_FILE
#error("RES_FILE not specified")
#endif

typedef enum EV_CONCAT(RES_TYPE, Codes) {
#define RESULT(x) EV_CONCAT(RES_PREFIX, x),
#define RESULT_VAL(x, n) EV_CONCAT(RES_PREFIX, x) = n,
#include RES_FILE
#undef RESULT
#undef RESULT_VAL
} RES_TYPE;

EV_UNUSED static const char *EV_CONCAT(RES_TYPE, Strings)[] = {
#define RESULT(x) EV_STRINGIZE(EV_CONCAT(RES_PREFIX, x)),
#ifdef __cplusplus // No designated initializers in cpp yet
#define RESULT_VAL(x, n) EV_STRINGIZE(EV_CONCAT(RES_PREFIX, x)),
#else
#define RESULT_VAL(x, n) [n] = EV_STRINGIZE(EV_CONCAT(RES_PREFIX, x)),
#endif
#include RES_FILE
#undef RESULT
#undef RESULT_VAL
};

#undef RES_PREFIX
#undef RES_TYPE
#undef RES_FILE

#ifndef RES_STRING
#define RES_STRING(type, val) EV_CONCAT(type, Strings)[val]
#endif
