#ifndef EVOL_FLECS_THREADS_HEADER
#define EVOL_FLECS_THREADS_HEADER

#include <evolpthreads.h>
#include <flecs.h>

static ecs_os_thread_t ev_flecs_thread_new(ecs_os_thread_callback_t callback, void *arg);
static void * ev_flecs_thread_join(ecs_os_thread_t thread);
static ecs_os_mutex_t ev_flecs_mutex_new(void);
static void ev_flecs_mutex_free(ecs_os_mutex_t m);
static void ev_flecs_mutex_lock(ecs_os_mutex_t m);
static void ev_flecs_mutex_unlock(ecs_os_mutex_t m);
static ecs_os_cond_t ev_flecs_cond_new(void);
static void ev_flecs_cond_free(ecs_os_cond_t c);
static void ev_flecs_cond_signal(ecs_os_cond_t c);
static void ev_flecs_cond_broadcast(ecs_os_cond_t c);
static void ev_flecs_cond_wait(ecs_os_cond_t c, ecs_os_mutex_t m);

void  ev_flecs_init_os_api(ecs_os_api_t *api);


#endif