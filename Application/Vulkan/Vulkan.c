//TODO Comments / Logging
// Evol's Vulkan module header
#include "Vulkan.h"
#include "Window.h"
#include "vulkan_utils.h"
#include <ev_log/ev_log.h>

#include "stdio.h"
#include "stdlib.h"

static int ev_vulkan_init();
static int ev_vulkan_deinit();

void ev_vulkan_create_instance();
void ev_vulkan_detect_physical_device();
void ev_vulkan_detect_queue_family_indices();
void ev_vulkan_create_logical_device();

void ev_vulkan_init_vma();

void ev_vulkan_create_surface();

void ev_vulkan_create_renderpass();

void ev_vulkan_create_swapchain(unsigned int *imageCount);
void ev_vulkan_create_swapchain_imageviews();
void ev_vulkan_create_swapchain_depthbuffer();
void ev_vulkan_create_swapchain_framebuffers();
void ev_vulkan_allocate_swapchain_commandbuffers();
void ev_vulkan_create_semaphores();
void ev_vulkan_destroy_swapchain();

void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image);
void ev_vulkan_destroy_image(EvImage *image);

void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer);
void ev_vulkan_destroy_buffer(EvBuffer *buffer);

VkShaderModule ev_vulkan_load_shader(const char* shaderPath);
void ev_vulkan_unload_shader(VkShaderModule shader);

void ev_vulkan_start_new_frame(void);
void ev_vulkan_end_frame(void);

void ev_vulkan_image_memory_barrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, 
  VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, 
  VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);

//getters
static inline VkDevice ev_vulkan_get_logical_device();
static inline VkRenderPass ev_vulkan_get_renderpass();
static inline unsigned int ev_vulkan_get_commandpool(QueueType type);
static inline VkCommandBuffer ev_vulkan_get_current_frame_commandbuffer(void);
static inline unsigned int ev_vulkan_get_queuefamily_index(QueueType type);

struct ev_Vulkan Vulkan = {
  .init = ev_vulkan_init,
  .deinit = ev_vulkan_deinit,

  .createSwapchain = ev_vulkan_create_swapchain,
  .destroySwapchain = ev_vulkan_destroy_swapchain,
  .createImage = ev_vulkan_create_image,
  .destroyImage = ev_vulkan_destroy_image,

  .createBuffer = ev_vulkan_create_buffer,
  .destroyBuffer = ev_vulkan_destroy_buffer,

  .loadShader = ev_vulkan_load_shader,
  .unloadShader = ev_vulkan_unload_shader,

  .startNewFrame = ev_vulkan_start_new_frame,
  .endFrame = ev_vulkan_end_frame,

  //getters
  .getDevice = ev_vulkan_get_logical_device,
  .getRenderPass = ev_vulkan_get_renderpass,
  .getCommandPool = ev_vulkan_get_commandpool,
  .getQueueFamilyIndex = ev_vulkan_get_queuefamily_index,
  .getCurrentFrameCommandBuffer = ev_vulkan_get_current_frame_commandbuffer,
};

struct ev_Vulkan_Data {
  VkInstance instance;
  VkApiVersion apiVersion;

  VmaAllocator allocator;

  VkPhysicalDevice physicalDevice;
  VkDevice logicalDevice;

  unsigned int queueFamilyIndices[QUEUE_TYPE_COUNT];

  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  VkSurfaceFormatKHR surfaceFormat;
  VkFormat depthStencilFormat;

  unsigned int swapchainImageCount;
  VkSwapchainKHR swapchain;
  VkImage *swapchainImages;
  VkImageView *swapchainImageViews;
  VkFramebuffer *framebuffers;
  EvImage depthBufferImage;
  VkImageView depthBufferImageView;

  unsigned int currentFrameIndex;

  VkRenderPass renderPass;

  VkCommandPool commandPools[QUEUE_TYPE_COUNT];
  VkCommandBuffer *swapchainCommandBuffers;

  VkSemaphore imageSemaphore;
  VkSemaphore signalSemaphore;

  VkQueue queues[QUEUE_TYPE_COUNT];

} VulkanData;

