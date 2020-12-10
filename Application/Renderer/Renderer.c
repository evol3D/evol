#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
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
static void ev_renderer_registertexturebuffer(Texture* texture);
static void ev_renderer_registerimagebuffer(uint32_t imageIndex, EvImage* newImageBuffer, VkImageView* imageView);


stbi_uc* loadImage(char* image, int* width, int* height, int* channel);

typedef vec_t(MemoryBuffer) MemoryBufferVec;
typedef vec_t(EvTexture) MemoryTextureVec;

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit   = ev_renderer_deinit,

  .startFrame = ev_renderer_startframe,
  .endFrame = ev_renderer_endframe,

  .draw = ev_renderer_draw,

  .registerIndexBuffer = ev_renderer_registerindexbuffer,
  .registerVertexBuffer = ev_renderer_registervertexbuffer,
  .registerMaterialBuffer = ev_renderer_registermaterialbuffer,
  .registerImageslBuffer = ev_renderer_registerimagebuffer,

  .registertexture = ev_renderer_registertexturebuffer
};

struct ev_Renderer_Data {
  MemoryPool resourcePool;
  MemoryPool TexturePool;

  MemoryBufferVec indexBuffers;
  MemoryBufferVec vertexBuffers;
  MemoryBuffer materialBuffer;
  MemoryTextureVec textureBuffers;

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

  ev_log_trace("Allocating TextureMemoryPool");
  RendererBackend.createTextureMemoryPool(128ull * 1024 * 1024, 1, 4, &RendererData.TexturePool);
  ev_log_debug("Allocated TextureMemoryPool");

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

static void ev_renderer_registertexturebuffer(Texture *texture) 
{
    EvImage newImageBuffer;
    VkImageView imageView;
    ev_renderer_registerimagebuffer(texture->imageIndex, &newImageBuffer, &imageView);

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
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f
    };
    vkCreateSampler(Vulkan.getDevice(), &samplerInfo, NULL, &sampler);

    EvTexture evtexture =
    {
        .image = newImageBuffer,
        .imageView = imageView,
        .sampler = sampler
    };
    vec_push(&RendererData.textureBuffers, evtexture);
}

static void ev_renderer_registerimagebuffer(uint32_t imageIndex, EvImage* newImageBuffer, VkImageView* imageView)
{
    {
        char* image = MaterialSystem.getImages().data[imageIndex];

        int width = 0, height = 0, channel = 0;
        void* pixels = loadImage(image, &width, &height, &channel);
        VkDeviceSize imageSize = width * height * 4;

        MemoryBuffer imageStagingBuffer;
        RendererBackend.allocateStagingBuffer(imageSize, &imageStagingBuffer);
        RendererBackend.updateStagingBuffer(&imageStagingBuffer, imageSize, pixels);

        RendererBackend.allocateImageInPool(RendererData.TexturePool, width, height, EV_IMAGE_USAGE_RESOURCE_BUFFER, newImageBuffer);

        RendererBackend.transitionImageLayout(newImageBuffer->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        RendererBackend.copyBufferToImage(imageStagingBuffer.buffer, newImageBuffer->image, width, height);
        RendererBackend.transitionImageLayout(newImageBuffer->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        RendererBackend.freeMemoryBuffer(&imageStagingBuffer);
        free(pixels);
    }

    RendererBackend.createImageView(VK_FORMAT_R8G8B8A8_SRGB, newImageBuffer->image, imageView);
}
stbi_uc* loadImage(char *image,int *width, int *height, int *channel)
{
    stbi_uc* pixels = stbi_load(image, width, height, channel, STBI_rgb_alpha);
    assert(pixels);
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

    //textures
  DescriptorSet textureDescriptorSet;
  RendererBackend.allocateDescriptorSet(EV_DESCRIPTOR_SET_LAYOUT_BUFFER_TEX, &textureDescriptorSet);
  Descriptor* textureDescriptors = malloc(sizeof(Descriptor) * RendererData.textureBuffers.length);
  for (int i = 0; i < RendererData.textureBuffers.length; ++i)      
      textureDescriptors[i] = (Descriptor){ EV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &RendererData.textureBuffers.data[i] };
  RendererBackend.pushDescriptorsToSet(textureDescriptorSet, textureDescriptors, RendererData.textureBuffers.length);
  free(textureDescriptors);

  DescriptorSet descriptorSets[] = {
    cameraDescriptorSet,
    resourceDescriptorSet,
    materialDescriptorSet,
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