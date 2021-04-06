#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifndef DEFAULT_TPOOL_THREAD_COUNT
# define DEFAULT_TPOOL_THREAD_COUNT (get_num_procs() + 2)
#endif

struct ev_tpool;
typedef struct ev_tpool ev_tpool_t;

typedef void (*thread_func_t)(void *arg);

EVCOREAPI ev_tpool_t *
ev_tpool_create(
    size_t num);

EVCOREAPI void 
ev_tpool_destroy(
    ev_tpool_t *tm);

EVCOREAPI bool 
ev_tpool_add_work(
    ev_tpool_t *tm, 
    thread_func_t func, 
    void *arg);

EVCOREAPI void 
ev_tpool_wait(
    ev_tpool_t *tm);