static int ev_vulkan_init()
{
  // TODO: Should this be checked?
  VulkanData.depthStencilFormat = VK_FORMAT_D16_UNORM_S8_UINT;

  // Zeroing up commandpools to identify the ones that get initialized
  for(int i = 0; i < QUEUE_TYPE_COUNT; ++i)
  {
    VulkanData.commandPools[i] = 0;
  }




  // Create the vulkan instance
  ev_vulkan_create_instance();

  // Detect the physical device
  ev_vulkan_detect_physical_device();

  // Detect indices of various queue families
  ev_vulkan_detect_queue_family_indices();

  // Create the logical device
  ev_vulkan_create_logical_device();

  // TODO: Remove
  vkGetDeviceQueue(VulkanData.logicalDevice, VulkanData.queueFamilyIndices[GRAPHICS], 0, &VulkanData.queues[GRAPHICS]);

  // Initialize VMA
  ev_vulkan_init_vma();

  // Create the vulkan surface, then detect the surface's capabilities
  ev_vulkan_create_surface();
  
  return 0;
}

static int ev_vulkan_deinit()
{
  // Destroy any commandpool that was created earlier
  for(int i = 0; i < QUEUE_TYPE_COUNT; ++i)
    if(VulkanData.commandPools[i])
      vkDestroyCommandPool(VulkanData.logicalDevice, VulkanData.commandPools[i], NULL);

  // Destroy the vulkan surface
  vkDestroySurfaceKHR(VulkanData.instance, VulkanData.surface, NULL);

  // Destroy VMA
  vmaDestroyAllocator(VulkanData.allocator);

  // Destroy the logical device
  vkDestroyDevice(VulkanData.logicalDevice, NULL);

  // Destroy the vulkan instance
  vkDestroyInstance(VulkanData.instance, NULL);

  return 0;
}

void ev_vulkan_create_instance()
{
  char *extensions[] = {
    "VK_KHR_surface",
    #ifdef _WIN32
    "VK_KHR_win32_surface",
    #else
    "VK_KHR_xcb_surface",
    #endif
  };

  char *validation_layers[] = {
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

  float dummyPriority = 1.0;
  VkDeviceQueueCreateInfo deviceQueueCreateInfos[] =
  {
    {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = VulkanData.queueFamilyIndices[GRAPHICS],
			.queueCount = 1,
			.pQueuePriorities = &dummyPriority,
		},
  };

  VkDeviceCreateInfo deviceCreateInfo = 
  {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .enabledExtensionCount = ARRAYSIZE(deviceExtensions),
    .ppEnabledExtensionNames = deviceExtensions,
    .queueCreateInfoCount = ARRAYSIZE(deviceQueueCreateInfos),
    .pQueueCreateInfos = deviceQueueCreateInfos,
  };

  VK_ASSERT(vkCreateDevice(VulkanData.physicalDevice, &deviceCreateInfo, NULL, &VulkanData.logicalDevice));
}

void ev_vulkan_detect_queue_family_indices()
{
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanData.physicalDevice, &queueFamilyPropertiesCount, NULL);
	VkQueueFamilyProperties *queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertiesCount);
  ev_log_debug("Malloc'ed %u bytes",queueFamilyPropertiesCount * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanData.physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

	for (int i = 0; i < queueFamilyPropertiesCount; ++i)
  {
    printf("Queue Family index: %d\n", i);
    printf("Queue Family Flags: %d\n", queueFamilyProperties[i].queueFlags);
    printf("Queue Family queue count: %d\n", queueFamilyProperties[i].queueCount);
  }
	for (int i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
		{
			VulkanData.queueFamilyIndices[GRAPHICS] = i;
			VulkanData.queueFamilyIndices[TRANSFER] = i;
			VulkanData.queueFamilyIndices[COMPUTE] = i;
			VulkanData.queueFamilyIndices[SPARSE_BINDING] = i;
			free(queueFamilyProperties);
			return;
		}
	}
  free(queueFamilyProperties);
	assert(!"Looks like you'll need to implement a better way for queue families now :D\n");
}

void ev_vulkan_create_surface()
{
  VK_ASSERT(Window.createVulkanSurface(VulkanData.instance, &VulkanData.surface));

  VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanData.physicalDevice, VulkanData.surface, &VulkanData.surfaceCapabilities));

  // TODO: Actually detect the format
  {
    VulkanData.surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    VulkanData.surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  }
}

