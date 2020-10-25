#include "Renderer.h"
#include "EventSystem.h"
#include "Renderer/renderer_types.h"
#include "events/events.h"
#include "World/modules/geometry_module.h"
#include "vec.h"
#include <vulkan/vulkan_core.h>
#include <ev_log/ev_log.h>

static int ev_renderer_init();
static int ev_renderer_deinit();

static int ev_renderer_startframe();
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
DescriptorSet descriptorSet;
static int ev_renderer_startframe()
{
  // TODO Error reporting
  if(descriptorSet == VK_NULL_HANDLE)
  {
    ev_log_trace("Creating DescriptorSet");
    RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_TEXTURE, &descriptorSet);
    ev_log_trace("Allocated DescriptorSet");
    Descriptor *descriptors = malloc(sizeof(MemoryBuffer) * RendererData.vertexBuffers.length);
    for(int i = 0; i < RendererData.vertexBuffers.length; ++i)
    {
      // TODO What would be the difference if we switched to EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER?
      descriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, RendererData.vertexBuffers.data + i};
    }

    ev_log_trace("Pushing Descriptors to DescriptorSet");
    RendererBackend.pushDescriptorsToSet(descriptorSet, descriptors, RendererData.vertexBuffers.length);
    ev_log_trace("Finished pushing Descriptors to DescriptorSet");
  }

  ev_log_trace("Starting API specific new frame initialization : RendererBackend.startNewFrame()");
  RendererBackend.startNewFrame();
  ev_log_trace("Finished API specific new frame initialization : RendererBackend.startNewFrame()");

  RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR);
  RendererBackend.bindDescriptorSets(&descriptorSet, 1);

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

static void ev_renderer_draw(PrimitiveRenderData primitiveRenderData, ev_Matrix4 transformMatrix)
{
  //TODO Create a "pushconstant?" struct that should hold the data that will be passed to the pipeline
  RendererBackend.pushConstant(&primitiveRenderData.vertexBufferId, sizeof(unsigned int));

  RendererBackend.bindIndexBuffer(&(RendererData.indexBuffers.data[primitiveRenderData.indexBufferId]));

  RendererBackend.drawIndexed(primitiveRenderData.indexCount);
}
