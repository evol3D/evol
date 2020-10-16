#ifndef EVOL_VULKANQUEUEMANAGER_H
#define EVOL_VULKANQUEUEMANAGER_H

// The real header file (Vulkan)
#include "vulkan/vulkan.h"
#include "vulkan_utils.h"


typedef enum
{
  GRAPHICS       = VK_QUEUE_GRAPHICS_BIT,
  TRANSFER       = VK_QUEUE_TRANSFER_BIT,
  COMPUTE        = VK_QUEUE_COMPUTE_BIT,
  SPARSE_BINDING = VK_QUEUE_SPARSE_BINDING_BIT,

  QUEUE_TYPE_COUNT
} QueueType;

extern struct ev_VulkanQueueManager {
    int (*init)(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo** queueCreateInfos, unsigned int *queueCreateInfosCount);
    VkQueue (*getQueue)(QueueType type);
    int (*deinit)(void);
    void (*retrieveQueues)(VkDevice logicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, unsigned int *queueCreateInfosCount);
    unsigned int (*getFamilyIndex)(QueueType type);
} VulkanQueueManager;

#endif //EVOL_VULKANQUEUEMANAGER_H 
