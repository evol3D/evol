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

void ev_vulkan_create_instance();
void ev_vulkan_detect_physical_device();
void ev_vulkan_create_logical_device();

void ev_vulkan_init_vma();

void ev_vulkan_createsurface(VkSurfaceKHR *surface);
void ev_vulkan_destroysurface(VkSurfaceKHR surface);

void ev_vulkan_createswapchain(unsigned int* imageCount, VkSurfaceKHR* surface, VkSurfaceFormatKHR *surfaceFormat, VkSwapchainKHR* swapchain);
void ev_vulkan_retrieveswapchainimages(VkSwapchainKHR swapchain, unsigned int * imageCount, VkImage ** images);
void ev_vulkan_allocate_swapchain_commandbuffers();
void ev_vulkan_create_semaphores();

void ev_vulkan_createimageviews(unsigned int imageCount, VkFormat imageFormat, VkImage *images, VkImageView **views);
void ev_vulkan_createframebuffer(VkImageView* attachments, unsigned int attachmentCount, VkRenderPass renderPass, VkFramebuffer *framebuffer);

void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image);
void ev_vulkan_destroy_image(EvImage *image);

void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer);
void ev_vulkan_destroy_buffer(EvBuffer *buffer);

VkShaderModule ev_vulkan_load_shader(const char* shaderPath);
void ev_vulkan_unload_shader(VkShaderModule shader);

void ev_vulkan_image_memory_barrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, 
  VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, 
  VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);

//getters
static inline VkDevice ev_vulkan_get_logical_device();
static inline VkRenderPass ev_vulkan_get_renderpass();
static inline VkCommandPool ev_vulkan_get_commandpool(QueueType type);
static inline VkCommandBuffer ev_vulkan_get_current_frame_commandbuffer(void);
static inline unsigned int ev_vulkan_get_queuefamily_index(QueueType type);

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

  .createImageViews             = ev_vulkan_createimageviews,
  .createFramebuffer            =  ev_vulkan_createframebuffer,

  .loadShader                   = ev_vulkan_load_shader,
  .unloadShader                 = ev_vulkan_unload_shader,

  // Getters
  .getDevice                    = ev_vulkan_get_logical_device,
  .getCommandPool               = ev_vulkan_get_commandpool,
};

struct ev_Vulkan_Data {
  VkInstance instance;
  VkApiVersion apiVersion;

  VmaAllocator allocator;

  VkPhysicalDevice physicalDevice;
  VkDevice logicalDevice;

  unsigned int queueFamilyIndices[QUEUE_TYPE_COUNT];

  unsigned int currentFrameIndex;

  VkRenderPass renderPass;

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

void ev_vulkan_destroysurface(VkSurfaceKHR surface)
{
  // Destroy the vulkan surface
  vkDestroySurfaceKHR(VulkanData.instance, surface, NULL);
}

void ev_vulkan_create_instance()
{
  const char *extensions[] = {
    "VK_KHR_surface",
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
    .apiVersion = VK_API_VERSION_1_1,
  };

  VkInstanceCreateInfo instanceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = ARRAYSIZE(validation_layers),
    .ppEnabledLayerNames = validation_layers,
    .enabledExtensionCount = ARRAYSIZE(extensions),
    .ppEnabledExtensionNames = extensions
  };

  VK_ASSERT(vkCreateInstance(&instanceCreateInfo, NULL, &VulkanData.instance));
}

void ev_vulkan_detect_physical_device()
{
  unsigned int physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices( VulkanData.instance, &physicalDeviceCount, NULL);

  if(!physicalDeviceCount)
    assert(!"No physical devices found");

  VkPhysicalDevice *physicalDevices = malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
  ev_log_debug("Malloc'ed %u bytes", physicalDeviceCount * sizeof(VkPhysicalDevice));
  ev_log_debug("Malloc'ed %u bytes", physicalDeviceCount * sizeof(VkPhysicalDevice));
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

void ev_vulkan_create_logical_device()
{
  const char *deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  VkDeviceQueueCreateInfo *deviceQueueCreateInfos = NULL;
  unsigned int queueCreateInfoCount = 0;
  VulkanQueueManager.init(VulkanData.physicalDevice, &deviceQueueCreateInfos, &queueCreateInfoCount);

  VkDeviceCreateInfo deviceCreateInfo = 
  {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .enabledExtensionCount = ARRAYSIZE(deviceExtensions),
    .ppEnabledExtensionNames = deviceExtensions,
    .queueCreateInfoCount = queueCreateInfoCount,
    .pQueueCreateInfos = deviceQueueCreateInfos,
  };

  VK_ASSERT(vkCreateDevice(VulkanData.physicalDevice, &deviceCreateInfo, NULL, &VulkanData.logicalDevice));

  VulkanQueueManager.retrieveQueues(VulkanData.logicalDevice, deviceQueueCreateInfos, &queueCreateInfoCount);
}

void ev_vulkan_createsurface(VkSurfaceKHR *surface)
{
  VK_ASSERT(Window.createVulkanSurface(VulkanData.instance, surface));
}

void ev_vulkan_createswapchain(unsigned int* imageCount, VkSurfaceKHR* surface, VkSurfaceFormatKHR *surfaceFormat, VkSwapchainKHR* swapchain)
{
  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanData.physicalDevice, *surface, &surfaceCapabilities));

  unsigned int windowWidth  = surfaceCapabilities.currentExtent.width;
  unsigned int windowHeight = surfaceCapabilities.currentExtent.height;

  if(windowWidth == UINT32_MAX || windowHeight == UINT32_MAX)
    Window.getSize(&windowWidth, &windowHeight);

