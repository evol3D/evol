#include <evol/common/types/vec.h>
#include <stdlib.h>
#include <string.h>

struct vec_meta {
  size_t length;
  size_t capacity;

  size_t elemsize;

  elem_copy  copy_fn;
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
  *(char *)iter += metadata->elemsize;
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

#define vec_init(type, copy, destr) vec_init_impl(sizeof(type), copy, destr)

int32_t
vec_push(vec_t *v, void *val)
{
  void *           buf      = ((char *)(*v) - sizeof(struct vec_meta));
  struct vec_meta *metadata = (struct vec_meta *)buf;

  if (metadata->length == metadata->capacity) {
    void *tmp =
      realloc(buf,
              sizeof(struct vec_meta) +
                (metadata->capacity * metadata->elemsize * VEC_GROWTH_RATE));
    if (!tmp) {
      return 1;
    }

    buf = tmp;

    metadata           = (struct vec_meta *)buf;
    metadata->capacity = metadata->capacity * VEC_GROWTH_RATE;

    *v = (char *)buf + sizeof(struct vec_meta);
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
