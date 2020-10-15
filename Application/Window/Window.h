#ifndef EVOL_WINDOW_H
#define EVOL_WINDOW_H

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_WINDOW_TITLE "Default Title"
#define MAX_WINDOW_TITLE_LENGTH 256         // Arbitrary number, unlikely to need more

#include "stdbool.h"

#include <vulkan/vulkan.h>

extern struct _ev_Window {
    int (*init)();
    int (*createWindow)();
    int (*setSize)(unsigned int, unsigned int);
    int (*deinit)();
    void *(*getWindowHandle)();
    int (*setTitle)(const char *);
    bool (*shouldClose)();
    void (*pollEvents)();
    bool (*isCreated)();
    double (*getTime)();
    void (*setShouldClose)(bool );
    VkResult (*createVulkanSurface)(VkInstance, VkSurfaceKHR*);
    void (*getSize)(unsigned int *, unsigned int *);
} Window;

#endif //EVOL_WINDOW_H
