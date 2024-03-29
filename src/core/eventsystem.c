#include <evol/core/evol.h>
#include <evol/core/eventsystem.h>
#include <evol/common/types/dvec.h>
#include <evol/threads/evolpthreads.h>

I32
ev_eventsystem_init(
  void);

I32
ev_eventsystem_deinit(
  void);

I32
ev_eventsystem_dispatch(
  ev_event_t event);

I32
ev_eventsystem_subscribe(
  ev_eventlistener_t *listener);

I32
ev_eventsystem_progress(
  void);

U32
ev_eventsystem_sync(
  void);

I32
ev_eventsystem_lock(
  void);

I32
ev_eventsystem_unlock(
  void);

struct ev_eventsystem EventSystem = {
  .init = ev_eventsystem_init,
  .deinit = ev_eventsystem_deinit,
  .dispatch = ev_eventsystem_dispatch,
  .subscribe = ev_eventsystem_subscribe,
  .progress = ev_eventsystem_progress,
  .sync = ev_eventsystem_sync,
};

struct ev_eventsystemdata {
  I64 updaterate;
  pthread_mutex_t systemlock;
} EventSystemData;

void
event_destr(
  ev_event_t *event)
{
  free(event->data);
}

void
vec_destr(
  vec_t *v)
{
  vec_fini(*v);
}

void
eventbuf_destr(
  dvec_t *buf)
{
  dvec_fini(*buf);
}

I32
ev_eventsystem_init(
  void)
{
  EventSystem.buffers = vec_init(dvec(ev_event_t), (elem_destr)eventbuf_destr);
  EventSystem.buffer_write_mutex = vec_init(pthread_mutex_t, (elem_destr)pthread_mutex_destroy);
  EventSystem.listeners = vec_init(vec(ev_eventlistener_t), (elem_destr)vec_destr);
  EventSystem.handlers_tpool = ev_tpool_create(0);

  pthread_mutex_init(&EventSystemData.systemlock, NULL);
  return 0;
}

I32
ev_eventsystem_deinit(
  void)
{
  ev_eventsystem_lock();
  vec_fini(EventSystem.buffers);
  vec_fini(EventSystem.buffer_write_mutex);
  vec_fini(EventSystem.listeners);
  ev_tpool_destroy(EventSystem.handlers_tpool);
  ev_eventsystem_unlock();
  pthread_mutex_destroy(&EventSystemData.systemlock);
}

I32
ev_eventsystem_dispatch(
  ev_event_t event)
{
  ev_eventtype_t primary_type = GET_PRIMARY_TYPE(event.type);
  pthread_mutex_lock(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[primary_type]);
  dvec_push(( (dvec_t *)EventSystem.buffers )[primary_type], &event);
  pthread_mutex_unlock(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[primary_type]);
}

U32
ev_eventsystem_sync(
  void)
{
  U32 prev_len = vec_len(EventSystem.buffers);
  U32 new_len = PRIMARY_EVENT_TYPE_COUNT + 1;
  I32 res = 0;

  if(new_len == prev_len) {
    return 0;
  }

  ev_eventsystem_lock();
  res |= vec_setlen(&EventSystem.listeners, new_len);
  res |= vec_setlen(&EventSystem.buffers, new_len);
  res |= vec_setlen(&EventSystem.buffer_write_mutex, new_len);

  for(U32 i = prev_len; i < new_len; i++) {
    EventSystem.listeners[i] = vec_init(ev_eventlistener_t);
    EventSystem.buffers[i] = dvec_init(ev_event_t, NULL, (elem_destr)event_destr);
    res |= pthread_mutex_init(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[i], NULL);
  }
  ev_eventsystem_unlock();

  return res;
}

I32
ev_eventsystem_subscribe(
  ev_eventlistener_t *listener)
{
  ev_eventtype_t primary_type = GET_PRIMARY_TYPE(listener->type);
  ev_eventsystem_lock();
  vec_push(&EventSystem.listeners[primary_type], listener);
  ev_eventsystem_unlock();
}

I32
ev_eventsystem_progress(
  void)
{
  ev_eventsystem_lock();
  for(U32 primary_type = 1; primary_type < vec_len(EventSystem.buffers); primary_type++) {
    dvec_t buffer = EventSystem.buffers[primary_type];
    if(vec_len(dvec_write(buffer)) == 0) {
      continue;
    }
    vec_t listeners_vec = EventSystem.listeners[primary_type];

    dvec_swap(buffer);

    ev_event_t *event;
    ev_eventlistener_t *listener;
    vec_foreach(event, dvec_read(buffer)) {
      vec_foreach(listener, listeners_vec) {
        if (!EVENT_MATCH(event->type, listener->type))
          continue;
        ev_tpool_add_work(EventSystem.handlers_tpool, listener->handler, event->data);
      }
    }
  }

  ev_tpool_wait(EventSystem.handlers_tpool);

  for(U32 primary_type = 1; primary_type < vec_len(EventSystem.buffers); primary_type++) {
    vec_clear(dvec_read(EventSystem.buffers[primary_type]));
  }
  ev_eventsystem_unlock();

  return 0;
}

I32
ev_eventsystem_lock(
  void)
{
  pthread_mutex_lock(&EventSystemData.systemlock);
}

I32
ev_eventsystem_unlock(
  void)
{
  pthread_mutex_unlock(&EventSystemData.systemlock);
}
