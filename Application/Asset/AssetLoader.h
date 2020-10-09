#ifndef EVOL_ASSET_LOADER_H
#define EVOL_ASSET_LOADER_H

extern struct ev_AssetLoader {
    int (*init)();
    int (*deinit)();
    int (*loadGLTF)(const char*);
} AssetLoader;


typedef struct ev_BufferView
{
  unsigned int buffer_idx;
  unsigned int size;
  unsigned int offset;
} BufferView;

#endif //EVOL_ASSET_LOADER_H
