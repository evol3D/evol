#include "Renderer.h"
#include "EventSystem.h"
#include "vec.h"
#include <ev_log/ev_log.h>
#include <stdio.h>
#include <cglm/cglm.h>

static int ev_renderer_init();
static int ev_renderer_deinit();

static int ev_renderer_startframe(ev_RenderCamera *camera);
static int ev_renderer_endframe();


static void ev_renderer_draw(MeshRenderData meshRenderData, ev_Matrix4 transformMatrix);

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size);
static unsigned int ev_renderer_registervertexbuffer(real *vertices, unsigned long long size);
static unsigned int ev_renderer_registernormalbuffer(real *normals, unsigned long long size);
static unsigned int ev_renderer_registermaterial(void* pixels, uint32_t width, uint32_t height);

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit   = ev_renderer_deinit,

  .startFrame = ev_renderer_startframe,
  .endFrame = ev_renderer_endframe,

  .draw = ev_renderer_draw,

  .registerIndexBuffer = ev_renderer_registerindexbuffer,
  .registerVertexBuffer = ev_renderer_registervertexbuffer,
  .registerNormalBuffer = ev_renderer_registernormalbuffer,
  .registerMaterial = ev_renderer_registermaterial,
};

struct ev_Renderer_Data {
  MemoryPool resourcePool;
  MemoryPool imagePool;

  MemoryBufferVec indexBuffers;
  MemoryBufferVec vertexBuffers;
  MemoryBufferVec normalBuffers;

  MemoryImageVec textureBuffers;

  UBO cameraUBO;

} RendererData;

static int ev_renderer_init()
{
  ev_log_trace("Initializing RendererBackend");
  RendererBackend.init();
  ev_log_debug("Initialized RendererBackend");
  vec_init(&RendererData.indexBuffers);
  vec_init(&RendererData.vertexBuffers);
  vec_init(&RendererData.normalBuffers);
  vec_init(&RendererData.textureBuffers);

  ev_log_trace("Loading BaseShaders");
  RendererBackend.loadBaseShaders();
  ev_log_debug("Loaded BaseShaders");
  ev_log_trace("Loading BaseDescriptorSetLayouts");
  RendererBackend.loadBaseDescriptorSetLayouts();
  ev_log_debug("Loaded BaseDescriptorSetLayouts");
  ev_log_trace("Loading BasePipelines");
  RendererBackend.loadBasePipelines();
  ev_log_debug("Loaded BasePipelines");

  ev_log_trace("Allocating ResourceMemoryPool");
  RendererBackend.createResourceMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.resourcePool);
  ev_log_debug("Allocated ResourceMemoryPool");

  ev_log_trace("Allocating ResourceMemoryPool");
  RendererBackend.createResourceMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.imagePool);
  ev_log_debug("Allocated ResourceMemoryPool");

  // Create a persistent UBO for the main camera
  RendererBackend.allocateUBO(sizeof(ev_RenderCamera), true, &RendererData.cameraUBO);

  return 0;
}

static int ev_renderer_deinit()
{
  RendererBackend.deinit();

  // Free the persistent UBO used for the main camera
  RendererBackend.freeUBO(&RendererData.cameraUBO);

  // Free all buffers used for index-buffer storage
  MemoryBuffer *buffer; EvTexture *texture; unsigned int idx;
  vec_foreach_ptr(&RendererData.indexBuffers, buffer, idx)
  {
    RendererBackend.freeMemoryBuffer(buffer);
  }
  vec_deinit(&RendererData.indexBuffers);

  // Free all buffers used for vertex-buffer storage
  vec_foreach_ptr(&RendererData.vertexBuffers, buffer, idx)
  {
    RendererBackend.freeMemoryBuffer(buffer);
  }
  vec_deinit(&RendererData.vertexBuffers);

  // Free all buffers used for normal-buffer storage
  vec_foreach_ptr(&RendererData.normalBuffers, buffer, idx)
  {
    RendererBackend.freeMemoryBuffer(buffer);
  }
  vec_deinit(&RendererData.normalBuffers);

  // Free all buffers used for images storage
  vec_foreach_ptr(&RendererData.textureBuffers, texture, idx)
  {
    RendererBackend.freeImage(&texture->image);
  }
  vec_deinit(&RendererData.textureBuffers);

  // After freeing all the buffers in this pool, free the pool itself
  RendererBackend.freeMemoryPool(RendererData.resourcePool);
  RendererBackend.freeMemoryPool(RendererData.imagePool);

  RendererBackend.memoryDump();
  return 0;
}

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size)
{
  unsigned int idx = RendererData.indexBuffers.length;

  MemoryBuffer newIndexBuffer;
  RendererBackend.allocateBufferInPool(RendererData.resourcePool, size, EV_BUFFER_USAGE_INDEX_BUFFER_BIT, &newIndexBuffer);

  MemoryBuffer indexStagingBuffer;
  RendererBackend.allocateStagingBuffer(size, &indexStagingBuffer);
  RendererBackend.updateStagingBuffer(&indexStagingBuffer, size, indices);
  RendererBackend.copyBuffer(size, &indexStagingBuffer, &newIndexBuffer);

  //TODO We should have a system that controls staging buffers
  RendererBackend.freeMemoryBuffer(&indexStagingBuffer);

  vec_push(&RendererData.indexBuffers, newIndexBuffer);

  return idx;
}

