#pragma once

#include <stddef.h>

void *
aligned_malloc(
    size_t bytes, 
    size_t alignment);

void *
aligned_free(
    void *alig_ptr);
