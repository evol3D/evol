//TODO Comments / Logging
// Evol's Vulkan module header
#include "Vulkan.h"
#include "Window.h"
#include "vulkan_utils.h"
#include "VulkanQueueManager.h"
#include <ev_log/ev_log.h>

#include "stdio.h"
#include "stdlib.h"

static int ev_vulkan_init();
static int ev_vulkan_deinit();

static void ev_vulkan_create_instance();
static void ev_vulkan_detect_physical_device();
static void ev_vulkan_create_logical_device();

static void ev_vulkan_init_vma();

static void ev_vulkan_createsurface(VkSurfaceKHR *surface);
static void ev_vulkan_destroysurface(VkSurfaceKHR surface);

static void ev_vulkan_createswapchain(unsigned int* imageCount, VkSurfaceKHR* surface, VkSurfaceFormatKHR *surfaceFormat, VkSwapchainKHR* swapchain);
static void ev_vulkan_retrieveswapchainimages(VkSwapchainKHR swapchain, unsigned int * imageCount, VkImage ** images);
static void ev_vulkan_destroyswapchain(VkSwapchainKHR swapchain);

static void ev_vulkan_createimageviews(unsigned int imageCount, VkFormat imageFormat, VkImage *images, VkImageView **views);
static void ev_vulkan_createframebuffer(VkImageView* attachments, unsigned int attachmentCount, VkRenderPass renderPass, VkFramebuffer *framebuffer);

static void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image);
static void ev_vulkan_destroy_image(EvImage *image);

static void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer);
static void ev_vulkan_destroy_buffer(EvBuffer *buffer);

static void ev_vulkan_allocatememorypool(VmaPoolCreateInfo *poolCreateInfo, VmaPool* pool);
static void ev_vulkan_freememorypool(VmaPool pool);

static void ev_vulkan_allocatebufferinpool(VkBufferCreateInfo *bufferCreateInfo, VmaPool pool, EvBuffer *buffer);
static void ev_vulkan_allocateimageinpool(VkImageCreateInfo *imageCreateInfo, VmaPool pool, EvImage *image);

static void ev_vulkan_allocateprimarycommandbuffer(QueueType queueType, VkCommandBuffer *cmdBuffer);

static void ev_vulkan_memorydump();

//getters
static inline VkDevice ev_vulkan_get_logical_device();
static inline VkPhysicalDevice ev_vulkan_get_physical_device();
static inline VkCommandPool ev_vulkan_get_commandpool(QueueType type);
static inline VmaAllocator ev_vulkan_getallocator();

struct ev_Vulkan Vulkan = {
  .init                         = ev_vulkan_init,
  .deinit                       = ev_vulkan_deinit,

  .createSurface                = ev_vulkan_createsurface,
  .destroySurface               = ev_vulkan_destroysurface,

  .createSwapchain              = ev_vulkan_createswapchain,
  .retrieveSwapchainImages      = ev_vulkan_retrieveswapchainimages,

  // Memory
  .createImage                  = ev_vulkan_create_image,
  .destroyImage                 = ev_vulkan_destroy_image,
  .createBuffer                 = ev_vulkan_create_buffer,
  .destroyBuffer                = ev_vulkan_destroy_buffer,

  .allocateMemoryPool           = ev_vulkan_allocatememorypool,
  .freeMemoryPool               = ev_vulkan_freememorypool,

  .allocateBufferInPool         = ev_vulkan_allocatebufferinpool,
  .allocateImageInPool          = ev_vulkan_allocateimageinpool,

  .createImageViews             = ev_vulkan_createimageviews,
  .createFramebuffer            =  ev_vulkan_createframebuffer,

  // Getters
  .getDevice                    = ev_vulkan_get_logical_device,
  .getPhysicalDevice            = ev_vulkan_get_physical_device,
  .getCommandPool               = ev_vulkan_get_commandpool,
  .getAllocator                 = ev_vulkan_getallocator,

  .destroySwapchain             = ev_vulkan_destroyswapchain,

  .allocatePrimaryCommandBuffer = ev_vulkan_allocateprimarycommandbuffer,


  // Debug
  .memoryDump                   = ev_vulkan_memorydump,
};

struct ev_Vulkan_Data {
  VkInstance instance;
  VkApiVersion apiVersion;

  VmaAllocator allocator;

  VkPhysicalDevice physicalDevice;
  VkDevice logicalDevice;