static unsigned int ev_renderer_registervertexbuffer(real *vertices, unsigned long long size)
{
  unsigned int idx = RendererData.vertexBuffers.length;

  MemoryBuffer newVertexBuffer;
  RendererBackend.allocateBufferInPool(RendererData.resourcePool, size, EV_USAGEFLAGS_RESOURCE_BUFFER, &newVertexBuffer);

  MemoryBuffer vertexStagingBuffer;
  RendererBackend.allocateStagingBuffer(size, &vertexStagingBuffer);
  RendererBackend.updateStagingBuffer(&vertexStagingBuffer, size, vertices);
  RendererBackend.copyBuffer(size, &vertexStagingBuffer, &newVertexBuffer);

  //TODO We should have a system that controls staging buffers
  RendererBackend.freeMemoryBuffer(&vertexStagingBuffer);

  vec_push(&RendererData.vertexBuffers, newVertexBuffer);

  return idx;
}

static unsigned int ev_renderer_registernormalbuffer(real *normals, unsigned long long size)
{
  unsigned int idx = RendererData.normalBuffers.length;

  MemoryBuffer newNormalBuffer;
  RendererBackend.allocateBufferInPool(RendererData.resourcePool, size, EV_USAGEFLAGS_RESOURCE_BUFFER, &newNormalBuffer);

  MemoryBuffer normalStagingBuffer;
  RendererBackend.allocateStagingBuffer(size, &normalStagingBuffer);
  RendererBackend.updateStagingBuffer(&normalStagingBuffer, size, normals);
  RendererBackend.copyBuffer(size, &normalStagingBuffer, &newNormalBuffer);

  //TODO We should have a system that controls staging buffers
  RendererBackend.freeMemoryBuffer(&normalStagingBuffer);

  vec_push(&RendererData.normalBuffers, newNormalBuffer);

  return idx;
}

