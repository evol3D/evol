#include <Renderer/RendererBackend.h>
#include <Window.h>
#include <Vulkan.h>
#include <string.h>
#include <stdlib.h>


static int ev_rendererbackend_init();
static int ev_rendererbackend_deinit();
static void ev_rendererbackend_startnewframe();
static void ev_rendererbackend_endframe();

static int ev_rendererbackend_loadbasepipelines();

static int ev_rendererbackend_loadbaseshaders();

static int ev_rendererbackend_loadbasedescriptorsetlayouts();

ShaderModule ev_rendererbackend_loadshader(const char* shaderPath);
void ev_rendererbackend_unloadshader(ShaderModule shader);


static int ev_rendererbackend_bindpipeline(GraphicsPipelineType type);

static int ev_rendererbackend_binddescriptorsets(DescriptorSet *descriptorSets, unsigned int count);
static int ev_rendererbackend_allocatedescriptorset(DescriptorSetLayoutType setLayoutType, DescriptorSet *descriptorSet);
static int ev_rendererbackend_pushdescriptorstoset(DescriptorSet descriptorSet, Descriptor *descriptors, unsigned int descriptorsCount);


static void ev_rendererbackend_createresourcememorypool(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool);
static void ev_rendererbackend_allocatebufferinpool(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer);

static void ev_rendererbackend_allocatestagingbuffer(unsigned long long bufferSize, MemoryBuffer *buffer);
static void ev_rendererbackend_updatestagingbuffer(MemoryBuffer *buffer, unsigned long long bufferSize, const void *data);

static void ev_rendererbackend_copybuffer(unsigned long long size, MemoryBuffer *src, MemoryBuffer *dst);

static void ev_rendererbackend_memorydump();

// Disgrace functions
static VkCommandBuffer ev_rendererbackend_getcurrentframecommandbuffer();
static VkRenderPass ev_rendererbackend_getrenderpass();

struct ev_RendererBackend RendererBackend = 
{
  .init = ev_rendererbackend_init,
  .deinit = ev_rendererbackend_deinit,

  .startNewFrame = ev_rendererbackend_startnewframe,
  .endFrame = ev_rendererbackend_endframe,

  .loadBasePipelines = ev_rendererbackend_loadbasepipelines,

  .loadBaseShaders = ev_rendererbackend_loadbaseshaders,

  .loadBaseDescriptorSetLayouts = ev_rendererbackend_loadbasedescriptorsetlayouts,

  .bindPipeline = ev_rendererbackend_bindpipeline,

  .bindDescriptorSets = ev_rendererbackend_binddescriptorsets,
  .allocateDescriptorSet = ev_rendererbackend_allocatedescriptorset,
  .pushDescriptorsToSet = ev_rendererbackend_pushdescriptorstoset,

  .loadShader = ev_rendererbackend_loadshader,
  .unloadShader = ev_rendererbackend_unloadshader,

  .createResourceMemoryPool = ev_rendererbackend_createresourcememorypool,
  .allocateBufferInPool = ev_rendererbackend_allocatebufferinpool,
  
  .allocateStagingBuffer = ev_rendererbackend_allocatestagingbuffer,
  .updateStagingBuffer = ev_rendererbackend_updatestagingbuffer,

  .copyBuffer = ev_rendererbackend_copybuffer,


  .memoryDump = ev_rendererbackend_memorydump,

  // TODO Remove
  .getCurrentFrameCommandBuffer = ev_rendererbackend_getcurrentframecommandbuffer,
  .getRenderPass = ev_rendererbackend_getrenderpass,
};


struct ev_RendererBackendData
{
  VkSurfaceKHR surface;
  VkSurfaceFormatKHR surfaceFormat;
  VkFormat depthStencilFormat;

  unsigned int swapchainImageCount;
  VkSwapchainKHR swapchain;
  VkImage *swapchainImages;
  VkImageView *swapchainImageViews;
  VkFramebuffer *framebuffers;
  EvImage depthBufferImage;
  VkImageView depthBufferImageView;
  VkCommandBuffer *swapchainCommandBuffers;

  VkCommandPool transferCommandPool;

  unsigned int currentFrameIndex;

  VkRenderPass renderPass;

  VkSemaphore *semaphores;

  const char * baseShadersPaths[EV_BASE_SHADER_COUNT];
  ShaderModule baseShaders[EV_BASE_SHADER_COUNT];

