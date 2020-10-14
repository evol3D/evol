#ifndef EVOL_RENDERERBACKEND_H
#define EVOL_RENDERERBACKEND_H

#ifdef VULKAN_BACKEND
#include <Vulkan/Vulkan.h>

typedef VkShaderModule        ShaderModule;
typedef VkPipeline            GraphicsPipeline;
typedef VkDescriptorSetLayout DescriptorSetLayout;
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

    ShaderModule (*loadShader)(const char *);
    void (*unloadShader)(ShaderModule);

    void (*createResourceMemoryPool)(unsigned long long blockSize, unsigned int minBlockCount, unsigned int maxBlockCount, MemoryPool *pool);
    void (*allocateBufferInPool)(MemoryPool pool, unsigned long long bufferSize, unsigned long long usageFlags, MemoryBuffer *buffer);

    void (*memoryDump)();

    //TODO fix the need for these
    VkCommandBuffer (*getCurrentFrameCommandBuffer)();
    VkRenderPass (*getRenderPass)();
} RendererBackend;


#endif //EVOL_RENDERERBACKEND_H 
