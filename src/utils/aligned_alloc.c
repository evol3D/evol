#include <evol/utils/aligned_alloc.h>
#include <stdlib.h>

void *
aligned_malloc(
    size_t bytes, 
    size_t alignment)
{
  size_t offset = (alignment - 1) + sizeof(void *);
  void *orig_block = malloc(bytes + offset);
  if(orig_block == NULL) {
    return NULL;
  }

  void *aligned_block = (void *) ((((size_t)orig_block) + offset) & ~(alignment - 1));
  ((void**)aligned_block)[-1] = orig_block;

  return aligned_block;
}

void *
aligned_free(
    void *alig_ptr)
{
  free(((void**)alig_ptr)[-1]);
}