  VkCommandPool commandPools[QUEUE_TYPE_COUNT];
} VulkanData;

static int ev_vulkan_init()
{
  // Zeroing up commandpools to identify the ones that get initialized
  for(int i = 0; i < QUEUE_TYPE_COUNT; ++i)
  {
    VulkanData.commandPools[i] = 0;
  }

  // Create the vulkan instance
  ev_log_debug("Creating Vulkan Instance");
  ev_vulkan_create_instance();
  ev_log_debug("Created Vulkan Instance Successfully");

  // Detect the physical device
  ev_log_debug("Detecting Vulkan Physical Device");
  ev_vulkan_detect_physical_device();
  ev_log_debug("Detected Vulkan Physical Device");

  // Create the logical device
  ev_log_debug("Creating Vulkan Logical Device");
  ev_vulkan_create_logical_device();
  ev_log_debug("Created Vulkan Logical Device");

  // Initialize VMA
  ev_log_debug("Initializing VMA");
  ev_vulkan_init_vma();
  ev_log_debug("Initialized VMA");

  return 0;
}

static int ev_vulkan_deinit()
{
  // Destroy any commandpool that was created earlier
  for(int i = 0; i < QUEUE_TYPE_COUNT; ++i)
    if(VulkanData.commandPools[i])
      vkDestroyCommandPool(VulkanData.logicalDevice, VulkanData.commandPools[i], NULL);

  // Destroy VMA
  vmaDestroyAllocator(VulkanData.allocator);

  // Destroy the logical device
  vkDestroyDevice(VulkanData.logicalDevice, NULL);

  // Destroy the vulkan instance
  vkDestroyInstance(VulkanData.instance, NULL);

  return 0;
}

static void ev_vulkan_destroysurface(VkSurfaceKHR surface)
{
  // Destroy the vulkan surface
  vkDestroySurfaceKHR(VulkanData.instance, surface, NULL);
}

static void ev_vulkan_create_instance()
{
  const char *extensions[] = {
    "VK_KHR_surface",
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    #ifdef _WIN32
    "VK_KHR_win32_surface",
    #else
    "VK_KHR_xcb_surface",
    #endif
  };

  const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
  };

  VkApplicationInfo applicationInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "evol_vulkan",
    .applicationVersion = 0,
    .apiVersion = VK_API_VERSION_1_2,
  };

  VkInstanceCreateInfo instanceCreateInfo = {
    .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo        = &applicationInfo,
    .enabledLayerCount       = ARRAYSIZE(validation_layers),
    .ppEnabledLayerNames     = validation_layers,
    .enabledExtensionCount   = ARRAYSIZE(extensions),
    .ppEnabledExtensionNames = extensions
  };

  VK_ASSERT(vkCreateInstance(&instanceCreateInfo, NULL, &VulkanData.instance));
}

static void ev_vulkan_detect_physical_device()
{
  unsigned int physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices( VulkanData.instance, &physicalDeviceCount, NULL);

  if(!physicalDeviceCount)
    assert(!"No physical devices found");

  VkPhysicalDevice *physicalDevices = malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices( VulkanData.instance, &physicalDeviceCount, physicalDevices);

  VulkanData.physicalDevice = physicalDevices[0];

  for(unsigned int i = 0; i < physicalDeviceCount; ++i)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      VulkanData.physicalDevice = physicalDevices[i];
      break;
    }
  }

  free(physicalDevices);
}

static void ev_vulkan_create_logical_device()
{
  const char *deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
  };

  VkDeviceQueueCreateInfo *deviceQueueCreateInfos = NULL;
  unsigned int queueCreateInfoCount = 0;
  VulkanQueueManager.init(VulkanData.physicalDevice, &deviceQueueCreateInfos, &queueCreateInfoCount);

  VkPhysicalDeviceDescriptorIndexingFeaturesEXT physicalDeviceDescriptorIndexingFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,
    .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
    .runtimeDescriptorArray = VK_TRUE,
    .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
    .descriptorBindingPartiallyBound = VK_TRUE,
  };

  VkDeviceCreateInfo deviceCreateInfo =
  {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &physicalDeviceDescriptorIndexingFeatures,
    .enabledExtensionCount = ARRAYSIZE(deviceExtensions),
    .ppEnabledExtensionNames = deviceExtensions,
    .queueCreateInfoCount = queueCreateInfoCount,
    .pQueueCreateInfos = deviceQueueCreateInfos,
  };

  VK_ASSERT(vkCreateDevice(VulkanData.physicalDevice, &deviceCreateInfo, NULL, &VulkanData.logicalDevice));

  VulkanQueueManager.retrieveQueues(VulkanData.logicalDevice, deviceQueueCreateInfos, &queueCreateInfoCount);
}

