/*!
 * \file vec.c
 */
#include <evol/common/types/vec.h>
#include <stdlib.h>
#include <string.h>

//! Metadata that is stored with a vector. Unique to each vector.
struct vec_meta {
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

vec_t
vec_init_impl(size_t elemsize, elem_copy copy, elem_destr destr)
{
  void *v = malloc(sizeof(struct vec_meta) + (VEC_INIT_CAP * elemsize));
  if (!v)
    return NULL;

  struct vec_meta *metadata = (struct vec_meta *)v;
  *metadata                 = (struct vec_meta){
    .length   = 0,
    .capacity = VEC_INIT_CAP,
    .elemsize = elemsize,
    .copy_fn  = copy,
    .destr_fn = destr,
  };

  return (vec_t)((char *)v + sizeof(struct vec_meta));
}

void *
vec_iter_begin(vec_t v)
{
  return v;
}

void *
vec_iter_end(vec_t v)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
  return ((char *)v) + (metadata->elemsize * metadata->length);
}

void
vec_iter_next(vec_t v, void **iter)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
  *iter = ((char*)*iter) + metadata->elemsize;
}

void
vec_fini(vec_t v)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
  if (metadata->destr_fn) {
    for (void *elem = vec_iter_begin(v); elem != vec_iter_end(v);
         vec_iter_next(v, &elem)) {
      metadata->destr_fn(elem);
    }
  }
  free((char *)v - sizeof(struct vec_meta));
}

int32_t
vec_push(vec_t *v, void *val)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)(*v) - sizeof(struct vec_meta));

  if (metadata->length == metadata->capacity) {
    if(vec_setcapacity(v, metadata->capacity * VEC_GROWTH_RATE)) {
      return 1;
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
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
  return metadata->length;
}

size_t
vec_capacity(vec_t v)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
  return metadata->capacity;
}

int32_t
vec_clear(vec_t v)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)v - sizeof(struct vec_meta));
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
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)(*v) - sizeof(struct vec_meta));

  if(len > metadata->capacity && vec_setcapacity(v, len)) {
      return 1;
  }

  metadata->length = len;
  return 0;
}

int32_t
vec_setcapacity(vec_t *v, size_t cap)
{
  struct vec_meta *metadata =
    (struct vec_meta *)((char *)(*v) - sizeof(struct vec_meta));
  if(metadata->capacity == cap) {
    return 0;
  }

  void *buf = ((char *)(*v) - sizeof(struct vec_meta));
  void *tmp = realloc(buf, sizeof(struct vec_meta) + (cap * metadata->elemsize));

  if (!tmp) {
    return 1;
  }

  buf = tmp;

  metadata           = (struct vec_meta *)buf;
  metadata->capacity = cap;

  *v = (char *)buf + sizeof(struct vec_meta);
  return 0;
}
