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
  ev_log_debug("Initializing RendererBackend");
  RendererBackend.init();
  ev_log_debug("Initialized RendererBackend");
  vec_init(&RendererData.indexBuffers);
  vec_init(&RendererData.vertexBuffers);

  ev_log_debug("Loading BaseShaders");
  RendererBackend.loadBaseShaders();
  ev_log_debug("Loaded BaseShaders");
  ev_log_debug("Loading BaseDescriptorSetLayouts");
  RendererBackend.loadBaseDescriptorSetLayouts();
  ev_log_debug("Loaded BaseDescriptorSetLayouts");
  ev_log_debug("Loading BasePipelines");
  RendererBackend.loadBasePipelines();
  ev_log_debug("Loaded BasePipelines");

  ev_log_debug("Allocating ResourceMemoryPool");
  RendererBackend.createResourceMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.resourcePool);
  ev_log_debug("Allocated ResourceMemoryPool");


  /* RendererBackend.startNewFrame(); */

  /* RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR); */
  /* RendererBackend.bindDescriptorSets(&descriptorSet, 1); */

  /* { */
  /*   vkCmdDrawIndexed(RendererBackend.getCurrentFrameCommandBuffer(), ARRAYSIZE(indices), 1, 0, 0, 0); */
  /* } */

  /* RendererBackend.endFrame(); */

  /* free(descriptors); */


  /* RendererBackend.memoryDump(); */

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

/* static void ev_renderer_bind() // TODO */
/* { */
/*   //TODO look into using secondary command buffer and recording for every pipeline */

/*   //TODO LOOK MORE INTO THIS */
/*   vkCmdBindPipeline(RendererBackend.getCurrentFrameCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, RendererData.graphicsPipelines[EV_GRAPHICS_PIPELINE_PBR]); */

/*   //Also not sure how we will handle this. but there it is -- will we have one big buffer with all meshes and we will cherry pick from it for every pipeline' */
/*   //or will we have specific buffer or each type of mesh that requires a specific type of pipeline and bind it seperatly. */
/*   //TODO LOOK FURTHER INTO THIS */
/*   { */
/*     // VkBuffer vertexBuffers[] = { 0 }; */
/*     // VkDeviceSize offsets[] = { 0 }; */
/*     // vkCmdBindVertexBuffers(Vulkan.getCurrentFrameCommandBuffer(), 0, ARRAYSIZE(vertexBuffers), vertexBuffers, offsets); */
/*     //TODO SUPPORT HAVING A MODEL WITH NO VERTEX BUFFER ? */
/*     /1* vkCmdBindIndexBuffer(Vulkan.getCurrentFrameCommandBuffer(), indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32); *1/ */
/*   } */

/*   //TODO DRAW LOOP HERE nad also look into supporting shaders with no indexes */
/*   { */
/*     //vkCmdDrawIndexed(Vulkan.getCurrentFrameCommandBuffer(), indices_counts[TYPE][i], 1, indices_offsets[TYPE][i], vertices_offsets[TYPE][i], 0); */
/*     vkCmdDraw(RendererBackend.getCurrentFrameCommandBuffer(), 3, 1, 0, 0); */

/*   } */
/* } */

static void EV_UNUSED ev_renderer_update_resources(MeshComponent* meshes, unsigned int meshes_count)
{
}

// TODO TODO Remove
DescriptorSet descriptorSet;
static int ev_renderer_startframe()
{
  // TODO Error reporting
  if(descriptorSet == VK_NULL_HANDLE)
  {
    ev_log_debug("Creating DescriptorSet");
    RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_TEXTURE, &descriptorSet);
    ev_log_debug("Allocated DescriptorSet");
    Descriptor *descriptors = malloc(sizeof(MemoryBuffer) * RendererData.vertexBuffers.length);
    for(int i = 0; i < RendererData.vertexBuffers.length; ++i)
    {
      // TODO What would be the difference if we switched to EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER?
      descriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, RendererData.vertexBuffers.data + i};
    }

    ev_log_debug("Pushing Descriptors to DescriptorSet");
    RendererBackend.pushDescriptorsToSet(descriptorSet, descriptors, RendererData.vertexBuffers.length);
    ev_log_debug("Finished pushing Descriptors to DescriptorSet");
  }

  ev_log_debug("Starting API specific new frame initialization : RendererBackend.startNewFrame()");
  RendererBackend.startNewFrame();
  ev_log_debug("Finished API specific new frame initialization : RendererBackend.startNewFrame()");

  RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR);
  RendererBackend.bindDescriptorSets(&descriptorSet, 1);

  return 0;
}

static int ev_renderer_endframe()
{
  // TODO Error reporting
  RendererBackend.endFrame();
  vkDeviceWaitIdle(Vulkan.getDevice());
  return 0;
}

static void ev_renderer_draw(PrimitiveRenderData primitiveRenderData, ev_Matrix4 transformMatrix)
{
  //TODO Create a "pushconstant?" struct that should hold the data that will be passed to the pipeline
  RendererBackend.pushConstant(&primitiveRenderData.vertexBufferId, sizeof(unsigned int));

  RendererBackend.bindIndexBuffer(&(RendererData.indexBuffers.data[primitiveRenderData.indexBufferId]));
  /* RendererBackend.bindIndexBuffer(&(RendererData.indexBuffers.data[0])); */

  RendererBackend.drawIndexed(primitiveRenderData.indexCount);
}
