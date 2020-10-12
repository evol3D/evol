#ifndef EVOL_ASSET_LOADER_H
#define EVOL_ASSET_LOADER_H

#include "AssetTypes.h"

extern struct ev_AssetLoader {
    int (*init)();
    int (*deinit)();
    int (*loadGLTF)(const char*);
} AssetLoader;

#endif //EVOL_ASSET_LOADER_H