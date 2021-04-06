#pragma once

#include <evol/common/ev_macros.h>

#ifdef EV_OS_WINDOWS
# include <stdbool.h>
# include <windows.h>
# include <time.h>
#else
# include <pthread.h>
#endif

// pthread types in win32
#ifdef EV_OS_WINDOWS
typedef CRITICAL_SECTION pthread_mutex_t;
typedef void pthread_mutexattr_t;
typedef void pthread_rwlockattr_t;
typedef void pthread_attr_t;
typedef void pthread_condattr_t;
typedef HANDLE pthread_t;
typedef CONDITION_VARIABLE pthread_cond_t;

typedef struct {
  SRWLOCK lock;
  bool exclusive;
} pthread_rwlock_t;

#endif

// pthread functions
#ifdef EV_OS_WINDOWS
EVCOREAPI int 
pthread_create(
  pthread_t *thread, 
  pthread_attr_t *attr, 
  void *(*start_routine)(void *), 
  void *arg);

EVCOREAPI int 
pthread_join(
  pthread_t thread, 
  void **value_ptr);

EVCOREAPI int 
pthread_detach(
  pthread_t);

EVCOREAPI int 
pthread_mutex_init(
  pthread_mutex_t *mutex, 
  pthread_mutexattr_t *attr);

EVCOREAPI int 
pthread_mutex_destroy(
  pthread_mutex_t *mutex);

EVCOREAPI int 
pthread_mutex_lock(
  pthread_mutex_t *mutex);

EVCOREAPI int 
pthread_mutex_unlock(
  pthread_mutex_t *mutex);


EVCOREAPI int 
pthread_cond_init(
  pthread_cond_t *cond, 
  pthread_condattr_t *attr);

EVCOREAPI int 
pthread_cond_destroy(
  pthread_cond_t *cond);

EVCOREAPI int 
pthread_cond_wait(
  pthread_cond_t *cond, 
  pthread_mutex_t *mutex);

EVCOREAPI int 
pthread_cond_timedwait(
  pthread_cond_t *cond, 
  pthread_mutex_t *mutex, 
  const struct timespec *abstime);

EVCOREAPI int 
pthread_cond_signal(
  pthread_cond_t *cond);

EVCOREAPI int 
pthread_cond_broadcast(
  pthread_cond_t *cond);

EVCOREAPI int 
pthread_rwlock_init(
  pthread_rwlock_t *rwlock, 
  const pthread_rwlockattr_t *attr);

EVCOREAPI int 
pthread_rwlock_destroy(
  pthread_rwlock_t *rwlock);

EVCOREAPI int 
pthread_rwlock_rdlock(
  pthread_rwlock_t *rwlock);

EVCOREAPI int 
pthread_rwlock_tryrdlock(
  pthread_rwlock_t *rwlock);

EVCOREAPI int 
pthread_rwlock_wrlock(
  pthread_rwlock_t *rwlock);

EVCOREAPI int 
pthread_rwlock_trywrlock(
  pthread_rwlock_t  *rwlock);

EVCOREAPI int 
pthread_rwlock_unlock(
  pthread_rwlock_t *rwlock);

EVCOREAPI pthread_t 
pthread_self();
#endif

// misc
EVCOREAPI unsigned int 
get_num_procs();

EVCOREAPI void 
ms_to_timespec(
  struct timespec *ts, 
  unsigned int ms);