static unsigned int ev_renderer_registermaterial(void* pixels, uint32_t width, uint32_t height)
{
  unsigned int idx = RendererData.textureBuffers.length;

  uint32_t size = width * height * 4;

  MemoryImage newimage;
  RendererBackend.allocateImageInPool(RendererData.imagePool, width, height , EV_USAGEFLAGS_RESOURCE_IMAGE, &newimage);

  MemoryBuffer imageStagingBuffer;
  RendererBackend.allocateStagingBuffer(size, &imageStagingBuffer);
  RendererBackend.updateStagingBuffer(&imageStagingBuffer, size, pixels);

  RendererBackend.trasitionImageLayout(newimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  RendererBackend.copyBufferToImage(imageStagingBuffer.buffer, newimage.image, width, height);
  RendererBackend.trasitionImageLayout(newimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  RendererBackend.freeMemoryBuffer(&imageStagingBuffer);

  VkImageView imageView;
  RendererBackend.createImageView(VK_FORMAT_R8G8B8A8_SRGB, &newimage.image, &imageView);

  VkSampler sampler;
  VkSamplerCreateInfo samplerInfo =
  {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .anisotropyEnable = VK_TRUE,
      .maxAnisotropy = 16.0f,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .mipLodBias = 0.0f,
      .minLod = 0.0f,
      .maxLod = 0.0f,
  };
  vkCreateSampler(Vulkan.getDevice(), &samplerInfo, NULL, &sampler);

  EvTexture evtexture =
  {
      .image = newimage,
      .imageView = imageView,
      .sampler = sampler
  };

  vec_push(&RendererData.textureBuffers, evtexture);

  return idx;
}

static int ev_renderer_startframe(ev_RenderCamera *camera)
{
  ev_log_trace("Starting API specific new frame initialization : RendererBackend.startNewFrame()");
  RendererBackend.startNewFrame();
  ev_log_trace("Finished API specific new frame initialization : RendererBackend.startNewFrame()");

  // Create a descriptor set that contains the Projection/View Matrices
  DescriptorSet cameraDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_CAMERA_PARAM, &cameraDescriptorSet);
  Descriptor cameraDescriptors[] = {{EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &RendererData.cameraUBO.buffer},};
  RendererBackend.pushDescriptorsToSet(cameraDescriptorSet, cameraDescriptors, ARRAYSIZE(cameraDescriptors), 0);

  // Update Camera UBO
  RendererBackend.updateUBO(sizeof(ev_RenderCamera), camera, &RendererData.cameraUBO);

  // Create a descriptor set that contains all the resources needed by the shader
  DescriptorSet resourceDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_BUFFER_ARR, &resourceDescriptorSet);
  Descriptor *vertexDescriptors = malloc(sizeof(Descriptor) * RendererData.vertexBuffers.length);
  Descriptor *normalDescriptors = malloc(sizeof(Descriptor) * RendererData.normalBuffers.length);
  for(int i = 0; i < RendererData.vertexBuffers.length; ++i)
    vertexDescriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, &RendererData.vertexBuffers.data[i]};
  for(int i = 0; i < RendererData.normalBuffers.length; ++i)
    normalDescriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, &RendererData.normalBuffers.data[i]};
  RendererBackend.pushDescriptorsToSet(resourceDescriptorSet, vertexDescriptors, RendererData.vertexBuffers.length, 0);
  RendererBackend.pushDescriptorsToSet(resourceDescriptorSet, normalDescriptors, RendererData.normalBuffers.length, 1);
  free(vertexDescriptors);
  free(normalDescriptors);

  DescriptorSet textureDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_BUFFER_MAT, &textureDescriptorSet);
  Descriptor *textureDescriptors = malloc(sizeof(Descriptor) * RendererData.textureBuffers.length);
  for(int i = 0; i < RendererData.textureBuffers.length; ++i)
    textureDescriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &RendererData.textureBuffers.data[i]};
  RendererBackend.pushDescriptorsToSet(textureDescriptorSet, textureDescriptors, RendererData.textureBuffers.length, 0);
  free(textureDescriptors);

  DescriptorSet descriptorSets[] = {
    cameraDescriptorSet,
    resourceDescriptorSet,
    textureDescriptorSet
  };

  RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_BASE);
  RendererBackend.bindDescriptorSets(descriptorSets, ARRAYSIZE(descriptorSets));

  return 0;
}

static void ev_renderer_draw(MeshRenderData meshRenderData, ev_Matrix4 transformMatrix)
{
  RendererBackend.bindPipeline(meshRenderData.pipelineType);

  int primitiveIdx = meshRenderData.primitives.length - 1;
  for(;primitiveIdx >= 0; --primitiveIdx)
  {
    PrimitiveRenderData *currentPrimitive = &meshRenderData.primitives.data[primitiveIdx];

    struct {
      unsigned int vertexBufferIndex;
      unsigned int normalBufferIndex;
      ev_Matrix4 modelMatrix;
    } params;
    params.vertexBufferIndex = currentPrimitive->vertexBufferId;
    params.normalBufferIndex = currentPrimitive->normalBufferId;
    glm_mat4_dup(transformMatrix, params.modelMatrix);

    RendererBackend.pushConstant(&params, sizeof(params));
    RendererBackend.bindIndexBuffer(&(RendererData.indexBuffers.data[currentPrimitive->indexBufferId]));
    RendererBackend.drawIndexed(currentPrimitive->indexCount);
  }
}

static int ev_renderer_endframe()
{
  // TODO Error reporting
  RendererBackend.endFrame();
  return 0;
}
