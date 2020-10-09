#include "AssetStore.h"
#include "stdio.h"
#include <vec.h>
#include <string.h>

static int ev_assetstore_init();
static int ev_assetstore_deinit();
static unsigned int ev_assetstore_loadbuffer(const char *);
static void ev_assetstore_destroybuffer(Buffer *b);
static unsigned int ev_assetstore_getbufferindex(const char*);

struct ev_AssetStore AssetStore = {
  .init   = ev_assetstore_init,
  .deinit = ev_assetstore_deinit,
  .loadBuffer = ev_assetstore_loadbuffer,
  .getBufferIndex = ev_assetstore_getbufferindex,
};

typedef vec_t(Buffer) buffer_vec_t;

struct ev_AssetStore_Data {
  buffer_vec_t buffers;
} AssetStoreData;

static int ev_assetstore_init()
{
  vec_init(&AssetStoreData.buffers);
  return 0;
}

static int ev_assetstore_deinit()
{
  // Destroy all dangling buffers
  Buffer *b; int i;
  vec_foreach_ptr(&AssetStoreData.buffers, b, i) {
    ev_assetstore_destroybuffer(b);
  };

  // Destroy the vector
  vec_deinit(&AssetStoreData.buffers);
  return 0;
}

static unsigned int ev_assetstore_loadbuffer(const char* uri)
{  
  FILE* file = fopen(uri, "rb");
  if(!file) return -1;

  Buffer newBuffer;

  fseek(file, 0, SEEK_END);
  newBuffer.size = ftell(file);
  fseek(file, 0, SEEK_SET);

  newBuffer.data = malloc(newBuffer.size);

  fread(newBuffer.data, 1, newBuffer.size, file);
  fclose(file);

  newBuffer.uri = malloc(strlen(uri) + 1);
  strcpy(newBuffer.uri, uri);

  unsigned int idx = AssetStoreData.buffers.length;
  vec_push(&(AssetStoreData.buffers), newBuffer);

  return idx;
}

static void ev_assetstore_destroybuffer(Buffer *b)
{
  if(b->data) free(b->data);
  if(b->uri) free(b->uri);
  b->size = 0;
}

// TODO change this to using a map instead
static unsigned int ev_assetstore_getbufferindex(const char* uri)
{
  Buffer *b; int i = 0;
  vec_foreach_ptr(&AssetStoreData.buffers, b, i) {
    if(!strcmp(b->uri, uri)) break;
  };

  if(i == AssetStoreData.buffers.length) i = -1;

  return i;
}
