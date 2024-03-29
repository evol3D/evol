/*!  \file ev_types.h */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define RES_PREFIX EV_ /*!< The prefix of all error types defined. */
#define RES_FILE <evol/meta/results/evolresults.h> /*!< The file in which the result types are defined. */
#define RES_TYPE                                                               \
  EvResult /*!< The name of the result enum that this file defines. */
#include <evol/meta/resdef.h>

#include <vec.h>
#include <evol/common/types/dvec.h>

#include <hashmap.h>

#include <evstr.h>

// #if defined(MATH_SUPPORT)
//TODO if use_math build option enabled

#include <cglm/cglm.h>
union EV_ALIGN(16) _ev_impl_vec3 {
  struct { float x, y, z; };
  struct { float r, g, b; };
};

union EV_ALIGN(16) _ev_impl_vec4 {
  struct { float x, y, z, w; };
  struct { float r, g, b, a; };
};

typedef union _ev_impl_vec3 Vec3;
#define Vec3new(nx, ny, nz)     (Vec3){.x = nx, .y = ny, .z = nz}
typedef union _ev_impl_vec4 Vec4;
#define Vec4new(nx, ny, nz, nw) (Vec4){.x = nx, .y = ny, .z = nz, .w = nw}

typedef float EV_ALIGN(16) Matrix4x4[4][4];

#define PRINT_VEC3(v) \
  printf("Vec3 %s = (%f, %f, %f)\n", EV_STRINGIZE(v), v.x, v.y, v.z)

#define PRINT_VEC4(v) \
  printf("Vec4 %s = (%f, %f, %f, %f)\n", EV_STRINGIZE(v), v.x, v.y, v.z, v.w)


// #endif


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

#if defined(EV_PRECISION_HIGH)
typedef F64 REAL;
#else
typedef F32 REAL;
#endif

typedef char *  STR;
typedef const char *  CONST_STR;

typedef evstring STRING;

typedef void(*FN_PTR)();
typedef void* PTR;

typedef PTR evolmodule_t;

typedef U64 GenericHandle;
#define INVALID_GENERIC_HANDLE (~0ull)

HashFunctionDefine(U8)
HashFunctionDefine(U16)
HashFunctionDefine(U32)
HashFunctionDefine(U64)

HashFunctionDefine(I8)
HashFunctionDefine(I16)
HashFunctionDefine(I32)
HashFunctionDefine(I64)

HashFunctionDefine(F32)
HashFunctionDefine(F64)

HashFunctionDefine(PTR)
HashFunctionDefine(FN_PTR)

HashFunctionDefine(GenericHandle)

HashFunctionDefineCustom(evstring, pVar)
{
    return hashmap_murmur(*pVar, strlen(*pVar), seed0, seed1);           
}

CmpFunctionDefine(U8)
CmpFunctionDefine(U16)
CmpFunctionDefine(U32)
CmpFunctionDefine(U64)

CmpFunctionDefine(I8)
CmpFunctionDefine(I16)
CmpFunctionDefine(I32)
CmpFunctionDefine(I64)

CmpFunctionDefine(F32)
CmpFunctionDefine(F64)

CmpFunctionDefine(PTR)
CmpFunctionDefine(FN_PTR)

CmpFunctionDefine(GenericHandle)

CmpFunctionDefineCustom(evstring, pVar0, pVar1)
{
  (void)udata;
  return strcmp(*pVar0, *pVar1);
}

#ifndef __cplusplus
HashmapDefine(evstring, GenericHandle, evstring_free, NULL);
#endif

#define EV_TYPE_NEW_DEFAULT(val) val
static inline void EV_TYPE_FREE_DEFAULT(void *_) {(void)_;}

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

  EV_TYPE_STRING,
#define EV_TYPE_NEW_STRING(val) evstring_new(val)
#define EV_TYPE_FREE_STRING evstring_free

  EV_TYPE_GenericHandle,
#define EV_TYPE_NEW_GenericHandle EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_GenericHandle EV_TYPE_FREE_DEFAULT
} ev_type;

/*!
 * \brief elem_copy implementation for the `evstring` type
 */
static inline void
evstring_veccopy(void *dst, const void *src)
{
  *(evstring*)dst = evstring_new(*(const char **)src);
}

/*!
 * \brief elem_destr implementation for the `evstring` type
 */
static inline void
evstring_vecdestr(void *elem)
{
  evstring_free(*(evstring*)elem);
}

#if defined(__cplusplus)
}
#endif
