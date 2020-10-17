#include "Renderer.h"
#include "EventSystem.h"
#include "events/events.h"
#include "World/modules/geometry_module.h"

static int ev_renderer_init();
static int ev_renderer_deinit();

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit = ev_renderer_deinit,
};

struct ev_Renderer_Data {
  MemoryPool resourcePool;
} RendererData;

static int ev_renderer_init()
{
  RendererBackend.init();

  RendererBackend.loadBaseShaders();
  RendererBackend.loadBaseDescriptorSetLayouts();
  RendererBackend.loadBasePipelines();

  RendererBackend.createResourceMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.resourcePool);

  const ev_Vector3 vertices[] = 
  {
      {    0,  0.5, 0, 1},
      { -0.5, -0.5, 0, 1},
      {  0.5, -0.5, 0, 1},
  };

  MemoryBuffer vertexBuffer;
  RendererBackend.allocateBufferInPool(RendererData.resourcePool, sizeof(ev_Vector3) * ARRAYSIZE(vertices), EV_USAGEFLAGS_RESOURCE_BUFFER, &vertexBuffer);

  MemoryBuffer stagingBuffer;
  RendererBackend.allocateStagingBuffer(sizeof(ev_Vector3) * ARRAYSIZE(vertices), &stagingBuffer);
  RendererBackend.updateStagingBuffer(&stagingBuffer, sizeof(ev_Vector3) * ARRAYSIZE(vertices), vertices); 
  RendererBackend.copyBuffer(sizeof(ev_Vector3) * ARRAYSIZE(vertices), &stagingBuffer, &vertexBuffer);

  DescriptorSet descriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_TEXTURE, &descriptorSet);

  Descriptor descriptors[] = 
  {
    {EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &vertexBuffer},
  };

  RendererBackend.pushDescriptorsToSet(descriptorSet, descriptors, ARRAYSIZE(descriptors));

  RendererBackend.startNewFrame();

  RendererBackend.bindPipeline(EV_GRAPHICS_PIPELINE_PBR);
  RendererBackend.bindDescriptorSets(&descriptorSet, 1);

  {
    vkCmdDraw(RendererBackend.getCurrentFrameCommandBuffer(), 3, 1, 0, 0);
  }

  RendererBackend.endFrame();

  RendererBackend.memoryDump();

  return 0;
}

static int ev_renderer_deinit()
{
  return 0;
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
