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

    void (*createSurface)(VkSurfaceKHR *surface);
    void (*destroySurface)(VkSurfaceKHR surface);

    void (*createSwapchain)(unsigned int* imageCount, VkSurfaceKHR* surface, VkSurfaceFormatKHR *surfaceFormat, VkSwapchainKHR* swapchain);
    void (*destroySwapchain)(VkSwapchainKHR swapchain);
    void (*retrieveSwapchainImages)(VkSwapchainKHR swapchain, unsigned int * imageCount, VkImage ** images);

    void (*createImage)(VkImageCreateInfo *, VmaAllocationCreateInfo *, EvImage *);
    void (*destroyImage)(EvImage *);
    void (*createBuffer)(VkBufferCreateInfo *, VmaAllocationCreateInfo *, EvBuffer *);
    void (*destroyBuffer)(EvBuffer *);

    void (*allocateMemoryPool)(VmaPoolCreateInfo *poolCreateInfo, VmaPool* pool);
    void (*allocateBufferInPool)(VkBufferCreateInfo *bufferCreateInfo, VmaPool pool, EvBuffer *buffer);

    void (*memoryDump)();

    void (*createImageViews)(unsigned int imageCount, VkFormat imageFormat, VkImage *images, VkImageView **views);
    void (*createFramebuffer)(VkImageView* attachments, unsigned int attachmentCount, VkRenderPass renderPass, VkFramebuffer *framebuffer);

    void (*startNewFrame)(void);
    void (*endFrame)(void);

    VkDevice (*getDevice)(void);
    VkPhysicalDevice (*getPhysicalDevice)(void);
    VkCommandPool (*getCommandPool)(QueueType);

    VmaAllocator (*getAllocator)();

} Vulkan;


#endif //EVOL_VULKAN_H 
