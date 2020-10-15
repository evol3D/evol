#include "VulkanQueueManager.h"
#include <ev_log/ev_log.h>
#include <stdlib.h>

// NOTE:
// Currently, we only allocate 2 queues. One of which is used for Graphics work
// and the other is used for Compute/Transfer work. While this may seem counter-
// intuitive and that we should allocate as much queues as possible, the
// following link provides the reasoning for this decision:
// https://stackoverflow.com/questions/55272626/what-is-actually-a-queue-family-in-vulkan

static int ev_vulkanqueuemanager_init(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo** queueCreateInfos, unsigned int *queueCreateInfosCount);
static void ev_vulkanqueuemanager_retrievequeues(VkDevice logicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, unsigned int *queueCreateInfosCount);
static VkQueue ev_vulkanqueuemanager_getqueue(QueueType type);
static int ev_vulkanqueuemanager_deinit();

struct ev_VulkanQueueManager VulkanQueueManager = {
  .init = ev_vulkanqueuemanager_init,
  .deinit = ev_vulkanqueuemanager_deinit,
  .getQueue = ev_vulkanqueuemanager_getqueue,
  .retrieveQueues = ev_vulkanqueuemanager_retrievequeues,
};

const float priorityOne = 1.0f;

struct ev_VulkanQueueManagerData {

  VkQueueFamilyProperties *queueFamilyProperties;
  unsigned int queueFamilyCount;

  VkQueue** queues;
  unsigned int *queuesAllocCount;
  unsigned int *queuesUseCount;
} VulkanQueueManagerData;

#define DATA(a) VulkanQueueManagerData.a
#define PROPERTIES VulkanQueueManagerData.queueFamilyProperties
#define QUEUES VulkanQueueManagerData.queues
#define QUEUES_ALLOC_COUNT VulkanQueueManagerData.queuesAllocCount
#define QUEUES_USE_COUNT VulkanQueueManagerData.queuesUseCount

static int ev_vulkanqueuemanager_init(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo** queueCreateInfos, unsigned int *queueCreateInfosCount)
{
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &DATA(queueFamilyCount), NULL);
  PROPERTIES = malloc(sizeof(VkQueueFamilyProperties) * DATA(queueFamilyCount));
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &DATA(queueFamilyCount), PROPERTIES);

  QUEUES = malloc(sizeof(VkQueue*) * DATA(queueFamilyCount));
  QUEUES_ALLOC_COUNT = malloc(sizeof(unsigned int) * DATA(queueFamilyCount));
  QUEUES_USE_COUNT = malloc(sizeof(unsigned int) * DATA(queueFamilyCount));
  for(unsigned int i = 0; i < DATA(queueFamilyCount); ++i)
  {
    QUEUES[i] = malloc(sizeof(VkQueue) * PROPERTIES[i].queueCount);
    QUEUES_ALLOC_COUNT[i] = 0;
    QUEUES_USE_COUNT[i] = 0;
  }

  unsigned int graphicsIdx = 0;
  unsigned int computeIdx = 0;

  bool graphicsFound = false;
  bool computeFound = false;

  for(unsigned int i = 0; i < DATA(queueFamilyCount); ++i)
  {
    if(graphicsFound && computeFound) break;                    // If both queues' indices were found, exit the loop.

    if(!graphicsFound                                           // If the graphics index is not already found
        && (PROPERTIES[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)   // and the current family support graphics
        && (PROPERTIES[i].queueCount > QUEUES_ALLOC_COUNT[i]))  // and we didn't allocate the current family's maximum queue count
    {                                                           // then
      graphicsIdx = i;                                          // - Set the graphics queuefamily index to the current queuefamily
      graphicsFound = true;                                     // - Set the graphicsFound flag to true
      QUEUES_ALLOC_COUNT[i]++;
    }

    if(!computeFound                                            // If the compute index is not already found
        && graphicsFound                                        // and the graphics index is already found        N.B: This is to ensure that if there's only 1 queue in the queue family that supports graphics, it's not used to create the compute queue.
        && (PROPERTIES[i].queueFlags & VK_QUEUE_COMPUTE_BIT)    // and the current family support compute
        && (PROPERTIES[i].queueCount > QUEUES_ALLOC_COUNT[i]))  // and we didn't allocate the current family's maximum queue count
    {                                                           // then
      computeIdx = i;                                           // - Set the compute queuefamily index to the current queuefamily
      computeFound = true;                                      // - Set the computeFound flag to true
      QUEUES_ALLOC_COUNT[i]++;
    }

  }

  *queueCreateInfosCount = 2;
  *queueCreateInfos = malloc(sizeof(VkDeviceQueueCreateInfo) * (*queueCreateInfosCount));

  (*queueCreateInfos)[0] = (VkDeviceQueueCreateInfo)
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = graphicsIdx,
      .queueCount = 1,
      .pQueuePriorities = &priorityOne,
    };
  (*queueCreateInfos)[1] = (VkDeviceQueueCreateInfo)
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = computeIdx,
      .queueCount = 1,
      .pQueuePriorities = &priorityOne,
    };

  return 0;
}

static int ev_vulkanqueuemanager_deinit()
{

  for(unsigned int i = 0; i < VulkanQueueManagerData.queueFamilyCount; ++i)
  {
    free(QUEUES[i]);
  }

  free(QUEUES_ALLOC_COUNT);
  free(QUEUES);

  free(PROPERTIES);
  return 0;
}

static VkQueue ev_vulkanqueuemanager_getqueue(QueueType type)
{
  // TODO: A real search and return approach should be implemented.
  for(int i = 0; i < DATA(queueFamilyCount); ++i)
    if((PROPERTIES[i].queueFlags & type) && QUEUES_USE_COUNT[i])
      return QUEUES[i][0];
      
  ev_log_error("Couldn't find Queue of type: %d. Returning VK_NULL_HANDLE.", type);
  return VK_NULL_HANDLE;
}

static void ev_vulkanqueuemanager_retrievequeues(VkDevice logicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, unsigned int *queueCreateInfosCount)
{
  for(int i = 0; i < *queueCreateInfosCount; ++i)
  {
    unsigned int familyIdx = queueCreateInfos[i].queueFamilyIndex;
    if(QUEUES_USE_COUNT[familyIdx] >= PROPERTIES[familyIdx].queueCount)
    {
      ev_log_error("Exceeded maximum queue count `%d` for QueueFamily `%d`", PROPERTIES[familyIdx].queueCount, familyIdx);
    }
    vkGetDeviceQueue(logicalDevice, familyIdx, QUEUES_USE_COUNT[familyIdx]++, QUEUES[familyIdx]);
  }

  free(queueCreateInfos);
}
