#ifndef EVOL_RENDERERBACKEND_H
#define EVOL_RENDERERBACKEND_H

// TODO this shouldn't be here
#include <Vulkan/Vulkan.h>

extern struct ev_RendererBackend {
    int (*init)();
    int (*deinit)();
    void (*startNewFrame)();
    void (*endFrame)();

    //TODO fix the need for these
    VkCommandBuffer (*getCurrentFrameCommandBuffer)();
    VkRenderPass (*getRenderPass)();
} RendererBackend;

#ifdef VULKAN_BACKEND
#include <Vulkan/Vulkan.h>

typedef VkShaderModule        ShaderModule;
typedef VkPipeline            GraphicsPipeline;
typedef VkDescriptorSetLayout DescriptorSetLayout;
typedef VkRenderPass          RenderPass;

#endif

#endif //EVOL_RENDERERBACKEND_H 
