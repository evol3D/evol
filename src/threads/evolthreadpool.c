#include <evol/common/ev_macros.h>
#include <evol/threads/evolpthreads.h>
#include <evol/threads/evolthreadpool.h>
#include <stdlib.h>

struct ev_tpool_work {
  thread_func_t func;
  void *arg;
  struct ev_tpool_work *next;
};

typedef struct ev_tpool_work ev_tpool_work_t;

struct ev_tpool {
  ev_tpool_work_t *work_first;
  ev_tpool_work_t *work_last;
  pthread_mutex_t work_mutex;
  pthread_cond_t work_cond;
  pthread_cond_t working_cond;
  size_t working_cnt;
  size_t thread_cnt;
  bool stop;
};

EVCOREAPI ev_tpool_work_t *
ev_tpool_work_create(
  thread_func_t func, 
  void *arg)
{
  ev_tpool_work_t *work;

  if(func == NULL)
    return NULL;

  work = malloc(sizeof(ev_tpool_work_t));
  work->func = func;
  work->arg = arg;
  work->next = NULL;

  return work;
}

EVCOREAPI void 
ev_tpool_work_destroy(
  ev_tpool_work_t *work)
{
  if(work == NULL)
    return;
  free(work);
}

EVCOREAPI ev_tpool_work_t *
ev_tpool_work_get(ev_tpool_t *tm)
{
  ev_tpool_work_t *work;
  if(tm == NULL)
    return NULL;

  work = tm->work_first;

  if(work == NULL)
    return NULL;

  if(work->next == NULL) {
    tm->work_first = NULL;
    tm->work_last = NULL;
  } else {
    tm->work_first = work->next;
  }

  return work;
}

EVCOREAPI void *
ev_tpool_worker(
  void *arg)
{
  ev_tpool_t *tm = arg;
  ev_tpool_work_t *work;

  while (1) {
    pthread_mutex_lock(&(tm->work_mutex));
    while(tm->work_first == NULL && !tm->stop)
      pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));

    if(tm->stop)
      break;

    work = ev_tpool_work_get(tm);
    tm->working_cnt++;
    pthread_mutex_unlock(&(tm->work_mutex));

    if(work != NULL) {
      work->func(work->arg);
      ev_tpool_work_destroy(work);
    }

    pthread_mutex_lock(&(tm->work_mutex));
    tm->working_cnt--;
    if(!tm->stop && tm->working_cnt == 0 && tm->work_first == NULL)
      pthread_cond_signal(&tm->working_cond);
    pthread_mutex_unlock(&(tm->work_mutex));
  }

  tm->thread_cnt --;
  pthread_cond_signal(&(tm->working_cond));
  pthread_mutex_unlock(&(tm->work_mutex));
  return NULL;
}

EVCOREAPI ev_tpool_t *
ev_tpool_create(
  size_t num)
{
  ev_tpool_t *tm;
  pthread_t thread;
  size_t i;

  if(num == 0)
    num = DEFAULT_TPOOL_THREAD_COUNT;

  tm = calloc(1, sizeof(ev_tpool_t));
  tm->thread_cnt = num;

  pthread_mutex_init(&(tm->work_mutex), NULL);
  pthread_cond_init(&(tm->work_cond), NULL);
  pthread_cond_init(&(tm->working_cond), NULL);

  tm->work_first = NULL;
  tm->work_last = NULL;

  for( i = 0; i < num; ++i)
  {
    pthread_create(&thread, NULL, ev_tpool_worker, tm);
    pthread_detach(thread);
  }

  return tm;
}

EVCOREAPI void 
ev_tpool_destroy(
  ev_tpool_t *tm)
{
  ev_tpool_work_t *work;
  ev_tpool_work_t *work2;

  if(tm == NULL)
    return;

  pthread_mutex_lock(&(tm->work_mutex));
  work = tm->work_first;
  while(work != NULL) {
    work2 = work->next;
    ev_tpool_work_destroy(work);
    work = work2;
  }

  tm->stop = true;
  pthread_cond_broadcast(&(tm->work_cond));
  pthread_mutex_unlock(&(tm->work_mutex));

  ev_tpool_wait(tm);

  pthread_mutex_destroy(&(tm->work_mutex));
  pthread_cond_destroy(&(tm->work_cond));
  pthread_cond_destroy(&(tm->working_cond));


  free(tm);
}

EVCOREAPI bool 
ev_tpool_add_work(
  ev_tpool_t *tm, 
  thread_func_t func, 
  void *arg)
{
  ev_tpool_work_t *work;

  if(tm == NULL)
    return false;

  work = ev_tpool_work_create(func, arg);

  if(work == NULL)
    return false;

  pthread_mutex_lock(&(tm->work_mutex));
  if(tm->work_first == NULL) {
    tm->work_first = work;
    tm->work_last = tm->work_first;
  } else {
    tm->work_last->next = work;
    tm->work_last = work;
  }

  pthread_cond_broadcast(&(tm->work_cond));
  pthread_mutex_unlock(&(tm->work_mutex));

  return true;
}

EVCOREAPI void 
ev_tpool_wait(
  ev_tpool_t *tm)
{
  if(tm == NULL)
    return;

  pthread_mutex_lock(&(tm->work_mutex));
  while(1) {
    if((!tm->stop && (tm->working_cnt != 0 || tm->work_first != NULL)) || (tm->stop && tm->thread_cnt != 0)) {
      pthread_cond_wait(&(tm->working_cond), &(tm->work_mutex));
    } else {
      break;
    }
  }

  pthread_mutex_unlock(&(tm->work_mutex));
}
