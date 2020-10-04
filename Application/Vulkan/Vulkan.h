#ifndef EVOL_VULKAN_H
#define EVOL_VULKAN_H

// The real header file (Vulkan)
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "vulkan_utils.h"

typedef unsigned int VkApiVersion;

typedef struct
{
  VkImage image;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
} EvImage;

typedef struct
{
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
} EvBuffer;

typedef enum
{
    GRAPHICS,
    TRANSFER,
    COMPUTE,
    SPARSE_BINDING,

    QUEUE_TYPE_COUNT
} QueueType;

extern struct ev_Vulkan {
    int (*init)();
    int (*deinit)();
    void (*createSwapchain)(unsigned int*);
    void (*destroySwapchain)();
    void (*createImage)(VkImageCreateInfo *, VmaAllocationCreateInfo *, EvImage *);
    void (*destroyImage)(EvImage *);
    void (*createBuffer)(VkBufferCreateInfo *, VmaAllocationCreateInfo *, EvBuffer *);
    void (*destroyBuffer)(EvBuffer *);

    VkShaderModule (*loadShader)(const char *);
    void (*unloadShader)(VkShaderModule);

    VkDevice (*getDevice)();
    VkRenderPass (*getRenderPass)();
} Vulkan;


#endif //EVOL_VULKAN_H 