  VkDescriptorPool descriptorPool;
  DescriptorSetLayout baseDescriptorSetLayouts[DESCRIPTOR_SET_LAYOUT_COUNT];

  GraphicsPipeline baseGraphicsPipelines[GRAPHICS_PIPELINES_COUNT];
  VkPipelineLayout baseGraphicsPipelinesLayouts[GRAPHICS_PIPELINES_COUNT];

  GraphicsPipelineType boundPipeline;

} RendererBackendData;

#define SET_SHADER(id, path) RendererBackendData.baseShadersPaths[id] = path;

typedef enum
{
  IMAGE_SEMAPHORE,
  SUBMISSION_SEMAPHORE,

  SEMAPHORE_COUNT,
} SemaphoreName;

void ev_rendererbackend_createdepthbuffer();
void ev_rendererbackend_createrenderpass();

#define DATA(x) RendererBackendData.x
#define CMDBUFFERS RendererBackendData.swapchainCommandBuffers
#define FRAME RendererBackendData.currentFrameIndex
#define FRAMEBUFFERS RendererBackendData.framebuffers
#define SEMAPHORES RendererBackendData.semaphores

#define BASE_SHADERS_PATHS RendererBackendData.baseShadersPaths
#define BASE_SHADERS RendererBackendData.baseShaders

#define BASE_PIPELINES RendererBackendData.baseGraphicsPipelines
#define BASE_PIPELINES_LAYOUTS RendererBackendData.baseGraphicsPipelinesLayouts

#define BASE_DESCRIPTOR_SET_LAYOUTS RendererBackendData.baseDescriptorSetLayouts

#define BOUND_PIPELINE RendererBackendData.baseGraphicsPipelines[RendererBackendData.boundPipeline]
#define BOUND_PIPELINE_LAYOUT RendererBackendData.baseGraphicsPipelinesLayouts[RendererBackendData.boundPipeline]

static int ev_rendererbackend_init()
{
  // TODO: Should this be checked?
  DATA(depthStencilFormat) = VK_FORMAT_D16_UNORM_S8_UINT;

  Vulkan.createSurface(&DATA(surface));

  // TODO: Actually detect the format
  {
    DATA(surfaceFormat).format = VK_FORMAT_B8G8R8A8_UNORM;
    DATA(surfaceFormat).colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  }

  DATA(swapchainImageCount) = 3;
  Vulkan.createSwapchain(
    &DATA(swapchainImageCount), 
    &DATA(surface), 
    &DATA(surfaceFormat), 
    &DATA(swapchain)
  );

  Vulkan.retrieveSwapchainImages(
    DATA(swapchain), 
    &DATA(swapchainImageCount), 
    &DATA(swapchainImages)
  );

  Vulkan.createImageViews(
    DATA(swapchainImageCount),
    DATA(surfaceFormat).format,
    DATA(swapchainImages),
    &DATA(swapchainImageViews)
  );

  ev_rendererbackend_createdepthbuffer();
  ev_rendererbackend_createrenderpass();

  DATA(framebuffers) = malloc(sizeof(VkFramebuffer) * DATA(swapchainImageCount));

  for(int i = 0; i < DATA(swapchainImageCount); ++i)
  {
    VkImageView attachments[] =
    {
      DATA(swapchainImageViews)[i],
      DATA(depthBufferImageView)
    };

    Vulkan.createFramebuffer(
        attachments,
        ARRAYSIZE(attachments),
        DATA(renderPass),
        DATA(framebuffers) + i
    );
  }

  // Creating a command buffer for each swapchain image
  VkCommandPool graphicsPool = Vulkan.getCommandPool(GRAPHICS);
  DATA(swapchainCommandBuffers) = malloc(sizeof(VkCommandBuffer) * DATA(swapchainImageCount));

  VkCommandBufferAllocateInfo commandBuffersAllocateInfo = 
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = graphicsPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = DATA(swapchainImageCount),
  };

  VK_ASSERT(vkAllocateCommandBuffers(Vulkan.getDevice(), &commandBuffersAllocateInfo, DATA(swapchainCommandBuffers)));

  // Creating all semaphores needed
  VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  SEMAPHORES = malloc(sizeof(VkSemaphore) * SEMAPHORE_COUNT);
  for(unsigned int i = 0; i < SEMAPHORE_COUNT; ++i)
  {
    VK_ASSERT(vkCreateSemaphore(Vulkan.getDevice(), &semaphoreCreateInfo, NULL, SEMAPHORES + i));
  }

  // Creating a commandpool for transfer operations
  VkCommandPoolCreateInfo transferCommandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  transferCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  transferCommandPoolCreateInfo.queueFamilyIndex = VulkanQueueManager.getFamilyIndex(TRANSFER);

  vkCreateCommandPool(Vulkan.getDevice(), &transferCommandPoolCreateInfo, NULL, &DATA(transferCommandPool));

  VkDescriptorPoolSize poolSizes[] = 
  {
    {
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: Measure performance difference of using VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER vs VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
      .descriptorCount = 1,
    },
  };

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  descriptorPoolCreateInfo.maxSets = 1;
  descriptorPoolCreateInfo.poolSizeCount = ARRAYSIZE(poolSizes);
  descriptorPoolCreateInfo.pPoolSizes = poolSizes;

  VK_ASSERT(vkCreateDescriptorPool(Vulkan.getDevice(), &descriptorPoolCreateInfo, NULL, &DATA(descriptorPool)));

  return 0;
}

