// Evol's Vulkan module header
#include "Vulkan.h"
#include "Window.h"
#include "vulkan_utils.h"
#include "stdlib.h"

static int ev_vulkan_init();
static int ev_vulkan_deinit();

void ev_vulkan_create_instance();
void ev_vulkan_detect_physical_device();
void ev_vulkan_detect_queue_family_indices();
void ev_vulkan_create_logical_device();
void ev_vulkan_init_vma();
void ev_vulkan_create_surface();
void ev_vulkan_create_swapchain(unsigned int imageCount);
void ev_vulkan_create_swapchain_imageviews();
void ev_vulkan_create_swapchain_depthbuffer();
void ev_vulkan_create_swapchain_framebuffers();
void ev_vulkan_allocate_swapchain_commandbuffers();
void ev_vulkan_destroy_swapchain();

void ev_vulkan_create_image(VkImageCreateInfo *imageCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvImage *image);
void ev_vulkan_destroy_image(EvImage *image);

void ev_vulkan_create_buffer(VkBufferCreateInfo *bufferCreateInfo, VmaAllocationCreateInfo *allocationCreateInfo, EvBuffer *buffer);
void ev_vulkan_destroy_buffer(EvBuffer *buffer);

struct ev_Vulkan Vulkan = {
        .init = ev_vulkan_init,
        .deinit = ev_vulkan_deinit,
        .createSwapchain = ev_vulkan_create_swapchain,
        .destroySwapchain = ev_vulkan_destroy_swapchain,
        .createImage = ev_vulkan_create_image,
        .destroyImage = ev_vulkan_destroy_image,
        .createBuffer = ev_vulkan_create_buffer,
        .destroyBuffer = ev_vulkan_destroy_buffer,
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

  VkRenderPass renderPass;

  VkCommandPool commandPools[QUEUE_TYPE_COUNT];
  VkCommandBuffer *swapchainCommandBuffers;

} VulkanData;

static int ev_vulkan_init()
{


  // TODO: Should this be checked?
  VulkanData.depthStencilFormat = VK_FORMAT_D16_UNORM_S8_UINT;




  // Create the vulkan instance
  ev_vulkan_create_instance();

  // Detect the physical device
  ev_vulkan_detect_physical_device();

  // Detect indices of various queue families
  ev_vulkan_detect_queue_family_indices();

  // Create the logical device
  ev_vulkan_create_logical_device();

  // Initialize VMA
  ev_vulkan_init_vma();

  // Create the vulkan surface, then detect the surface's capabilities
  ev_vulkan_create_surface();

  return 0;
}

static int ev_vulkan_deinit()
{
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
    #endif
  };

  char *validation_layers[] = {
		"VK_LAYER_KHRONOS_validation",
  };

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "evol_vulkan",
		.applicationVersion = 0,
		.apiVersion = VulkanData.apiVersion,
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
	vkGetPhysicalDeviceQueueFamilyProperties(VulkanData.physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

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
  Window.createVulkanSurface(VulkanData.instance, &VulkanData.surface);
  VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanData.physicalDevice, VulkanData.surface, &VulkanData.surfaceCapabilities));

  // TODO: Actually detect the format
  {
    VulkanData.surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    VulkanData.surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  }
}

void ev_vulkan_create_swapchain(unsigned int imageCount)
{
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
          
  VulkanData.swapchainImageCount = MIN(MAX(imageCount, VulkanData.surfaceCapabilities.minImageCount), VulkanData.surfaceCapabilities.maxImageCount);
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
  vkGetSwapchainImagesKHR(VulkanData.logicalDevice, VulkanData.swapchain, &VulkanData.swapchainImageCount, VulkanData.swapchainImages);

  ev_vulkan_create_swapchain_imageviews();
  ev_vulkan_create_swapchain_depthbuffer();
  ev_vulkan_create_swapchain_framebuffers();
  ev_vulkan_allocate_swapchain_commandbuffers();
}

void ev_vulkan_create_swapchain_imageviews()
{
  VulkanData.swapchainImageViews = malloc(sizeof(VkImageView) * (VulkanData.swapchainImageCount));
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
  assert(!"Need renderpass and depthbuffer for framebuffers");

  VulkanData.framebuffers = malloc(sizeof(VkFramebuffer) * VulkanData.swapchainImageCount);

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
  if(!VulkanData.commandPools[GRAPHICS])
  {
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      // TODO: Is this really what we want to have?
      .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
      .queueFamilyIndex = VulkanData.queueFamilyIndices[GRAPHICS],
    };
    VK_ASSERT(vkCreateCommandPool(VulkanData.logicalDevice, &commandPoolCreateInfo, NULL, &(VulkanData.commandPools[GRAPHICS])));
  }

  VulkanData.swapchainCommandBuffers = malloc(sizeof(VkCommandBuffer) * VulkanData.swapchainImageCount);

  VkCommandBufferAllocateInfo commandBuffersAllocateInfo =
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    //TODO: Should this be GRAPHICS or PRESENT?
    .commandPool = VulkanData.commandPools[GRAPHICS],
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

  free(VulkanData.swapchainCommandBuffers);
  free(VulkanData.framebuffers);
  free(VulkanData.swapchainImageViews);
  free(VulkanData.swapchainImages);
}

