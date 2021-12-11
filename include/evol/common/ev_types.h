/*!  \file ev_types.h */
#pragma once
#include <evol/common/ev_macros.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* #include <stdint.h> */
/* #include <stdbool.h> */

// #define RES_PREFIX EV_ /*!< The prefix of all error types defined. */
// #define RES_FILE <evol/meta/results/evolresults.h> /*!< The file in which the result types are defined. */
// #define RES_TYPE EvResult /*!< The name of the result enum that this file defines. */
// #include <evol/meta/resdef.h>

/* #include <vec.h> */
/* #include <evol/common/types/dvec.h> */

/* #include <hashmap.h> */

/* #include <evstr.h> */

// #if defined(MATH_SUPPORT)
//TODO if use_math build option enabled

/* #include <cglm/cglm.h> */
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


typedef __uint8_t  u8;
typedef __uint16_t u16;
typedef __uint32_t u32;
typedef __uint64_t u64;

typedef __int8_t  i8;
typedef __int16_t i16;
typedef __int32_t i32;
typedef __int64_t i64;

typedef float   f32;
typedef double  f64;

#if defined(EV_PRECISION_HIGH)
typedef f64 real;
#else
typedef f32 real;
#endif

typedef void(*fnptr)();


typedef u64 GenericHandle;
#define INVALID_GENERIC_HANDLE (~0ull)

/* HashFunctionDefine(u8) */
/* HashFunctionDefine(u16) */
/* HashFunctionDefine(u32) */
/* HashFunctionDefine(u64) */

/* HashFunctionDefine(i8) */
/* HashFunctionDefine(i16) */
/* HashFunctionDefine(i32) */
/* HashFunctionDefine(i64) */

/* HashFunctionDefine(f32) */
/* HashFunctionDefine(f64) */

/* HashFunctionDefine(fnptr) */

/* HashFunctionDefine(GenericHandle) */

/* HashFunctionDefineCustom(evstring, pVar) */
/* { */
/*     return hashmap_murmur(*pVar, evstring_len(*pVar), seed0, seed1); */           
/* } */

/* CmpFunctionDefine(u8) */
/* CmpFunctionDefine(u16) */
/* CmpFunctionDefine(u32) */
/* CmpFunctionDefine(u64) */

/* CmpFunctionDefine(i8) */
/* CmpFunctionDefine(i16) */
/* CmpFunctionDefine(i32) */
/* CmpFunctionDefine(i64) */

/* CmpFunctionDefine(f32) */
/* CmpFunctionDefine(f64) */

/* CmpFunctionDefine(fnptr) */

/* CmpFunctionDefine(GenericHandle) */

/* CmpFunctionDefineCustom(evstring, pVar0, pVar1) */
/* { */
/*   (void)udata; */
/*   return strcmp(*pVar0, *pVar1); */
/* } */

/* #ifndef __cplusplus */
/* HashmapDefine(evstring, GenericHandle, evstring_free, NULL); */
/* #endif */

#define EV_TYPE_NEW_DEFAULT(val) val
static inline void EV_TYPE_FREE_DEFAULT(void *_) {(void)_;}

typedef enum {
  EV_TYPE_i8,
#define EV_TYPE_NEW_i8 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_i8 EV_TYPE_FREE_DEFAULT
  EV_TYPE_i16,
#define EV_TYPE_NEW_i16 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_i16 EV_TYPE_FREE_DEFAULT
  EV_TYPE_i32,
#define EV_TYPE_NEW_i32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_i32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_i64,
#define EV_TYPE_NEW_i64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_i64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_u8,
#define EV_TYPE_NEW_u8 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_u8 EV_TYPE_FREE_DEFAULT
  EV_TYPE_u16,
#define EV_TYPE_NEW_u16 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_u16 EV_TYPE_FREE_DEFAULT
  EV_TYPE_u32,
#define EV_TYPE_NEW_u32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_u32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_u64,
#define EV_TYPE_NEW_u64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_u64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_f32,
#define EV_TYPE_NEW_f32 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_f32 EV_TYPE_FREE_DEFAULT
  EV_TYPE_f64,
#define EV_TYPE_NEW_f64 EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_f64 EV_TYPE_FREE_DEFAULT

  EV_TYPE_fnptr,
#define EV_TYPE_NEW_fnptr EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_fnptr EV_TYPE_FREE_DEFAULT

  EV_TYPE_string,
#define EV_TYPE_NEW_string(val) evstring_new(val)
#define EV_TYPE_FREE_string evstring_free

  EV_TYPE_GenericHandle,
#define EV_TYPE_NEW_GenericHandle EV_TYPE_NEW_DEFAULT
#define EV_TYPE_FREE_GenericHandle EV_TYPE_FREE_DEFAULT
} ev_type;

/*!
 * \brief elem_copy implementation for the `evstring` type
 */
/* static inline void */
/* evstring_veccopy(void *dst, const void *src) */
/* { */
/*   *(evstring*)dst = evstring_new(*(const char **)src); */
/* } */

/*!
 * \brief elem_destr implementation for the `evstring` type
 */
/* static inline void */
/* evstring_vecdestr(void *elem) */
/* { */
/*   evstring_free(*(evstring*)elem); */
/* } */

#if defined(__cplusplus)
}
#endif
