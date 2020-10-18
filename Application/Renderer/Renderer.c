#include "Renderer.h"
#include "EventSystem.h"
#include "events/events.h"
#include "World/modules/geometry_module.h"
#include "vec.h"

static int ev_renderer_init();
static int ev_renderer_deinit();

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size);
static unsigned int ev_renderer_registervertexbuffer(real *vertices, unsigned long long size);


struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit = ev_renderer_deinit,

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
  RendererBackend.init();
  vec_init(&RendererData.indexBuffers);
  vec_init(&RendererData.vertexBuffers);

  RendererBackend.loadBaseShaders();
  RendererBackend.loadBaseDescriptorSetLayouts();
  RendererBackend.loadBasePipelines();

  RendererBackend.createResourceMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.resourcePool);

  /* DescriptorSet descriptorSet; */
  /* RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_TEXTURE, &descriptorSet); */

  
  /* Descriptor *descriptors = malloc(sizeof(MemoryBuffer) * RendererData.vertexBuffers.length); */

  /* for(int i = 0; i < RendererData.vertexBuffers.length; ++i) */
  /* { */
  /*   descriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RendererData.vertexBuffers.data + i}; */
  /* } */

  /* RendererBackend.pushDescriptorsToSet(descriptorSet, descriptors, RendererData.vertexBuffers.length); */

  /* RendererBackend.startNewFrame(); */

  /* RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR); */
  /* RendererBackend.bindDescriptorSets(&descriptorSet, 1); */

  /* /1* { *1/ */
  /* /1*   vkCmdDrawIndexed(RendererBackend.getCurrentFrameCommandBuffer(), ARRAYSIZE(indices), 1, 0, 0, 0); *1/ */
  /* /1* } *1/ */

  /* RendererBackend.endFrame(); */

  /* free(descriptors); */


  RendererBackend.memoryDump();

  return 0;
}

static int ev_renderer_deinit()
{
  return 0;
}

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size)
{
  unsigned int idx = RendererData.indexBuffers.length;

  MemoryBuffer newIndexBuffer;
  RendererBackend.allocateBufferInPool(RendererData.resourcePool, size, EV_USAGEFLAGS_RESOURCE_BUFFER, &newIndexBuffer);

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