static int ev_rendererbackend_deinit()
{
  Vulkan.destroySurface(RendererBackendData.surface);
  return 0;
}

void ev_rendererbackend_createdepthbuffer()
{
  unsigned int width, height;
  Window.getSize(&width, &height);

  VkImageCreateInfo depthImageCreateInfo = {
    .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType     = VK_IMAGE_TYPE_2D,
    .format        = DATA(depthStencilFormat),
    .extent        = {
      .width       = width,
      .height      = height,
      .depth       = 1,
    },
    .mipLevels     = 1,
    .arrayLayers   = 1,
    .samples       = VK_SAMPLE_COUNT_1_BIT,
    .tiling        = VK_IMAGE_TILING_OPTIMAL,
    .usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  VmaAllocationCreateInfo vmaAllocationCreateInfo = {
    .usage = VMA_MEMORY_USAGE_GPU_ONLY,
  };

  Vulkan.createImage(&depthImageCreateInfo, &vmaAllocationCreateInfo, &(DATA(depthBufferImage)));

  // Creating the imageview
  VkImageViewCreateInfo depthImageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = DATA(depthBufferImage).image,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = DATA(depthStencilFormat),
    .components = {0, 0, 0, 0},
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    }
  };

  VK_ASSERT(vkCreateImageView(Vulkan.getDevice(), &depthImageViewCreateInfo, NULL, &DATA(depthBufferImageView)));
}

