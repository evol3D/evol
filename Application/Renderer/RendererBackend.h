#ifndef EVOL_RENDERERBACKEND_H
#define EVOL_RENDERERBACKEND_H

#include "renderer_types.h"
#include "stdbool.h"

#ifdef VULKAN_BACKEND
# include <Vulkan.h>

  typedef VkShaderModule        ShaderModule;
  typedef VkPipeline            GraphicsPipeline;
  typedef VkDescriptorSetLayout DescriptorSetLayout;
  typedef VkDescriptorSet       DescriptorSet;
  typedef VkRenderPass          RenderPass;

  typedef EvBuffer              MemoryBuffer;
  typedef EvImage               MemoryImage;
  typedef VmaPool               MemoryPool;
  typedef unsigned int          MemoryType;

# define EV_USAGEFLAGS_RESOURCE_BUFFER          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
# define EV_BUFFER_USAGE_INDEX_BUFFER_BIT       VK_BUFFER_USAGE_INDEX_BUFFER_BIT   | VK_BUFFER_USAGE_TRANSFER_DST_BIT

  typedef enum DescriptorType {
     EV_DESCRIPTOR_TYPE_SAMPLER                = VK_DESCRIPTOR_TYPE_SAMPLER,
     EV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
     EV_DESCRIPTOR_TYPE_SAMPLED_IMAGE          = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
     EV_DESCRIPTOR_TYPE_STORAGE_IMAGE          = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
     EV_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER   = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
     EV_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER   = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
     EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
     EV_DESCRIPTOR_TYPE_STORAGE_BUFFER         = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
     EV_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
     EV_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
     EV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT       = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
  } DescriptorType;

#endif

typedef struct Descriptor
{
DescriptorType type;
void *descriptorData;
} Descriptor;

typedef struct UBO
{
    MemoryBuffer buffer;
    void *mappedData;
} UBO;

extern struct ev_RendererBackend {
    int (*init)();
    int (*deinit)();
    void (*startNewFrame)();
    void (*endFrame)();

    int (*loadBasePipelines)();
    int (*unloadBasePipelines)();


    int (*loadBaseShaders)();
    int (*unloadBaseShaders)();

    int (*loadBaseDescriptorSetLayouts)();
    int (*unloadBaseDescriptorSetLayouts)();

    int (*bindPipeline)(GraphicsPipelineType type);

    int (*bindDescriptorSets)(DescriptorSet *descriptorSets, unsigned int count);

    int (*bindIndexBuffer)(MemoryBuffer *indexBuffer);

    int (*allocateDescriptorSet)(DescriptorSetLayoutType setLayoutType, DescriptorSet *descriptorSet);

    int (*pushDescriptorsToSet)(DescriptorSet descriptorSet, Descriptor *descriptors, unsigned int descriptorsCount);


    ShaderModule (*loadShader)(const char *);
    void (*unloadShader)(ShaderModule);

    void (*createResourceMemoryPool)(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool);
    void (*freeMemoryPool)(MemoryPool pool);
    void (*allocateBufferInPool)(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer);
    void (*freeMemoryBuffer)(MemoryBuffer *buffer);

    void (*allocateStagingBuffer)(unsigned long long bufferSize, MemoryBuffer *buffer);
    void (*updateStagingBuffer)(MemoryBuffer *buffer, unsigned long long bufferSize, const void *data);

    void (*allocateUBO)(unsigned long long bufferSize, bool persistentMap, UBO *ubo);
    void (*updateUBO)(unsigned long long size, const void *data, UBO *ubo);
    void (*freeUBO)(UBO *ubo);

    void (*copyBuffer)(unsigned long long size, MemoryBuffer *src, MemoryBuffer *dst);

    void (*memoryDump)();

    void (*pushConstant)(void *data, unsigned int size);

    void (*drawIndexed)(unsigned int indexCount);

} RendererBackend;


#endif //EVOL_RENDERERBACKEND_H 