void ev_vulkan_create_swapchain(unsigned int *imageCount)
{
  ev_vulkan_create_renderpass();

  unsigned int windowWidth = VulkanData.surfaceCapabilities.currentExtent.width;
  unsigned int windowHeight = VulkanData.surfaceCapabilities.currentExtent.height;

  if(windowWidth == UINT32_MAX || windowHeight == UINT32_MAX)
    Window.getSize(&windowWidth, &windowHeight);

  VkBool32 surfaceSupported = VK_FALSE;
  vkGetPhysicalDeviceSurfaceSupportKHR(
    VulkanData.physicalDevice, VulkanData.queueFamilyIndices[GRAPHICS], 
    VulkanData.surface, &surfaceSupported);   

  if(surfaceSupported == VK_FALSE)
    assert(!"Surface not supported by physical device!");

  
  // The forbidden fruit (don't touch it)
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
		(VulkanData.surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
      ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
      : (VulkanData.surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
        : (VulkanData.surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
          ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
          : VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
          
  VulkanData.swapchainImageCount = MAX(*imageCount, VulkanData.surfaceCapabilities.minImageCount);

  if(VulkanData.surfaceCapabilities.maxImageCount) // If there is an upper limit
  {
    VulkanData.swapchainImageCount = MIN(VulkanData.swapchainImageCount, VulkanData.surfaceCapabilities.maxImageCount);
  }

  // Set the passed variable to the agreed on image count
  *imageCount = VulkanData.swapchainImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo =
  {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = VulkanData.surface,
    .minImageCount = VulkanData.swapchainImageCount,
    .imageFormat = VulkanData.surfaceFormat.format,
    .imageColorSpace = VulkanData.surfaceFormat.colorSpace,
    .imageExtent = {
      .width = windowWidth,
      .height = windowHeight,
    },
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .preTransform = VulkanData.surfaceCapabilities.currentTransform,
    .compositeAlpha = compositeAlpha,
    .presentMode = VK_PRESENT_MODE_FIFO_KHR, // TODO: Make sure that this is always supported
    .clipped = VK_TRUE,
    .oldSwapchain = VK_NULL_HANDLE,
  };

  VK_ASSERT(vkCreateSwapchainKHR(VulkanData.logicalDevice, &swapchainCreateInfo, NULL, &VulkanData.swapchain));

  vkGetSwapchainImagesKHR(VulkanData.logicalDevice, VulkanData.swapchain, &VulkanData.swapchainImageCount, NULL);
  VulkanData.swapchainImages = malloc(sizeof(VkImage) * VulkanData.swapchainImageCount);
  ev_log_debug("Malloc'ed %u bytes", sizeof(VkImage) * VulkanData.swapchainImageCount);
  vkGetSwapchainImagesKHR(VulkanData.logicalDevice, VulkanData.swapchain, &VulkanData.swapchainImageCount, VulkanData.swapchainImages);

  ev_vulkan_create_swapchain_imageviews();
  ev_vulkan_create_swapchain_depthbuffer();
  ev_vulkan_create_swapchain_framebuffers();
  ev_vulkan_allocate_swapchain_commandbuffers();
  ev_vulkan_create_semaphores();
}

void ev_vulkan_create_semaphores()
{
  VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VK_ASSERT(vkCreateSemaphore(VulkanData.logicalDevice, &semaphoreCreateInfo, NULL, &VulkanData.imageSemaphore));
  VK_ASSERT(vkCreateSemaphore(VulkanData.logicalDevice, &semaphoreCreateInfo, NULL, &VulkanData.signalSemaphore));
}

void ev_vulkan_create_swapchain_imageviews()
{
  VulkanData.swapchainImageViews = malloc(sizeof(VkImageView) * (VulkanData.swapchainImageCount));
  ev_log_debug("Malloc'ed %u bytes", sizeof(VkImageView) * (VulkanData.swapchainImageCount));
	for (unsigned int i = 0; i < VulkanData.swapchainImageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = VulkanData.swapchainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VulkanData.surfaceFormat.format,
			.components = {0, 0, 0, 0},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		vkCreateImageView(VulkanData.logicalDevice, &imageViewCreateInfo, NULL, VulkanData.swapchainImageViews + i);
	}
}

void ev_vulkan_create_swapchain_depthbuffer()
{
  unsigned int width, height;
  Window.getSize(&width, &height);

  VkImageCreateInfo depthImageCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = VulkanData.depthStencilFormat,
    .extent = {
      .width = width,
      .height = height,
      .depth = 1,
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  VmaAllocationCreateInfo vmaAllocationCreateInfo = {
    .usage = VMA_MEMORY_USAGE_GPU_ONLY,
  };

  Vulkan.createImage(&depthImageCreateInfo, &vmaAllocationCreateInfo, &(VulkanData.depthBufferImage));

  // Creating the imageview
  VkImageViewCreateInfo depthImageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = VulkanData.depthBufferImage.image,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = VulkanData.depthStencilFormat,
    .components = {0, 0, 0, 0},
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    }
  };

  VK_ASSERT(vkCreateImageView(VulkanData.logicalDevice, &depthImageViewCreateInfo, NULL, &VulkanData.depthBufferImageView));
}

void ev_vulkan_create_swapchain_framebuffers()
{
  VulkanData.framebuffers = malloc(sizeof(VkFramebuffer) * VulkanData.swapchainImageCount);
  ev_log_debug("Malloc'ed %u bytes", sizeof(VkFramebuffer) * VulkanData.swapchainImageCount);

  unsigned int windowWidth, windowHeight;
  Window.getSize(&windowWidth, &windowHeight);

  for (unsigned int i = 0; i < VulkanData.swapchainImageCount; ++i)
  {
    VkImageView attachments[] = 
    {
      VulkanData.swapchainImageViews[i],
      VulkanData.depthBufferImageView,
    };

    VkFramebufferCreateInfo swapchainFramebufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = VulkanData.renderPass,
      .attachmentCount = ARRAYSIZE(attachments),
      .pAttachments = attachments,
      .width = windowWidth,
      .height = windowHeight,
      .layers = 1,
    };
    vkCreateFramebuffer(VulkanData.logicalDevice, &swapchainFramebufferCreateInfo, NULL, VulkanData.framebuffers + i);
  }
}

void ev_vulkan_allocate_swapchain_commandbuffers()
{
  VkCommandPool gPool = Vulkan.getCommandPool(GRAPHICS);

  VulkanData.swapchainCommandBuffers = malloc(sizeof(VkCommandBuffer) * VulkanData.swapchainImageCount);
  ev_log_debug("Malloc'ed %u bytes", sizeof(VkCommandBuffer) * VulkanData.swapchainImageCount);

  VkCommandBufferAllocateInfo commandBuffersAllocateInfo =
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    //TODO: Should this be GRAPHICS or PRESENT?
    .commandPool = gPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = VulkanData.swapchainImageCount
  };

  VK_ASSERT(vkAllocateCommandBuffers(VulkanData.logicalDevice, &commandBuffersAllocateInfo, VulkanData.swapchainCommandBuffers));
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

void ev_vulkan_destroy_swapchain()
{
  // Destroy semaphores
  vkDestroySemaphore(VulkanData.logicalDevice, VulkanData.imageSemaphore, NULL);
  vkDestroySemaphore(VulkanData.logicalDevice, VulkanData.signalSemaphore, NULL);


  // Free frame commandbuffers
  vkFreeCommandBuffers(VulkanData.logicalDevice, VulkanData.commandPools[GRAPHICS], VulkanData.swapchainImageCount, VulkanData.swapchainCommandBuffers);

  // Destroy depth buffer
  vkDestroyImageView(VulkanData.logicalDevice, VulkanData.depthBufferImageView, NULL);
  Vulkan.destroyImage(&VulkanData.depthBufferImage);

  for(unsigned int i = 0; i < VulkanData.swapchainImageCount; ++i)
  {
    // Destroy Framebuffers
    vkDestroyFramebuffer(VulkanData.logicalDevice, VulkanData.framebuffers[i], NULL);
    
    // Destroy swapchain imageviews
    vkDestroyImageView(VulkanData.logicalDevice, VulkanData.swapchainImageViews[i], NULL);
  }

  vkDestroySwapchainKHR(VulkanData.logicalDevice, VulkanData.swapchain, NULL);

  // Destroying the renderpass
  vkDestroyRenderPass(VulkanData.logicalDevice, VulkanData.renderPass, NULL);

  free(VulkanData.swapchainCommandBuffers);
  free(VulkanData.framebuffers);
  free(VulkanData.swapchainImageViews);
  free(VulkanData.swapchainImages);
}

static inline VkDevice ev_vulkan_get_logical_device()
{
  return VulkanData.logicalDevice;
}
static inline VkRenderPass ev_vulkan_get_renderpass()
{
  return VulkanData.renderPass;
}

void ev_vulkan_create_renderpass()
{
  VkAttachmentDescription attachmentDescriptions[] = 
	{
		{
			.format = VulkanData.surfaceFormat.format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		},
		{
			.format = VulkanData.depthStencilFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		}
	};

	VkAttachmentReference colorAttachmentReferences[] =
	{
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		},
	};

	VkAttachmentReference depthStencilAttachmentReference =
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkSubpassDescription subpassDescriptions[] =
	{
		{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = NULL,
			.colorAttachmentCount = ARRAYSIZE(colorAttachmentReferences),
			.pColorAttachments = colorAttachmentReferences,
			.pResolveAttachments = NULL,
			.pDepthStencilAttachment = &depthStencilAttachmentReference,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = NULL,
		},
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = ARRAYSIZE(attachmentDescriptions),
		.pAttachments = attachmentDescriptions,
		.subpassCount = ARRAYSIZE(subpassDescriptions),
		.pSubpasses = subpassDescriptions,
		.dependencyCount = 0,
		.pDependencies = NULL,
	};

  VK_ASSERT(vkCreateRenderPass(VulkanData.logicalDevice, &renderPassCreateInfo, NULL, &VulkanData.renderPass));
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

static inline unsigned int ev_vulkan_get_queuefamily_index(QueueType type)
{
  return VulkanData.queueFamilyIndices[type];
}

static inline unsigned int ev_vulkan_get_commandpool(QueueType type)
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

void ev_vulkan_start_new_frame(void)
{
  VK_ASSERT(vkAcquireNextImageKHR(VulkanData.logicalDevice, VulkanData.swapchain, ~0ull, VulkanData.imageSemaphore, NULL, &(VulkanData.currentFrameIndex)));

  //start recording into the right command buffer
  {
    VkCommandBufferBeginInfo commandBufferBeginInfo = { 
		  .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		  .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	  };
	  VK_ASSERT(vkBeginCommandBuffer(VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex], &commandBufferBeginInfo));
  }

  ev_vulkan_image_memory_barrier(VulkanData.swapchainImages[VulkanData.currentFrameIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, NULL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT);

  unsigned int width, height;
  Window.getSize(&width, &height);

  //strarting render pass
  {
    VkClearValue clearValues[] = {
		  {
			  .color = {0.33f, 0.22f, 0.37f, 1.f},
		  },
	  	{
		  	.depthStencil = {1.0f, 0.0f},
		  }
	  };


	  VkRenderPassBeginInfo renderPassBeginInfo = 
    {
      .sType =  VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = VulkanData.renderPass,
      .framebuffer = VulkanData.framebuffers[VulkanData.currentFrameIndex],
      .renderArea.extent.width = width,
      .renderArea.extent.height = height,
      .clearValueCount = ARRAYSIZE(clearValues),
      .pClearValues = clearValues,
    };
	  vkCmdBeginRenderPass(Vulkan.getCurrentFrameCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); 
  }

  //not sure if this should be set every time
  {
    VkViewport viewport = 
    {
			.x = 0,
			.y = height,
			.width = width,
			.height = -(float)height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		VkRect2D scissor = {
			.offset = {0, 0},
			.extent = {width, height},
		};

		vkCmdSetViewport(VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex], 0, 1, &viewport);
		vkCmdSetScissor(VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex], 0, 1, &scissor);
  }

  //I think here we should handle stuff to evol renderer
}