static void ev_vulkan_createsurface(VkSurfaceKHR *surface)
{
  VK_ASSERT(Window.createVulkanSurface(VulkanData.instance, surface));

  // Check that the surface is supported by the Graphics QueueFamily present

  VkBool32 surfaceSupported = VK_FALSE;
  vkGetPhysicalDeviceSurfaceSupportKHR(VulkanData.physicalDevice, VulkanQueueManager.getFamilyIndex(GRAPHICS), *surface, &surfaceSupported);

  if(surfaceSupported == VK_FALSE)
    assert(!"Surface not supported by physical device!");

}

static void ev_vulkan_createswapchain(unsigned int* imageCount, VkSurfaceKHR* surface, VkSurfaceFormatKHR *surfaceFormat, VkSwapchainKHR* swapchain)
{
  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanData.physicalDevice, *surface, &surfaceCapabilities));

  unsigned int windowWidth  = surfaceCapabilities.currentExtent.width;
  unsigned int windowHeight = surfaceCapabilities.currentExtent.height;

  if(windowWidth == UINT32_MAX || windowHeight == UINT32_MAX)
    Window.getSize(&windowWidth, &windowHeight);

  // The forbidden fruit (don't touch it)
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
    (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
      ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
      : (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
        : (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
          ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
          : VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

  *imageCount = MAX(*imageCount, surfaceCapabilities.minImageCount);

  if(surfaceCapabilities.maxImageCount) // If there is an upper limit
  {
    *imageCount = MIN(*imageCount, surfaceCapabilities.maxImageCount);
  }

  VkSwapchainCreateInfoKHR swapchainCreateInfo =
  {
    .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface          = *surface,
    .minImageCount    = *imageCount,
    .imageFormat      = surfaceFormat->format,
    .imageColorSpace  = surfaceFormat->colorSpace,
    .imageExtent      = {
      .width          = windowWidth,
      .height         = windowHeight,
    },
    .imageArrayLayers = 1,
    .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .preTransform     = surfaceCapabilities.currentTransform,
    .compositeAlpha   = compositeAlpha,
    .presentMode      = VK_PRESENT_MODE_FIFO_KHR, // TODO: Make sure that this is always supported
    .clipped          = VK_TRUE,
    .oldSwapchain     = VK_NULL_HANDLE,
  };

  VK_ASSERT(vkCreateSwapchainKHR(VulkanData.logicalDevice, &swapchainCreateInfo, NULL, swapchain));
}

static void ev_vulkan_init_vma()
{
  VmaAllocatorCreateInfo createInfo = {
    .physicalDevice = VulkanData.physicalDevice,
    .device = VulkanData.logicalDevice,
    .instance = VulkanData.instance,
  };

  vmaCreateAllocator(&createInfo, &VulkanData.allocator);
}

static void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image)
{
  vmaCreateImage(VulkanData.allocator, imageCreateInfo, allocationCreateInfo, &(image->image), &(image->allocation), &(image->allocationInfo));
}

static void ev_vulkan_destroy_image(EvImage *image)
{
  vmaDestroyImage(VulkanData.allocator, image->image, image->allocation);
}

static void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer)
{
  VK_ASSERT(vmaCreateBuffer(VulkanData.allocator, bufferCreateInfo, allocationCreateInfo, &(buffer->buffer), &(buffer->allocation), &(buffer->allocationInfo)));
}

static void ev_vulkan_destroy_buffer(EvBuffer *buffer)
{
  vmaDestroyBuffer(VulkanData.allocator, buffer->buffer, buffer->allocation);
}

inline VkDevice ev_vulkan_get_logical_device()
{
  return VulkanData.logicalDevice;
}

inline VkCommandPool ev_vulkan_get_commandpool(QueueType type)
{
  if(!VulkanData.commandPools[type])
  {
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      // TODO: Is this really what we want to have?
      .flags = (VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      .queueFamilyIndex = VulkanQueueManager.getFamilyIndex(type),
    };
    VK_ASSERT(vkCreateCommandPool(VulkanData.logicalDevice, &commandPoolCreateInfo, NULL, &(VulkanData.commandPools[type])));
  }
  return VulkanData.commandPools[type];
}

