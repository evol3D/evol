#ifndef EVOL_ASSET_STORE_H
#define EVOL_ASSET_STORE_H

extern struct ev_AssetStore {
    int (*init)();
    int (*deinit)();
    unsigned int (*loadBuffer)(const char*);
    unsigned int (*getBufferIndex)(const char*);
} AssetStore;

typedef struct Buffer
{
  void *data;
  unsigned int size;
  char *uri;
} Buffer;

#endif //EVOL_ASSET_LOADER_H
