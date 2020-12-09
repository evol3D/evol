#include "Renderer.h"

#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include "cglm/cglm.h"
#include "stdio.h"
#include "vec.h"

static int ev_renderer_init();
static int ev_renderer_deinit();

static int ev_renderer_startframe(ev_RenderCamera *camera);
static int ev_renderer_endframe();

static void ev_renderer_draw(MeshRenderData meshRenderData, ev_Matrix4 transformMatrix);

static unsigned int ev_renderer_registerindexbuffer(unsigned int *indices, unsigned long long size);
static unsigned int ev_renderer_registervertexbuffer(real *vertices, unsigned long long size);
static void ev_renderer_registermaterialbuffer(Material* materials, unsigned long long size);

typedef vec_t(MemoryBuffer) MemoryBufferVec;

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit   = ev_renderer_deinit,

  .startFrame = ev_renderer_startframe,
  .endFrame = ev_renderer_endframe,

  .draw = ev_renderer_draw,

  .registerIndexBuffer = ev_renderer_registerindexbuffer,
  .registerVertexBuffer = ev_renderer_registervertexbuffer,
  .registerMaterialBuffer = ev_renderer_registermaterialbuffer,
};

struct ev_Renderer_Data {
  MemoryPool resourcePool;

  MemoryBufferVec indexBuffers;
  MemoryBufferVec vertexBuffers;
  MemoryBuffer materialBuffer;

  UBO cameraUBO;
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
  MemoryBuffer *buffer; unsigned int idx;
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

  // After freeing all the buffers in this pool, free the pool itself
  RendererBackend.freeMemoryPool(RendererData.resourcePool);

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

static void ev_renderer_registermaterialbuffer(Material* materials, unsigned long long size) 
{
    MemoryBuffer newMaterialBuffer;
    RendererBackend.allocateBufferInPool(RendererData.resourcePool, size, EV_USAGEFLAGS_RESOURCE_BUFFER, &newMaterialBuffer);

    MemoryBuffer materialStagingBuffer;
    RendererBackend.allocateStagingBuffer(size, &materialStagingBuffer);
    RendererBackend.updateStagingBuffer(&materialStagingBuffer, size, materials);
    RendererBackend.copyBuffer(size, &materialStagingBuffer, &newMaterialBuffer);

    //TODO We should have a system that controls staging buffers
    RendererBackend.freeMemoryBuffer(&materialStagingBuffer);

    RendererData.materialBuffer = newMaterialBuffer;
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
  RendererBackend.pushDescriptorsToSet(cameraDescriptorSet, cameraDescriptors, ARRAYSIZE(cameraDescriptors));

  // Update Camera UBO
  RendererBackend.updateUBO(sizeof(ev_RenderCamera), camera, &RendererData.cameraUBO);

  // Create a descriptor set that contains all the resources needed by the shader
  DescriptorSet resourceDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_BUFFER_ARR, &resourceDescriptorSet);
  Descriptor *resourceDescriptors = malloc(sizeof(Descriptor) * RendererData.vertexBuffers.length);
  for(int i = 0; i < RendererData.vertexBuffers.length; ++i)
    resourceDescriptors[i] = (Descriptor){EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, &RendererData.vertexBuffers.data[i]};
  RendererBackend.pushDescriptorsToSet(resourceDescriptorSet, resourceDescriptors, RendererData.vertexBuffers.length);
  free(resourceDescriptors);

  DescriptorSet materialDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_BUFFER_MAT, &materialDescriptorSet);
  Descriptor materialDescriptor = 
  {
      EV_DESCRIPTOR_TYPE_STORAGE_BUFFER, &RendererData.materialBuffer.buffer 
  };
  RendererBackend.pushDescriptorsToSet(materialDescriptorSet, &materialDescriptor, 1);

  DescriptorSet descriptorSets[] = {
    cameraDescriptorSet,
    resourceDescriptorSet,
    materialDescriptorSet
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
      ev_Matrix4 modelMatrix;
    } params;
    params.vertexBufferIndex = currentPrimitive->vertexBufferId;
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