#include "Renderer.h"
#include "EventSystem.h"
#include "Renderer/renderer_types.h"
#include "events/events.h"
#include "World/modules/geometry_module.h"
#include "vec.h"
#include <vulkan/vulkan_core.h>
#include <ev_log/ev_log.h>
#include <stdio.h>

static int ev_renderer_init();
static int ev_renderer_deinit();

static int ev_renderer_startframe(ev_RenderCamera *camera);
static int ev_renderer_endframe();


static void ev_renderer_draw(PrimitiveRenderData primitiveRenderData, ev_Matrix4 transformMatrix);

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size);
static unsigned int ev_renderer_registervertexbuffer(real *vertices, unsigned long long size);

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit = ev_renderer_deinit,

  .startFrame = ev_renderer_startframe,
  .endFrame = ev_renderer_endframe,

  .draw = ev_renderer_draw,

  .registerIndexBuffer = ev_renderer_registerindexbuffer,
  .registerVertexBuffer = ev_renderer_registervertexbuffer,
};

typedef vec_t(MemoryBuffer) MemoryBufferVec;

struct ev_Renderer_Data {
  MemoryPool resourcePool;

  MemoryBufferVec indexBuffers;
  MemoryBufferVec vertexBuffers;

} RendererData;

static int ev_renderer_init()
{
  ev_log_trace("Initializing RendererBackend");
  RendererBackend.init();
  ev_log_debug("Initialized RendererBackend");
  vec_init(&RendererData.indexBuffers);
  vec_init(&RendererData.vertexBuffers);

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

  return 0;
}

static int ev_renderer_deinit()
{
  vec_deinit(&RendererData.indexBuffers);
  vec_deinit(&RendererData.vertexBuffers);
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
  // TODO free/reuse staging buffer

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
  // TODO free/reuse staging buffer

  vec_push(&RendererData.vertexBuffers, newVertexBuffer);

  return idx;
}

// TODO TODO Remove
DescriptorSet cameraDescriptorSet = VK_NULL_HANDLE;
DescriptorSet resourceDescriptorSet = VK_NULL_HANDLE;
MemoryBuffer cameraParamBuffer;
static int ev_renderer_startframe(ev_RenderCamera *camera)
{
  if(cameraDescriptorSet == VK_NULL_HANDLE)
  {
    RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_CAMERA_PARAM, &cameraDescriptorSet);
    RendererBackend.allocateUniformBuffer(sizeof(ev_RenderCamera), &cameraParamBuffer);
  }

  Descriptor cameraDescriptors[] = {
    {EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &cameraParamBuffer},
  };
  RendererBackend.pushDescriptorsToSet(cameraDescriptorSet, cameraDescriptors, ARRAYSIZE(cameraDescriptors));
  RendererBackend.updateStagingBuffer(&cameraParamBuffer, sizeof(ev_RenderCamera), camera);

  // TODO Error reporting
  if(resourceDescriptorSet == VK_NULL_HANDLE)
  {
    ev_log_trace("Creating DescriptorSet");
    RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_TEXTURE, &resourceDescriptorSet);
    ev_log_trace("Allocated DescriptorSet");
  }
  Descriptor *resourceDescriptors = malloc(sizeof(Descriptor) * RendererData.vertexBuffers.length);
  for(int i = 0; i < RendererData.vertexBuffers.length; ++i)
  {
    // TODO What would be the difference if we switched to EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER?
    resourceDescriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, &RendererData.vertexBuffers.data[i]};
  }
  ev_log_trace("Pushing Descriptors to DescriptorSet");
  RendererBackend.pushDescriptorsToSet(resourceDescriptorSet, resourceDescriptors, RendererData.vertexBuffers.length);
  ev_log_trace("Finished pushing Descriptors to DescriptorSet");
  free(resourceDescriptors);

  ev_log_trace("Starting API specific new frame initialization : RendererBackend.startNewFrame()");
  RendererBackend.startNewFrame();
  ev_log_trace("Finished API specific new frame initialization : RendererBackend.startNewFrame()");

  DescriptorSet descriptorSets[] = {
    resourceDescriptorSet,
    cameraDescriptorSet,
  };

  RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR);
  RendererBackend.bindDescriptorSets(descriptorSets, ARRAYSIZE(descriptorSets));

  return 0;
}

static int ev_renderer_endframe()
{
  // TODO Error reporting
  RendererBackend.endFrame();
  // TODO Use fences
  vkDeviceWaitIdle(Vulkan.getDevice());
  return 0;
}

#include <cglm/cglm.h>
static void ev_renderer_draw(PrimitiveRenderData primitiveRenderData, ev_Matrix4 transformMatrix)
{
  struct {
    unsigned int vertexBufferIndex;
    ev_Matrix4 modelMatrix;
  } params;
  params.vertexBufferIndex = primitiveRenderData.vertexBufferId;
  glm_mat4_dup(transformMatrix, params.modelMatrix);

  RendererBackend.pushConstant(&params, sizeof(params));

  RendererBackend.bindIndexBuffer(&(RendererData.indexBuffers.data[primitiveRenderData.indexBufferId]));

  RendererBackend.drawIndexed(primitiveRenderData.indexCount);
}
