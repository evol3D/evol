#ifndef EVOL_VULKAN_H
#define EVOL_VULKAN_H

// The real header file (Vulkan)
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#include "vulkan_utils.h"
#include "VulkanQueueManager.h"

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

extern struct ev_Vulkan {
    int (*init)(void);
    int (*deinit)(void);

    void (*createSurface)(void);

    void (*createSwapchain)(unsigned int*);
    void (*destroySwapchain)(void);
    void (*createImage)(VkImageCreateInfo *, VmaAllocationCreateInfo *, EvImage *);
    void (*destroyImage)(EvImage *);
    void (*createBuffer)(VkBufferCreateInfo *, VmaAllocationCreateInfo *, EvBuffer *);
    void (*destroyBuffer)(EvBuffer *);

    void (*startNewFrame)(void);
    void (*endFrame)(void);
    VkCommandBuffer (*getCurrentFrameCommandBuffer)(void);

    VkShaderModule (*loadShader)(const char *);
    void (*unloadShader)(VkShaderModule);

    VkDevice (*getDevice)(void);
    VkRenderPass (*getRenderPass)(void);
    VkCommandPool (*getCommandPool)(QueueType);

} Vulkan;


#endif //EVOL_VULKAN_H 
