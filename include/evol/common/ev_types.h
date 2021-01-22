/*!  \file ev_types.h */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define RES_PREFIX EV_ /*!< The prefix of all error types defined. */
#define RES_FILE <evol/meta/results/evolresults.h> /*!< The file in which the result types are defined. */
#define RES_TYPE                                                               \
  EvResult /*!< The name of the result enum that this file defines. */
#include <evol/meta/resdef.h>

#include <evol/common/types/vec.h>
#include <evol/extern/sds.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t  I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef float   F32;
typedef double  F64;

typedef char *  STR;
typedef const char *  CONST_STR;

typedef void(*FN_PTR)();
typedef void* PTR;

typedef PTR evolmodule_t;

typedef enum {
  EV_TYPE_I8,
  EV_TYPE_I16,
  EV_TYPE_I32,
  EV_TYPE_I64,

  EV_TYPE_U8,
  EV_TYPE_U16,
  EV_TYPE_U32,
  EV_TYPE_U64,

  EV_TYPE_F32,
  EV_TYPE_F64,

  EV_TYPE_PTR,
  EV_TYPE_FNPTR,

  EV_TYPE_STR,
} ev_type;

/*!
 * \brief A specialized vector for sds (dynamic strings)
 */
typedef vec_t sdsvec_t;

/*!
 * \brief elem_copy implementation for the `sds` type
 */
static inline void
sdsveccopy(void *dst, const void *src)
{
  *(sds*)dst = sdsnew(*(sds*)src);
}

/*!
 * \brief elem_destr implementation for the `sds` type
 */
static inline void
sdsvecdestr(void *elem)
{
  sdsfree(*(sds*)elem);
}

/*!
 * \brief `vec_init` equivalent that takes no arguments and initializes the
 * vector with the default values for sds.
 */
#define sdsvec_init() vec_init_impl(sizeof(sds), sdsveccopy, sdsvecdestr)
