#include "evol/common/types/vec.h"
#include <evol/common/types/dvec.h>

struct dvec{
  vec_t buffers[2];
  size_t read;
  size_t write;
};

vec_t
dvec_read(dvec_t dv)
{
  return dv->buffers[dv->read];
}

vec_t
dvec_write(dvec_t dv)
{
  return dv->buffers[dv->write];
}

int32_t
dvec_push(dvec_t dv, void *val)
{
  return vec_push(&dv->buffers[dv->write], val);
}

void
dvec_swap(dvec_t dv)
{
  size_t tmp = dv->read;
  dv->read = dv->write;
  dv->write = tmp;
}

dvec_t
dvec_init_impl(size_t elemsize, elem_copy copy, elem_destr destr)
{
  struct dvec *dv = malloc(sizeof(struct dvec));
  if(dv == NULL) {
    goto dvec_fail;
  }

  dv->read = 0;dv->write = 1;

  dv->buffers[0] = vec_init_impl(elemsize, copy, destr);
  if(dv->buffers[0] == NULL) {
    goto buffer_0_fail;
  }

  dv->buffers[1] = vec_init_impl(elemsize, copy, destr);
  if(dv->buffers[1] == NULL) {
    goto buffer_1_fail;
  }

  return (dvec_t) dv;

buffer_1_fail:
  vec_fini(dv->buffers[0]);
buffer_0_fail:
  free(dv);
dvec_fail:
  return NULL;
}

void
dvec_fini(dvec_t dv)
{
  vec_fini(dv->buffers[0]);
  vec_fini(dv->buffers[1]);
  free(dv);
}