/*   VkBool32 surfaceSupported = VK_FALSE; */
/*   vkGetPhysicalDeviceSurfaceSupportKHR( */
/*     VulkanData.physicalDevice, VulkanData.queueFamilyIndices[GRAPHICS], */ 
/*     *surface, &surfaceSupported); */   

/*   if(surfaceSupported == VK_FALSE) */
/*     assert(!"Surface not supported by physical device!"); */

  
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

void ev_vulkan_init_vma()
{
  VmaAllocatorCreateInfo createInfo = {
    .physicalDevice = VulkanData.physicalDevice,
    .device = VulkanData.logicalDevice,
    .instance = VulkanData.instance,
  };

  vmaCreateAllocator(&createInfo, &VulkanData.allocator);
}

void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image)
{
  vmaCreateImage(VulkanData.allocator, imageCreateInfo, allocationCreateInfo, &(image->image), &(image->allocation), &(image->allocationInfo));
}

void ev_vulkan_destroy_image(EvImage *image)
{
  vmaDestroyImage(VulkanData.allocator, image->image, image->allocation);
}

void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer)
{
  vmaCreateBuffer(VulkanData.allocator, bufferCreateInfo, allocationCreateInfo, &(buffer->buffer), &(buffer->allocation), &(buffer->allocationInfo));
}

void ev_vulkan_destroy_buffer(EvBuffer *buffer)
{
  vmaDestroyBuffer(VulkanData.allocator, buffer->buffer, buffer->allocation);
}

static inline VkDevice ev_vulkan_get_logical_device()
{
  return VulkanData.logicalDevice;
}

VkShaderModule ev_vulkan_load_shader(const char* shaderPath)
{
  FILE* file = fopen(shaderPath, "rb");
  if(!file) return VK_NULL_HANDLE;

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *shaderCode = malloc(length);
  ev_log_debug("Malloc'ed %u bytes", sizeof(char) * length);

  fread(shaderCode, 1, length, file);
  fclose(file);

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = length,
    .pCode = (const unsigned int *)shaderCode,
  };

  VkShaderModule shaderModule;
  VK_ASSERT(vkCreateShaderModule(VulkanData.logicalDevice, &shaderModuleCreateInfo, NULL, &shaderModule));

  free(shaderCode);
  ev_log_debug("Free'ed %u bytes", sizeof(char) * length);
  return shaderModule;
}

void ev_vulkan_unload_shader(VkShaderModule shader)
{
  vkDestroyShaderModule(VulkanData.logicalDevice, shader, NULL);
}

static inline VkCommandPool ev_vulkan_get_commandpool(QueueType type)
{
  if(!VulkanData.commandPools[type])
  {
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      // TODO: Is this really what we want to have?
      .flags = (VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
      .queueFamilyIndex = VulkanData.queueFamilyIndices[type],
    };
    VK_ASSERT(vkCreateCommandPool(VulkanData.logicalDevice, &commandPoolCreateInfo, NULL, &(VulkanData.commandPools[type])));
  }
  return VulkanData.commandPools[type];
}

/* void ev_vulkan_image_memory_barrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags) */
/* { */
/*   VkImageMemoryBarrier imageMemoryBarrier = */ 
/*   { */
/*     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, */
/*     .pNext = NULL, */
/*     .srcAccessMask = srcAccessMask, */
/*     .dstAccessMask = dstAccessMask, */
/*     .oldLayout = oldLayout, */
/*     .newLayout = newLayout, */
/*     .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // TODO */
/*     .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // TODO */
/*     .image = image, */
/*     .subresourceRange = */ 
/*     { */
/*       .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, */
/*       .levelCount = VK_REMAINING_MIP_LEVELS, */
/*       .layerCount = VK_REMAINING_ARRAY_LAYERS, */
/*     } */
/*   }; */

/*   vkCmdPipelineBarrier(Vulkan.getCurrentFrameCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, 0, NULL, 0, NULL, 1, &imageMemoryBarrier); */
/* } */

void ev_vulkan_retrieveswapchainimages(VkSwapchainKHR swapchain, unsigned int * imageCount, VkImage ** images)
{
  VK_ASSERT(vkGetSwapchainImagesKHR(VulkanData.logicalDevice, swapchain, imageCount, NULL));

  *images = malloc(sizeof(VkImage) * (*imageCount));
  if(!images)
    assert(!"Couldn't allocate memory for the swapchain images!");

  VK_ASSERT(vkGetSwapchainImagesKHR(VulkanData.logicalDevice, swapchain, imageCount, *images));
}

void ev_vulkan_createimageviews(unsigned int imageCount, VkFormat imageFormat, VkImage *images, VkImageView **views)
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

void ev_vulkan_createframebuffer(VkImageView* attachments, unsigned int attachmentCount, VkRenderPass renderPass, VkFramebuffer *framebuffer)
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