void ev_rendererbackend_createrenderpass()
{
  VkAttachmentDescription attachmentDescriptions[] = 
  {
    {
      .format = DATA(surfaceFormat).format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    },
    {
      .format = DATA(depthStencilFormat),
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

  VK_ASSERT(vkCreateRenderPass(Vulkan.getDevice(), &renderPassCreateInfo, NULL, &DATA(renderPass)));
}

void ev_rendererbackend_startnewframe()
{
  // Create semaphore
  VK_ASSERT(vkAcquireNextImageKHR(Vulkan.getDevice(), DATA(swapchain), ~0ull, SEMAPHORES[IMAGE_SEMAPHORE]/* image semaphore */, NULL, &FRAME));

  //start recording into the right command buffer
  {
    VkCommandBufferBeginInfo commandBufferBeginInfo = { 
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_ASSERT(vkBeginCommandBuffer(CMDBUFFERS[FRAME], &commandBufferBeginInfo));
  }

  /* ev_vulkan_image_memory_barrier(VulkanData.swapchainImages[VulkanData.currentFrameIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT); */

  unsigned int width, height;
  Window.getSize(&width, &height);

  //strarting render pass
  {
    VkClearValue clearValues[] = 
    {
      {
        .color = { {0.33f, 0.22f, 0.37f, 1.f} },
      },
      {
        .depthStencil = {1.0f, 0.0f},
      }
    };

    VkRenderPassBeginInfo renderPassBeginInfo = 
    {
      .sType =  VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = DATA(renderPass),
      .framebuffer = FRAMEBUFFERS[FRAME],
      .renderArea.extent.width = width,
      .renderArea.extent.height = height,
      .clearValueCount = ARRAYSIZE(clearValues),
      .pClearValues = clearValues,
    };
    vkCmdBeginRenderPass(CMDBUFFERS[FRAME], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); 
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

    vkCmdSetViewport(CMDBUFFERS[FRAME], 0, 1, &viewport);
    vkCmdSetScissor (CMDBUFFERS[FRAME], 0, 1, &scissor);
  }
}

void ev_rendererbackend_endframe()
{
  vkCmdEndRenderPass(CMDBUFFERS[FRAME]);

  VkImageMemoryBarrier imageMemoryBarrier = 
  {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .dstAccessMask = 0,
    .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // TODO
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // TODO
    .image = DATA(swapchainImages)[FRAME],
    .subresourceRange = 
    {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .levelCount = VK_REMAINING_MIP_LEVELS,
      .layerCount = VK_REMAINING_ARRAY_LAYERS,
    }
  };

  vkCmdPipelineBarrier(CMDBUFFERS[FRAME], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);


  VK_ASSERT(vkEndCommandBuffer(CMDBUFFERS[FRAME]));

  VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.pWaitDstStageMask = &stageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &CMDBUFFERS[FRAME];
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &SEMAPHORES[IMAGE_SEMAPHORE];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &SEMAPHORES[SUBMISSION_SEMAPHORE];

  VK_ASSERT(vkQueueSubmit(VulkanQueueManager.getQueue(GRAPHICS), 1, &submitInfo, VK_NULL_HANDLE));

  VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &DATA(swapchain);
  presentInfo.pImageIndices = &FRAME;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &SEMAPHORES[SUBMISSION_SEMAPHORE];

  VK_ASSERT(vkQueuePresentKHR(VulkanQueueManager.getQueue(GRAPHICS), &presentInfo));

  // TODO: Do we need to reset the command buffer?
  // TODO: Does it overwrite existing memory if not reset? If so, then resetting it
  // will only introduce the overhead of freeing memory just to allocate it again.
  // 
  // Code:
  //  vkResetCommandBuffer(VulkanData.swapchainCommandBuffers[VulkanData.currentFrameIndex], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  //

  //assert(!"Not implemented");
}

static VkCommandBuffer ev_rendererbackend_getcurrentframecommandbuffer()
{
  return CMDBUFFERS[FRAME];
}
static VkRenderPass ev_rendererbackend_getrenderpass()
{
  return DATA(renderPass);
}

/* void ev_vulkan_destroy_swapchain() */
/* { */
/*   // Destroy semaphores */
/*   vkDestroySemaphore(VulkanData.logicalDevice, VulkanData.imageSemaphore, NULL); */
/*   vkDestroySemaphore(VulkanData.logicalDevice, VulkanData.signalSemaphore, NULL); */

/*   // Free frame commandbuffers */
/*   vkFreeCommandBuffers(VulkanData.logicalDevice, VulkanData.commandPools[GRAPHICS], VulkanData.swapchainImageCount, VulkanData.swapchainCommandBuffers); */

/*   // Destroy depth buffer */
/*   vkDestroyImageView(VulkanData.logicalDevice, VulkanData.depthBufferImageView, NULL); */
/*   Vulkan.destroyImage(&VulkanData.depthBufferImage); */

/*   for(unsigned int i = 0; i < VulkanData.swapchainImageCount; ++i) */
/*   { */
/*     // Destroy Framebuffers */
/*     vkDestroyFramebuffer(VulkanData.logicalDevice, VulkanData.framebuffers[i], NULL); */
    
/*     // Destroy swapchain imageviews */
/*     vkDestroyImageView(VulkanData.logicalDevice, VulkanData.swapchainImageViews[i], NULL); */
/*   } */

/*   vkDestroySwapchainKHR(VulkanData.logicalDevice, VulkanData.swapchain, NULL); */

/*   // Destroying the renderpass */
/*   vkDestroyRenderPass(VulkanData.logicalDevice, VulkanData.renderPass, NULL); */

/*   free(VulkanData.swapchainCommandBuffers); */
/*   free(VulkanData.framebuffers); */
/*   free(VulkanData.swapchainImageViews); */
/*   free(VulkanData.swapchainImages); */
/* } */

#include <stdio.h>
#include <ev_log/ev_log.h>

ShaderModule ev_rendererbackend_loadshader(const char* shaderPath)
{
  FILE* file = fopen(shaderPath, "rb");
  if(!file) return VK_NULL_HANDLE;

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *shaderCode = malloc(length);

  fread(shaderCode, 1, length, file);
  fclose(file);

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = length,
    .pCode = (const unsigned int *)shaderCode,
  };

  VkShaderModule shaderModule;
  VK_ASSERT(vkCreateShaderModule(Vulkan.getDevice(), &shaderModuleCreateInfo, NULL, &shaderModule));

  free(shaderCode);
  return shaderModule;
}

void ev_rendererbackend_unloadshader(ShaderModule shader)
{
  vkDestroyShaderModule(Vulkan.getDevice(), shader, NULL);
}

static void ev_rendererbackend_createresourcememorypool(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool)
{
  unsigned int memoryType;

  { // Detecting memorytype index
    VkBufferCreateInfo sampleBufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    /* sampleBufferCreateInfo.size = 1024; */
    sampleBufferCreateInfo.usage = EV_USAGEFLAGS_RESOURCE_BUFFER;

    VmaAllocationCreateInfo allocationCreateInfo = {
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    vmaFindMemoryTypeIndexForBufferInfo(Vulkan.getAllocator(), &sampleBufferCreateInfo, &allocationCreateInfo, &memoryType);
  }

  VmaPoolCreateInfo poolCreateInfo = {
	  .memoryTypeIndex   = memoryType,
	  .blockSize         = blockSize,
	  .minBlockCount     = minBlockCount,
	  .maxBlockCount     = maxBlockCount,
  };

  Vulkan.allocateMemoryPool(&poolCreateInfo, pool);
}

static void ev_rendererbackend_allocatebufferinpool(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer)
{
  VkBufferCreateInfo bufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.usage = usageFlags;

  Vulkan.allocateBufferInPool(&bufferCreateInfo, pool, buffer);
}

static void ev_rendererbackend_memorydump()
{
  Vulkan.memoryDump();
}

static void ev_rendererbackend_allocatestagingbuffer(unsigned long long bufferSize, MemoryBuffer *buffer)
{
  VmaAllocationCreateInfo allocationCreateInfo = {
    .usage = VMA_MEMORY_USAGE_CPU_ONLY, // TODO: Experiment with VMA_MEMORY_USAGE_CPU_TO_GPU
  };

  VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  Vulkan.createBuffer(&bufferCreateInfo, &allocationCreateInfo, buffer);
}

static void ev_rendererbackend_updatestagingbuffer(MemoryBuffer *buffer, unsigned long long bufferSize, const void *data)
{
  void *mapped;
  vmaMapMemory(Vulkan.getAllocator(), buffer->allocation, &mapped);
  memcpy(mapped, data, bufferSize);
  vmaUnmapMemory(Vulkan.getAllocator(), buffer->allocation);
}

static void ev_rendererbackend_copybuffer(unsigned long long size, MemoryBuffer *src, MemoryBuffer *dst)
{
  VkCommandBuffer tempCommandBuffer;
  VkCommandBufferAllocateInfo tempCommandBufferAllocateInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  tempCommandBufferAllocateInfo.commandPool = DATA(transferCommandPool);
  tempCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  tempCommandBufferAllocateInfo.commandBufferCount = 1;
  vkAllocateCommandBuffers(Vulkan.getDevice(), &tempCommandBufferAllocateInfo, &tempCommandBuffer);

  VkCommandBufferBeginInfo tempCommandBufferBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  tempCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(tempCommandBuffer, &tempCommandBufferBeginInfo);

  VkBufferCopy copyRegion;
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(tempCommandBuffer, src->buffer, dst->buffer, 1, &copyRegion);

  vkEndCommandBuffer(tempCommandBuffer);

  VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

  VkSubmitInfo submitInfo         = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.pWaitDstStageMask    = &stageMask;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &tempCommandBuffer;
  submitInfo.waitSemaphoreCount   = 0;
  submitInfo.pWaitSemaphores      = NULL;
  submitInfo.signalSemaphoreCount = 0;
  submitInfo.pSignalSemaphores    = NULL;

  VkQueue transferQueue = VulkanQueueManager.getQueue(TRANSFER);
  VK_ASSERT(vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE));

  //TODO Change this to take advantage of fences
  vkQueueWaitIdle(transferQueue);

  vkFreeCommandBuffers(Vulkan.getDevice(), DATA(transferCommandPool), 1, &tempCommandBuffer);
}

static int ev_rendererbackend_loadbasepipelines()
{
  //TODO when you implement multiple pipelines take a look at input attributes , this pipeline should be a rigging pipeline

  VkPipelineShaderStageCreateInfo pipelineShaderStages[] = 
  {
    {
      .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage  = VK_SHADER_STAGE_VERTEX_BIT,
      .module = BASE_SHADERS[EV_BASE_SHADER_PBR_VERT],
      .pName  = "main"
    },
    {
      .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = BASE_SHADERS[EV_BASE_SHADER_PBR_FRAG],
      .pName  = "main"
    },
  };

  VkPipelineVertexInputStateCreateInfo pipelineVertexInputState = 
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 0,
    .pVertexBindingDescriptions = NULL,
    .vertexAttributeDescriptionCount = 0,
    .pVertexAttributeDescriptions = NULL
  };

  VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };

  VkPipelineViewportStateCreateInfo pipelineViewportState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .scissorCount = 1,
  };

  VkPipelineRasterizationStateCreateInfo pipelineRasterizationState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .lineWidth = 1.0,
  };

  VkPipelineMultisampleStateCreateInfo pipelineMultisampleState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
  };

  VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    .depthBoundsTestEnable = VK_FALSE,
    .back = {
      .failOp = VK_STENCIL_OP_KEEP,
      .passOp = VK_STENCIL_OP_KEEP,
      .compareOp = VK_COMPARE_OP_ALWAYS
    },
    .front = {
      .failOp = VK_STENCIL_OP_KEEP,
      .passOp = VK_STENCIL_OP_KEEP,
      .compareOp = VK_COMPARE_OP_ALWAYS
    },
    .stencilTestEnable = VK_FALSE
  };

  VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
    .colorWriteMask = 
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_A_BIT ,
  };

  VkPipelineColorBlendStateCreateInfo pipelineColorBlendState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .attachmentCount = 1,
    .pAttachments = &pipelineColorBlendAttachmentState,
  };

  VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo pipelineDynamicState = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = ARRAYSIZE(dynamicStates),
    .pDynamicStates = dynamicStates,
  };

  //push constants
  VkPushConstantRange pc = {
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = 96 //TODO sizeof(push constant struct)
  };

  VkDescriptorSetLayout setLayouts[] = {
    BASE_DESCRIPTOR_SET_LAYOUTS[EV_DESCRIPTOR_SET_LAYOUT_RIG],
    BASE_DESCRIPTOR_SET_LAYOUTS[EV_DESCRIPTOR_SET_LAYOUT_TEXTURE],
  };

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = ARRAYSIZE(setLayouts),
    .pSetLayouts = setLayouts,

    .pushConstantRangeCount = 1,
    .pPushConstantRanges = &pc
  };

  VK_ASSERT(vkCreatePipelineLayout(Vulkan.getDevice(), &pipelineLayoutCreateInfo, NULL, &BASE_PIPELINES_LAYOUTS[EV_GRAPHICS_PIPELINE_PBR]));

  // The graphicsPipelinesCreateInfos array should follow the order set by
  // the GraphicsPipelineUsage enum.
  VkGraphicsPipelineCreateInfo graphicsPipelinesCreateInfos[] = 
  { 
    {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = ARRAYSIZE(pipelineShaderStages),
      .pStages = pipelineShaderStages,
      .pVertexInputState = &pipelineVertexInputState,
      .pInputAssemblyState = &pipelineInputAssemblyState,
      .pViewportState = &pipelineViewportState,
      .pRasterizationState = &pipelineRasterizationState,
      .pMultisampleState = &pipelineMultisampleState,
      .pDepthStencilState = &pipelineDepthStencilState,
      .pColorBlendState = &pipelineColorBlendState,
      .pDynamicState = &pipelineDynamicState,
      .layout = BASE_PIPELINES_LAYOUTS[EV_GRAPHICS_PIPELINE_PBR],
      .renderPass = RendererBackend.getRenderPass(),
      .subpass = 0, // TODO Read more about the graphics pipelines and what this number represents (first subpass to run?)
    } 
  };

  VK_ASSERT(
    vkCreateGraphicsPipelines(
      Vulkan.getDevice(), NULL, 
      ARRAYSIZE(graphicsPipelinesCreateInfos), 
      graphicsPipelinesCreateInfos, NULL, 
      &BASE_PIPELINES[EV_GRAPHICS_PIPELINE_PBR]));
  return 0;
}

