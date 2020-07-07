#pragma once

#include <stdlib.h>

#ifndef VEC_INIT_CAP
#define VEC_INIT_CAP 8
#endif

#ifndef VEC_GROWTH_RATE
#define VEC_GROWTH_RATE 3 / 2
#endif

typedef void *vec_t;

typedef void (*elem_copy)(void *, const void *);
typedef void (*elem_destr)(void *);

vec_t
vec_init_impl(size_t elemsize, elem_copy copy, elem_destr destr);
#define vec_init(type, copy, destr) vec_init_impl(sizeof(type), copy, destr)

void *
vec_iter_begin(vec_t v);
void *
vec_iter_end(vec_t v);
void
vec_iter_next(vec_t v, void **iter);

void
vec_fini(vec_t v);

int32_t
vec_push(vec_t *v, void *val);
