#pragma once

#include <stddef.h>
#include <evol/common/ev_macros.h>

EVCOREAPI void *
aligned_malloc(
    size_t bytes, 
    size_t alignment);

EVCOREAPI void *
aligned_free(
    void *alig_ptr);