static int ev_rendererbackend_loadbaseshaders()
{
#include <Renderer/baseshaderpaths.txt>

  for(int i = 0; i < EV_BASE_SHADER_COUNT; ++i)
  {
    BASE_SHADERS[i] = ev_rendererbackend_loadshader(BASE_SHADERS_PATHS[i]);
  }
  return 0;
}

static int ev_rendererbackend_loadbasedescriptorsetlayouts()
{
  //SET 0 FOR TEXTURES
  {
    VkDescriptorSetLayoutBinding bindings[] =
    { 
      {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImmutableSamplers = NULL,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
      }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = 
    {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = ARRAYSIZE(bindings),
      .pBindings = bindings
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan.getDevice(), &descriptorSetLayoutCreateInfo, NULL, &BASE_DESCRIPTOR_SET_LAYOUTS[EV_DESCRIPTOR_SET_LAYOUT_TEXTURE]));
  }

   //SET 1 FOR rigging
  {
    VkDescriptorSetLayoutBinding bindings[] =
    { 
      {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImmutableSamplers = NULL,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
      }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = ARRAYSIZE(bindings),
      .pBindings = bindings
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan.getDevice(), &descriptorSetLayoutCreateInfo, NULL, &BASE_DESCRIPTOR_SET_LAYOUTS[EV_DESCRIPTOR_SET_LAYOUT_RIG]));
  }
  return 0;
}

