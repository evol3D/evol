
#include "flecs_threads.h"

static ecs_os_thread_t ev_flecs_thread_new(ecs_os_thread_callback_t callback, void *arg)
{
  pthread_t thread;
  int r;
  if((r = pthread_create (&thread, NULL, callback, arg)))
  {
    abort();
  }
  return (ecs_os_thread_t)thread;
}

static void * ev_flecs_thread_join(ecs_os_thread_t thread)
{
  void *arg;
  pthread_join((pthread_t)thread, &arg);
  return arg;
}

static ecs_os_mutex_t ev_flecs_mutex_new(void)
{
  pthread_mutex_t *mutex = ecs_os_malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(mutex, NULL)) {
      abort();
  }
  return (ecs_os_mutex_t)(uintptr_t)mutex;
}

static void ev_flecs_mutex_free(ecs_os_mutex_t m)
{
  pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
  pthread_mutex_destroy(mutex);
  ecs_os_free(mutex);
}

static void ev_flecs_mutex_lock(ecs_os_mutex_t m)
{
  pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
  if (pthread_mutex_lock(mutex)) {
      abort();
  }
}

static void ev_flecs_mutex_unlock(ecs_os_mutex_t m)
{
  pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
  if (pthread_mutex_unlock(mutex)) {
      abort();
  }
}

static ecs_os_cond_t ev_flecs_cond_new(void)
{
  pthread_cond_t *cond = ecs_os_malloc(sizeof(pthread_cond_t));
  if (pthread_cond_init(cond, NULL)) {
      abort();
  }
  return (ecs_os_cond_t)(uintptr_t)cond;
}

static void ev_flecs_cond_free(ecs_os_cond_t c)
{
  pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
  if (pthread_cond_destroy(cond)) {
      abort();
  }
  ecs_os_free(cond);
}

static void ev_flecs_cond_signal(ecs_os_cond_t c)
{
  pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
  if (pthread_cond_signal(cond)) {
      abort();
  }
}

static void ev_flecs_cond_broadcast(ecs_os_cond_t c)
{
  pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
  if (pthread_cond_broadcast(cond)) {
      abort();
  }
}

static void ev_flecs_cond_wait(ecs_os_cond_t c, ecs_os_mutex_t m)
{
  pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
  pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
  if (pthread_cond_wait(cond, mutex)) {
      abort();
  }
}


void  ev_flecs_init_os_api(ecs_os_api_t *api)
{
  api->thread_new_ = ev_flecs_thread_new;
  api->thread_join_ = ev_flecs_thread_join;
  api->mutex_new_ = ev_flecs_mutex_new;
  api->mutex_free_ = ev_flecs_mutex_free;
  api->mutex_lock_ = ev_flecs_mutex_lock;
  api->mutex_unlock_ = ev_flecs_mutex_unlock;
  api->cond_new_ = ev_flecs_cond_new;
  api->cond_free_ = ev_flecs_cond_free;
  api->cond_signal_ = ev_flecs_cond_signal;
  api->cond_broadcast_ = ev_flecs_cond_broadcast;
  api->cond_wait_ = ev_flecs_cond_wait;
}

