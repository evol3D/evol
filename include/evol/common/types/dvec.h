/*!
 * \file dvec.h
 */
#pragma once

#include "vec.h"

typedef struct dvec *dvec_t;

#define dvec(type) dvec_t

dvec_t
dvec_init_impl(
    size_t elemsize, 
    elem_copy copy, 
    elem_destr destr);

#define dvec_init(type, copy, destr) dvec_init_impl(sizeof(type), copy, destr)

vec_t
dvec_read(
    dvec_t dv);

vec_t
dvec_write(
    dvec_t dv);

int32_t
dvec_push(
    dvec_t dv, 
    void *val);

void
dvec_swap(
    dvec_t dv);

void
dvec_fini(
    dvec_t v);