static int ev_rendererbackend_bindpipeline(GraphicsPipelineType type)
{
  vkCmdBindPipeline(CMDBUFFERS[FRAME], VK_PIPELINE_BIND_POINT_GRAPHICS, BASE_PIPELINES[type]);
  RendererBackendData.boundPipeline = type;
  return 0;
}

static int ev_rendererbackend_binddescriptorsets(DescriptorSet *descriptorSets, unsigned int count)
{
  vkCmdBindDescriptorSets(CMDBUFFERS[FRAME], VK_PIPELINE_BIND_POINT_GRAPHICS, BOUND_PIPELINE_LAYOUT, 0, count, descriptorSets, 0, 0);
  return 0;
}

static int ev_rendererbackend_allocatedescriptorset(DescriptorSetLayoutType setLayoutType, DescriptorSet *descriptorSet)
{
  VkDescriptorSetAllocateInfo setAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  setAllocateInfo.descriptorPool = DATA(descriptorPool);
  setAllocateInfo.descriptorSetCount = 1;
  setAllocateInfo.pSetLayouts = &BASE_DESCRIPTOR_SET_LAYOUTS[EV_DESCRIPTOR_SET_LAYOUT_TEXTURE];

  vkAllocateDescriptorSets(Vulkan.getDevice(), &setAllocateInfo, descriptorSet);
  return 0;
}