void ev_vulkan_end_frame(void)
{
  vkCmdEndRenderPass(Vulkan.getCurrentFrameCommandBuffer());

  ev_vulkan_image_memory_barrier(VulkanData.swapchainImages[VulkanData.currentFrameIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, NULL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT);

  VK_ASSERT(vkEndCommandBuffer(Vulkan.getCurrentFrameCommandBuffer()));

  VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.pWaitDstStageMask = &stageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex];
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &VulkanData.imageSemaphore;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &VulkanData.signalSemaphore;

  VK_ASSERT(vkQueueSubmit(VulkanData.queues[GRAPHICS], 1, &submitInfo, VK_NULL_HANDLE));

  VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &VulkanData.swapchain;
  presentInfo.pImageIndices = &VulkanData.currentFrameIndex;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &VulkanData.signalSemaphore;

  VK_ASSERT(vkQueuePresentKHR(VulkanData.queues[GRAPHICS], &presentInfo));

  //assert(!"Not implemented");
}

static inline VkCommandBuffer ev_vulkan_get_current_frame_commandbuffer(void)
{
  // TODO: Do we need to reset the command buffer?
  // TODO: Does it overwrite existing memory if not reset? If so, then resetting it
  // will only introduce the overhead of freeing memory just to allocate it again.
  // 
  // Code:
  //  vkResetCommandBuffer(VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  //
  
  return VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex];
}

void ev_vulkan_image_memory_barrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags)
{
	VkImageMemoryBarrier imageMemoryBarrier = 
  {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,	// TODO
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,	// TODO
		.image = image,
		.subresourceRange = 
    {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.layerCount = VK_REMAINING_ARRAY_LAYERS,
		}
	};

	vkCmdPipelineBarrier(Vulkan.getCurrentFrameCommandBuffer(), srcStageMask, dstStageMask, dependencyFlags, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
}