static void ev_vulkan_retrieveswapchainimages(VkSwapchainKHR swapchain, unsigned int * imageCount, VkImage ** images)
{
  VK_ASSERT(vkGetSwapchainImagesKHR(VulkanData.logicalDevice, swapchain, imageCount, NULL));

  *images = malloc(sizeof(VkImage) * (*imageCount));
  if(!images)
    assert(!"Couldn't allocate memory for the swapchain images!");

  VK_ASSERT(vkGetSwapchainImagesKHR(VulkanData.logicalDevice, swapchain, imageCount, *images));
}

static void ev_vulkan_createimageviews(unsigned int imageCount, VkFormat imageFormat, VkImage *images, VkImageView **views)
{
  *views = malloc(sizeof(VkImageView) * imageCount);
  for (unsigned int i = 0; i < imageCount; ++i)
  {
    VkImageViewCreateInfo imageViewCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = images[i],
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = imageFormat,
      .components = {0, 0, 0, 0},
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCreateImageView(VulkanData.logicalDevice, &imageViewCreateInfo, NULL, *views + i);
  }
}

static void ev_vulkan_createframebuffer(VkImageView* attachments, unsigned int attachmentCount, VkRenderPass renderPass, VkFramebuffer *framebuffer)
{
  unsigned int windowWidth, windowHeight;
  Window.getSize(&windowWidth, &windowHeight);

  VkFramebufferCreateInfo swapchainFramebufferCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .renderPass = renderPass,
    .attachmentCount = attachmentCount,
    .pAttachments = attachments,
    .width = windowWidth,
    .height = windowHeight,
    .layers = 1,
  };
  VK_ASSERT(vkCreateFramebuffer(VulkanData.logicalDevice, &swapchainFramebufferCreateInfo, NULL, framebuffer));
}

static void ev_vulkan_allocatememorypool(VmaPoolCreateInfo *poolCreateInfo, VmaPool* pool)
{
  VK_ASSERT(vmaCreatePool(VulkanData.allocator, poolCreateInfo, pool));
}

static void ev_vulkan_freememorypool(VmaPool pool)
{
  vmaDestroyPool(VulkanData.allocator, pool);
}

static void ev_vulkan_allocatebufferinpool(VkBufferCreateInfo *bufferCreateInfo, VmaPool pool, EvBuffer *buffer)
{
  VmaAllocationCreateInfo allocationCreateInfo = {
    .pool = pool,
  };

  ev_vulkan_create_buffer(bufferCreateInfo, &allocationCreateInfo, buffer);
}

static void ev_vulkan_allocateimageinpool(VkImageCreateInfo *imageCreateInfo, VmaPool pool, EvImage *image)
{
  VmaAllocationCreateInfo allocationCreateInfo = {
    .pool = pool,
  };

  ev_vulkan_create_image(imageCreateInfo, &allocationCreateInfo, image);
}

inline VmaAllocator ev_vulkan_getallocator()
{
  return VulkanData.allocator;
}

static void ev_vulkan_memorydump()
{
  char* vmaDump;
  vmaBuildStatsString(VulkanData.allocator, &vmaDump, VK_TRUE);

  FILE *dumpFile = fopen("vmadump.json", "w+");
  fputs(vmaDump, dumpFile);
  fclose(dumpFile);

  vmaFreeStatsString(VulkanData.allocator, vmaDump);
}

inline VkPhysicalDevice ev_vulkan_get_physical_device()
{
  return VulkanData.physicalDevice;
}

static void ev_vulkan_destroyswapchain(VkSwapchainKHR swapchain)
{
  vkDestroySwapchainKHR(VulkanData.logicalDevice, swapchain, NULL);
}

static void ev_vulkan_allocateprimarycommandbuffer(QueueType queueType, VkCommandBuffer *cmdBuffer)
{
  VkCommandBufferAllocateInfo cmdBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  cmdBufferAllocateInfo.commandPool = VulkanData.commandPools[queueType];
  cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufferAllocateInfo.commandBufferCount = 1;
  vkAllocateCommandBuffers(VulkanData.logicalDevice, &cmdBufferAllocateInfo, cmdBuffer);
}
