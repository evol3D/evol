#include <Renderer/RendererBackend.h>
#include <Window.h>
#include <Vulkan.h>
#include <stdlib.h>

static int ev_rendererbackend_init();
static int ev_rendererbackend_deinit();
static void ev_rendererbackend_startnewframe();
static void ev_rendererbackend_endframe();

ShaderModule ev_rendererbackend_loadshader(const char* shaderPath);
void ev_rendererbackend_unloadshader(ShaderModule shader);

static VkCommandBuffer ev_rendererbackend_getcurrentframecommandbuffer();
static VkRenderPass ev_rendererbackend_getrenderpass();

static void ev_rendererbackend_createresourcememorypool(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool);
static void ev_rendererbackend_allocatebufferinpool(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer);

static void ev_rendererbackend_memorydump();

struct ev_RendererBackend RendererBackend = 
{
  .init = ev_rendererbackend_init,
  .deinit = ev_rendererbackend_deinit,
  .startNewFrame = ev_rendererbackend_startnewframe,
  .endFrame = ev_rendererbackend_endframe,

  .loadShader = ev_rendererbackend_loadshader,
  .unloadShader = ev_rendererbackend_unloadshader,

  .createResourceMemoryPool = ev_rendererbackend_createresourcememorypool,
  .allocateBufferInPool = ev_rendererbackend_allocatebufferinpool,
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

  unsigned int currentFrameIndex;

  VkRenderPass renderPass;

  VkSemaphore *semaphores;

} RendererBackendData;

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
