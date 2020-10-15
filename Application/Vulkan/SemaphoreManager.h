#ifndef EVOL_SEMAPHOREMANAGER_H
#define EVOL_SEMAPHOREMANAGER_H

// The real header file (Vulkan)
#include "vulkan/vulkan.h"

extern struct ev_SemaphoreManager {
    int (*init)(void);
    int (*deinit)(void);
} SemaphoreManager;

#endif //EVOL_VULKANQUEUEMANAGER_H 
