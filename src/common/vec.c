/*!
 * \file vec.c
 */
#include <evol/common/types/vec.h>
#include <stdlib.h>
#include <string.h>

//! Metadata that is stored with a vector. Unique to each vector.
struct vec_meta_t {
  //! The number of elements in the vector.
  size_t length;
  //! The maximum length of the vector before it needs to be resized.
  size_t capacity;
  //! The size (in bytes) of memory that each element takes.
  size_t elemsize;

  //! If set, the vector uses this function to copy new values into the vector.
  elem_copy copy_fn;
  //! If set, the vector uses this function to destroy stored values.
  elem_destr destr_fn;
};

#define vec_meta(v) \
  ((struct vec_meta_t *)((char *)(v) - sizeof(struct vec_meta_t)))

vec_t
vec_init_impl(size_t elemsize, elem_copy copy, elem_destr destr)
{
  void *v = malloc(sizeof(struct vec_meta_t) + (VEC_INIT_CAP * elemsize));
  if (!v)
    return NULL;

  struct vec_meta_t *metadata = (struct vec_meta_t *)v;
  *metadata = (struct vec_meta_t){
    .length   = 0,
    .capacity = VEC_INIT_CAP,
    .elemsize = elemsize,
    .copy_fn  = copy,
    .destr_fn = destr,
  };

  return (vec_t)((char *)v + sizeof(struct vec_meta_t));
}

void *
vec_iter_begin(vec_t v)
{
  return v;
}

void *
vec_iter_end(vec_t v)
{
  struct vec_meta_t *metadata = vec_meta(v);
  return ((char *)v) + (metadata->elemsize * metadata->length);
}

void
vec_iter_next(vec_t v, void **iter)
{
  struct vec_meta_t *metadata = vec_meta(v);
  *iter = ((char*)*iter) + metadata->elemsize;
}

void
vec_fini(vec_t v)
{
  struct vec_meta_t *metadata = vec_meta(v);
  if (metadata->destr_fn) {
    for (void *elem = vec_iter_begin(v); elem != vec_iter_end(v);
         vec_iter_next(v, &elem)) {
      metadata->destr_fn(elem);
    }
  }
  free((char *)v - sizeof(struct vec_meta_t));
}

int32_t
vec_push(vec_t *v, void *val)
{
  struct vec_meta_t *metadata = vec_meta(*v);

  if (metadata->length == metadata->capacity) {
    if(vec_grow(v)) {
      return 1;
    } else {
      metadata = vec_meta(*v);
    }
  }

  void *dst = ((char *)*v) + (metadata->length * metadata->elemsize);
  if (metadata->copy_fn) {
    metadata->copy_fn(dst, val);
  } else {
    memcpy(dst, val, metadata->elemsize);
  }
  metadata->length++;
  return 0;
}

size_t
vec_len(vec_t v)
{
  return (vec_meta(v))->length;
}

size_t
vec_capacity(vec_t v)
{
  return (vec_meta(v))->capacity;
}

int32_t
vec_clear(vec_t v)
{
  struct vec_meta_t *metadata = vec_meta(v);
  if (metadata->destr_fn) {
    for (void *elem = vec_iter_begin(v); elem != vec_iter_end(v);
         vec_iter_next(v, &elem)) {
      metadata->destr_fn(elem);
    }
  }

  metadata->length = 0;
}

int32_t
vec_setlen(vec_t *v, size_t len)
{
  struct vec_meta_t *metadata = vec_meta(*v);

  while(len > metadata->capacity) {
    if(vec_grow(v)) {
      return 1;
    }
    metadata = vec_meta(*v);
  }

  metadata->length = len;
  return 0;
}

int32_t
vec_setcapacity(vec_t *v, size_t cap)
{
  struct vec_meta_t *metadata = vec_meta(*v);
  if(metadata->capacity == cap) {
    return 0;
  }

  void *buf = ((char *)(*v) - sizeof(struct vec_meta_t));
  void *tmp = realloc(buf, sizeof(struct vec_meta_t) + (cap * metadata->elemsize));

  if (!tmp) {
    return 1;
  }

  buf = tmp;

  metadata           = (struct vec_meta_t *)buf;
  metadata->capacity = cap;

  *v = (char *)buf + sizeof(struct vec_meta_t);
  return 0;
}

int32_t
vec_grow(vec_t *v)
{
  return vec_setcapacity(v, vec_meta(*v)->capacity * VEC_GROWTH_RATE);
}
