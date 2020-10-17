#ifndef EVOL_RENDERERBACKEND_H
#define EVOL_RENDERERBACKEND_H

#include "renderer_types.h"

#ifdef VULKAN_BACKEND
#include <Vulkan.h>

typedef VkShaderModule        ShaderModule;
typedef VkPipeline            GraphicsPipeline;
typedef VkDescriptorSetLayout DescriptorSetLayout;
typedef VkDescriptorSet       DescriptorSet;
typedef VkRenderPass          RenderPass;

typedef EvBuffer              MemoryBuffer;
typedef EvImage               MemoryImage;
typedef VmaPool               MemoryPool;
typedef unsigned int          MemoryType;

#define EV_USAGEFLAGS_RESOURCE_BUFFER VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT

#endif

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

    int (*allocateDescriptorSet)(DescriptorSetLayoutType setLayoutType, DescriptorSet *descriptorSet);


    ShaderModule (*loadShader)(const char *);
    void (*unloadShader)(ShaderModule);

    void (*createResourceMemoryPool)(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool);
    void (*allocateBufferInPool)(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer);

    void (*allocateStagingBuffer)(unsigned long long bufferSize, MemoryBuffer *buffer);
    void (*updateStagingBuffer)(MemoryBuffer *buffer, unsigned long long bufferSize, const void *data);

    void (*copyBuffer)(unsigned long long size, MemoryBuffer *src, MemoryBuffer *dst);

    void (*memoryDump)();

    //TODO fix the need for these
    VkCommandBuffer (*getCurrentFrameCommandBuffer)();
    VkRenderPass (*getRenderPass)();
} RendererBackend;


#endif //EVOL_RENDERERBACKEND_H 