static int ev_rendererbackend_pushdescriptorstoset(DescriptorSet descriptorSet, Descriptor *descriptors, unsigned int descriptorsCount)
{
  VkWriteDescriptorSet *setWrites = malloc(descriptorsCount * sizeof(VkWriteDescriptorSet));
  VkDescriptorBufferInfo *bufferInfos = malloc(descriptorsCount * sizeof(VkDescriptorBufferInfo));

  for(unsigned int i = 0; i < descriptorsCount; ++i)
  {
    switch(descriptors[i].type)
    {
      case EV_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      case EV_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      case EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case EV_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        bufferInfos[i] = (VkDescriptorBufferInfo){
          .buffer = ((MemoryBuffer*)descriptors[i].descriptorData)->buffer,
          .offset = 0,
          .range = VK_WHOLE_SIZE,
        };
        setWrites[i] = (VkWriteDescriptorSet){
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .descriptorCount = 1,
          .descriptorType = (VkDescriptorType)descriptors[i].type,
          .dstSet = descriptorSet,
          .pBufferInfo = &bufferInfos[i],
        };
        break;

      case EV_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      case EV_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        break;

      case EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
      case EV_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      case EV_DESCRIPTOR_TYPE_SAMPLER:
      case EV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      case EV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      default:
        ;
    }
  }

  vkUpdateDescriptorSets(Vulkan.getDevice(), descriptorsCount, setWrites, 0, NULL);

  free(setWrites);
  return 0;
}