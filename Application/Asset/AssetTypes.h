#ifndef EVOL_ASSET_TYPES_H
#define EVOL_ASSET_TYPES_H

typedef struct Buffer
{
  void *data;
  unsigned int size;
  char *uri;
} Buffer;

typedef struct ev_BufferView
{
  unsigned int buffer_idx;
  unsigned int size;
  unsigned int offset;
} BufferView;

#endif //EVOL_ASSET_TYPES_H
