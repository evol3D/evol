#define EV_CORE_FILE
#include <evol/core/evstore.h>
#include <evol/common/ev_log.h>
#include <hashmap.h>
#include <string.h>
#include <evolpthreads.h>

struct evstore {
  struct hashmap *map;
  pthread_rwlock_t rwlock;
};

uint64_t evstore_entry_hash(const void* data, uint64_t seed0, uint64_t seed1)
{
  const evstore_entry_t *entry = data;
  return hashmap_murmur(entry->key, strlen(entry->key), seed0, seed1);
}

int32_t evstore_entry_compare(const void* data1, const void* data2, void* _udata)
{
  (void)_udata;
  const evstore_entry_t *entry1 = data1;
  const evstore_entry_t *entry2 = data2;

  return strcmp(entry1->key, entry2->key);
}

bool entry_clear(const void *data, void *_udata)
{
  (void)_udata;
  evstore_entry_t *entry = data;
  if(entry->free)
    entry->free(entry->data);
  return true;
}

evstore_t *
evstore_create()
{
  U32 entry_size = sizeof(evstore_entry_t);
  U32 init_cap = 16;

  U64 seed0 = 0;
  U64 seed1 = 0;

  PTR udata = NULL;

  struct hashmap *map = hashmap_new(
    entry_size, init_cap,
    seed0, seed1,
    evstore_entry_hash, evstore_entry_compare,
    udata
  );

  if(!map) {
    goto map_creation_failed;
  }

  evstore_t *store = malloc(sizeof(evstore_t));
  if(!store) {
    goto store_allocation_failed;
  }

  store->map = map;
  I32 rwlock_init_res = pthread_rwlock_init(&store->rwlock, NULL);
  if(rwlock_init_res) {
    ev_log_error("Couldn't initialized rwlock for evstore. ERROR: ", rwlock_init_res);
    goto rwlock_init_failed;
  }

  return store;

rwlock_init_failed:
  free(store);
store_allocation_failed:
  hashmap_free(map);
map_creation_failed:
  return NULL;
}

void
evstore_destroy(evstore_t *store)
{
  if(store == NULL)
    return;

  evstore_clear(store);
  hashmap_free(store->map);
  pthread_rwlock_destroy(&store->rwlock);
  free(store);
}

EvStoreResult
evstore_get(evstore_t *store, const char *key, evstore_entry_t *res)
{
  I32 res_err = 0;
  evstore_entry_t search_entry = {.key = key};

  pthread_rwlock_rdlock(&store->rwlock);
  evstore_entry_t *found = (evstore_entry_t*)hashmap_get(store->map, &search_entry);

  if(found == NULL) {
    res_err = EV_STORE_ENTRY_NOTFOUND;
  } else {
    *res = *found;
    res_err = EV_STORE_ENTRY_FOUND;
  }
  pthread_rwlock_unlock(&store->rwlock);

  return res_err;
}

EvStoreResult
evstore_get_checktype(evstore_t *store, const char *key, ev_type type, evstore_entry_t *res)
{
  I32 res_err = 0;
  evstore_entry_t search_entry = {.key = key};

  pthread_rwlock_rdlock(&store->rwlock);
  evstore_entry_t *found = (evstore_entry_t*)hashmap_get(store->map, &search_entry);

  if(found == NULL) {
    res_err = EV_STORE_ENTRY_NOTFOUND;
  } else if(found->type != type) {
    res_err = EV_STORE_ENTRY_WRONGTYPE;
  } else {
    *res = *found;
    res_err = EV_STORE_ENTRY_FOUND;
  }
  pthread_rwlock_unlock(&store->rwlock);

  return res_err;
}

EvStoreResult
evstore_set(evstore_t *store, evstore_entry_t *entry)
{
  pthread_rwlock_wrlock(&store->rwlock);
  evstore_entry_t *prev_entry = hashmap_set(store->map, entry);
  pthread_rwlock_unlock(&store->rwlock);

  if(prev_entry == NULL) {
    if(hashmap_oom(store->map) == true) {
      return EV_STORE_ERROR_OOM;
    }
  } else {
    if(prev_entry->free) {
      prev_entry->free(prev_entry->data);
    }
  }

  return EV_STORE_SUCCESS;
}

EvStoreResult
evstore_clear(evstore_t *store)
{
  pthread_rwlock_wrlock(&store->rwlock);
  hashmap_scan(store->map, entry_clear, NULL);
  pthread_rwlock_unlock(&store->rwlock);

  return EV_STORE_SUCCESS;
}
