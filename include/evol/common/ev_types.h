/*!  \file ev_types.h */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define RES_PREFIX EV_ /*!< The prefix of all error types defined. */
#define RES_FILE                                                               \
  <evol / meta / results / evolresults.h> /*!< The file in which the result types are defined. */
#define RES_TYPE                                                               \
  EvResult /*!< The name of the result enum that this file defines. */
#include <evol/meta/resdef.h>

#include <evol/common/types/vec.h>
#include <evol/extern/sds.h>

typedef void *evolmodule_t;

/*!
 * \brief A specialized vector for sds (dynamic strings)
 */
typedef vec_t sdsvec_t;

/*!
 * \brief `vec_init` equivalent that takes no arguments and initializes the
 * vector with the default values for sds.
 */
#define sdsvec_init() vec_init(sds, sdsveccopy, sdsvecdestr)

/*!
 * \brief elem_copy implementation for the `sds` type
 */
inline void
sdsveccopy(void *dst, const void *src)
{
  *(sds*)dst = sdsnew(*(sds*)src);
}

/*!
 * \brief elem_destr implementation for the `sds` type
 */
inline void
sdsvecdestr(void *elem)
{
  sdsfree(*(sds*)elem);
}
