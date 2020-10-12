#ifndef EVOL_ASSET_STORE_H
#define EVOL_ASSET_STORE_H

#include "AssetTypes.h"

extern struct ev_AssetStore {
    int (*init)();
    int (*deinit)();
    unsigned int (*loadBuffer)(const char*);
    unsigned int (*getBufferIndex)(const char*);
} AssetStore;

k;
#endif //EVOL_ASSET_STORE_H
