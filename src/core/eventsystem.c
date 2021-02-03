#include <evol/core/eventsystem.h>
#include <evol/common/types/dvec.h>

I32
ev_eventsystem_init(void);

I32
ev_eventsystem_deinit(void);

I32
ev_eventsystem_dispatch(ev_event_t event);

I32
ev_eventsystem_subscribe(ev_eventlistener_t *listener);

I32
ev_eventsystem_progress();

U32
ev_eventsystem_sync();

struct ev_eventsystem EventSystem = {
  .init = ev_eventsystem_init,
  .deinit = ev_eventsystem_deinit,
  .dispatch = ev_eventsystem_dispatch,
  .subscribe = ev_eventsystem_subscribe,
  .progress = ev_eventsystem_progress,
  .sync = ev_eventsystem_sync,
};

void
event_destr(ev_event_t *event)
{
  free(event->data);
}

void
vec_destr(vec_t *v)
{
  vec_fini(*v);
}

void
eventbuf_destr(dvec_t *buf)
{
  dvec_fini(*buf);
}


I32
ev_eventsystem_init(void)
{
  EventSystem.buffers = vec_init(dvec(ev_event_t), NULL, (elem_destr)eventbuf_destr);
  EventSystem.buffer_write_mutex = vec_init(pthread_mutex_t, NULL, (elem_destr)pthread_mutex_destroy);
  EventSystem.listeners = vec_init(vec(ev_eventlistener_t), NULL, (elem_destr)vec_destr);
  EventSystem.handlers_tpool = ev_tpool_create(0);

  return 0;
}

I32
ev_eventsystem_deinit(void)
{
  vec_fini(EventSystem.buffers);
  vec_fini(EventSystem.buffer_write_mutex);
  vec_fini(EventSystem.listeners);
  ev_tpool_destroy(EventSystem.handlers_tpool);
}

I32
ev_eventsystem_dispatch(ev_event_t event)
{
  ev_eventtype_t primary_type = GET_PRIMARY_TYPE(event.type);
  pthread_mutex_lock(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[primary_type]);
  dvec_push(( (dvec_t *)EventSystem.buffers )[primary_type], &event);
  pthread_mutex_unlock(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[primary_type]);
}

U32
ev_eventsystem_sync()
{
  U32 prev_len = vec_len(EventSystem.buffers);
  U32 new_len = PRIMARY_EVENT_TYPE_COUNT + 1;
  I32 res = 0;

  if(new_len == prev_len) {
    return 0;
  }

  res |= vec_setlen(EventSystem.listeners, new_len);
  res |= vec_setlen(EventSystem.buffers, new_len);
  res |= vec_setlen(EventSystem.buffer_write_mutex, new_len);

  for(U32 i = prev_len; i < new_len; i++) {
    ((vec_t *)EventSystem.listeners)[i] = vec_init(ev_eventlistener_t, NULL, NULL);
    ((dvec_t *)EventSystem.buffers)[i] = dvec_init(ev_event_t, NULL, (elem_destr)event_destr);
    res |= pthread_mutex_init(&((pthread_mutex_t *)EventSystem.buffer_write_mutex)[i], NULL);
  }

  return res;
}

I32
ev_eventsystem_subscribe(ev_eventlistener_t *listener)
{
  ev_eventtype_t primary_type = GET_PRIMARY_TYPE(listener->type);
  vec_push(((vec_t*)EventSystem.listeners)[primary_type], listener);
}

I32
ev_eventsystem_progress()
{
  for(U32 primary_type = 1; primary_type <= PRIMARY_EVENT_TYPE_COUNT; primary_type++) {
    dvec_t buffer = ((dvec_t *)EventSystem.buffers)[primary_type];
    dvec_swap(buffer);
    vec_t read = dvec_read(buffer);

    for(U32 event_idx = 0; event_idx < vec_len(read); event_idx++) {
      for(U32 listener_idx = 0; listener_idx < vec_len(EventSystem.listeners); listener_idx++) {
        ev_event_t event = ((ev_event_t *)read)[event_idx];
        ev_eventlistener_t listener = ((ev_eventlistener_t *)EventSystem.listeners)[listener_idx];
        if (!EVENT_MATCH(event.type, listener.type)) {
          continue;
        }
        ev_tpool_add_work(EventSystem.handlers_tpool, listener.handler, event.data);
      }
    }
  }

  ev_tpool_wait(EventSystem.handlers_tpool);

  for(U32 primary_type = 1; primary_type <= PRIMARY_EVENT_TYPE_COUNT; primary_type++) {
    vec_clear(((dvec_t *)EventSystem.buffers)[primary_type]);
  }

  return 0;
}
