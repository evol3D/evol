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

#ifdef EV_PRECISION_HIGH
typedef F64 REAL;
#else
typedef F32 REAL;
#endif

typedef char *  STR;
typedef const char *  CONST_STR;

typedef sds SDS;

typedef void(*FN_PTR)();
typedef void* PTR;

typedef PTR evolmodule_t;

#define EV_TYPE_NEW_DEFAULT(val) val
#define EV_TYPE_FREE_DEFAULT NULL

typedef enum {
  EV_TYPE_I8,
#define EV_TYPE_NEW_I8 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_I8 EV_TYPE_FREE_DEFAULT
  EV_TYPE_I16,
#define EV_TYPE_NEW_I16 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_I16 EV_TYPE_FREE_DEFAULT
  EV_TYPE_I32,
#define EV_TYPE_NEW_I32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_I32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_I64,
#define EV_TYPE_NEW_I64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_I64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_U8,
#define EV_TYPE_NEW_U8 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_U8 EV_TYPE_FREE_DEFAULT
  EV_TYPE_U16,
#define EV_TYPE_NEW_U16 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_U16 EV_TYPE_FREE_DEFAULT
  EV_TYPE_U32,
#define EV_TYPE_NEW_U32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_U32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_U64,
#define EV_TYPE_NEW_U64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_U64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_F32,
#define EV_TYPE_NEW_F32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_F32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_F64,
#define EV_TYPE_NEW_F64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_F64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_PTR,
#define EV_TYPE_NEW_PTR EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_PTR EV_TYPE_FREE_DEFAULT
  EV_TYPE_FN_PTR,
#define EV_TYPE_NEW_FN_PTR EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_FN_PTR EV_TYPE_FREE_DEFAULT

  EV_TYPE_STR,
#define EV_TYPE_NEW_STR EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_STR EV_TYPE_FREE_DEFAULT
  EV_TYPE_CONST_STR,
#define EV_TYPE_NEW_CONST_STR EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_CONST_STR EV_TYPE_FREE_DEFAULT
  EV_TYPE_SDS,
#define EV_TYPE_NEW_SDS sdsnew
#define EV_TYPE_FREE_SDS sdsfree
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
