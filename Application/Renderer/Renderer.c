#include "Renderer.h"
#include "Vulkan/Vulkan.h"

static int ev_renderer_init();
static int ev_renderer_deinit();
static void ev_renderer_prepare_descriptorset_layouts();
static void ev_renderer_prepare_graphics_pipelines();

static inline void ev_renderer_load_base_shaders();
static inline void ev_renderer_unload_base_shaders();

static void ev_renderer_bind();

#define SET_SHADER(id, path) RendererData.baseShaderPaths[id] = path;

typedef enum
{
  EV_DESCRIPTOR_SET_LAYOUT_TEXTURE,
  EV_DESCRIPTOR_SET_LAYOUT_RIG,

  // Add before this line
  DESCRIPTOR_SET_LAYOUT_COUNT,
} DescriptorSetLayoutUsage;

typedef enum
{
  EV_BASE_SHADER_PBR_VERT, EV_BASE_SHADER_PBR_FRAG,

  // Add before this line
  EV_BASE_SHADER_COUNT
} EvBaseShaderIdentifiers;

typedef enum
{
  EV_GRAPHICS_PIPELINE_PBR,

  // Add before this line
  GRAPHICS_PIPELINES_COUNT,
} GraphicsPipelineUsage;

struct ev_Renderer Renderer = {
  .init   = ev_renderer_init,
  .deinit = ev_renderer_deinit,
};

struct ev_Renderer_Data {
  VkDescriptorSetLayout descriptorSetLayouts[DESCRIPTOR_SET_LAYOUT_COUNT];
  VkPipeline graphicsPipelines[GRAPHICS_PIPELINES_COUNT];
  const char * baseShaderPaths[EV_BASE_SHADER_COUNT];
  VkShaderModule baseShaders[EV_BASE_SHADER_COUNT];
  unsigned int bufferCount;
} RendererData;

static int ev_renderer_init()
{
  RendererData.bufferCount = 3;
  Vulkan.createSwapchain(&RendererData.bufferCount);

  ev_renderer_prepare_descriptorset_layouts();

  ev_renderer_load_base_shaders();
  ev_renderer_prepare_graphics_pipelines();

  Vulkan.startNewFrame();
  ev_renderer_bind();
  Vulkan.endFrame();

  return 0;
}

static int ev_renderer_deinit()
{
  ev_renderer_unload_base_shaders();

  Vulkan.destroySwapchain();
  return 0;
}

static void ev_renderer_prepare_descriptorset_layouts()
{
  //SET 0 FOR TEXTURES
  {
    VkDescriptorSetLayoutBinding bindings[] =
    { 
      {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = NULL,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
      }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = ARRAYSIZE(bindings),
      .pBindings = bindings
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan.getDevice(), &descriptorSetLayoutCreateInfo, NULL, &RendererData.descriptorSetLayouts[EV_DESCRIPTOR_SET_LAYOUT_TEXTURE]));
  }

   //SET 1 FOR rigging
  {
    VkDescriptorSetLayoutBinding bindings[] =
    { 
      {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImmutableSamplers = NULL,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
      }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = ARRAYSIZE(bindings),
      .pBindings = bindings
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan.getDevice(), &descriptorSetLayoutCreateInfo, NULL, &RendererData.descriptorSetLayouts[EV_DESCRIPTOR_SET_LAYOUT_RIG]));
  }
}

static void ev_renderer_prepare_graphics_pipelines()
{
  //TODO when you implement multiple pipelines take a look at input attributes , this pipeline should be a rigging pipeline
  VkPipeline simplePipeline;
  VkPipelineLayout pipelineLayout;

  VkPipelineShaderStageCreateInfo pipelineShaderStages[] = 
  {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	  .stage = VK_SHADER_STAGE_VERTEX_BIT,
	  .module = RendererData.baseShaders[EV_BASE_SHADER_PBR_VERT],
	  .pName = "main"
	},
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	  .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
	  .module = RendererData.baseShaders[EV_BASE_SHADER_PBR_FRAG],
	  .pName = "main"
	},
  };

	VkVertexInputBindingDescription BindingDescriptions[] =
	{
		// { 0 , 3 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //positions
		// { 1 , 3 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //normals
		// { 2 , 2 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //uv0
		// { 3 , 2 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //uv1
		// { 4 , 4 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //joints
		// { 5 , 4 * sizeof(float) , VK_VERTEX_INPUT_RATE_VERTEX }, //weights
	};

	VkVertexInputAttributeDescription InputAttributeDescrptions[] =
	{
		// { 0 , 0 , VK_FORMAT_R32G32B32_SFLOAT , 0 },
		// { 1 , 1 , VK_FORMAT_R32G32B32_SFLOAT , 0 },
		// { 2 , 2 , VK_FORMAT_R32G32_SFLOAT , 0 },
		// { 3 , 3 , VK_FORMAT_R32G32_SFLOAT , 0 },
		// { 4 , 4 , VK_FORMAT_R32G32B32A32_SFLOAT , 0 },
		// { 5 , 5 , VK_FORMAT_R32G32B32A32_SFLOAT , 0 }
	};

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = ARRAYSIZE(BindingDescriptions),
		.pVertexBindingDescriptions = BindingDescriptions,
		.vertexAttributeDescriptionCount = ARRAYSIZE(InputAttributeDescrptions),
		.pVertexAttributeDescriptions = InputAttributeDescrptions
	};

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	VkPipelineViewportStateCreateInfo pipelineViewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1,
	};

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.lineWidth = 1.0,
	};

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
		.depthBoundsTestEnable = VK_FALSE,
		.back = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS
		},
		.front = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS
		},
		.stencilTestEnable = VK_FALSE
	};

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
		.colorWriteMask = 
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_A_BIT ,
	};

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &pipelineColorBlendAttachmentState,
	};

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo pipelineDynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = ARRAYSIZE(dynamicStates),
		.pDynamicStates = dynamicStates,
	};

	//push constants
	VkPushConstantRange pc = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = 96 //TODO sizeof(push constant struct)
	};

  VkDescriptorSetLayout setLayouts[] = {
    RendererData.descriptorSetLayouts[EV_DESCRIPTOR_SET_LAYOUT_RIG],
    RendererData.descriptorSetLayouts[EV_DESCRIPTOR_SET_LAYOUT_TEXTURE],
  };

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = ARRAYSIZE(setLayouts),
		.pSetLayouts = setLayouts,

		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pc
	};

	VK_ASSERT(vkCreatePipelineLayout(Vulkan.getDevice(), &pipelineLayoutCreateInfo, NULL, &pipelineLayout));

  // The graphicsPipelinesCreateInfos array should follow the order set by
  // the GraphicsPipelineUsage enum.
	VkGraphicsPipelineCreateInfo graphicsPipelinesCreateInfos[] = 
  { 
    {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = ARRAYSIZE(pipelineShaderStages),
      .pStages = pipelineShaderStages,
      .pVertexInputState = &pipelineVertexInputState,
      .pInputAssemblyState = &pipelineInputAssemblyState,
      .pViewportState = &pipelineViewportState,
      .pRasterizationState = &pipelineRasterizationState,
      .pMultisampleState = &pipelineMultisampleState,
      .pDepthStencilState = &pipelineDepthStencilState,
      .pColorBlendState = &pipelineColorBlendState,
      .pDynamicState = &pipelineDynamicState,
      .layout = pipelineLayout,
      .renderPass = Vulkan.getRenderPass(),
      .subpass = 0, // TODO Read more about the graphics pipelines and what this number represents (first subpass to run?)
    } 
  };

	VK_ASSERT(
    vkCreateGraphicsPipelines(
      Vulkan.getDevice(), NULL, 
      ARRAYSIZE(graphicsPipelinesCreateInfos), 
      graphicsPipelinesCreateInfos, NULL, 
      RendererData.graphicsPipelines));
}

static inline void ev_renderer_load_base_shaders()
{
  #include "baseshaderpaths.txt"

  for(int i = 0; i < EV_BASE_SHADER_COUNT; ++i)
  {
    RendererData.baseShaders[i] = Vulkan.loadShader(RendererData.baseShaderPaths[i]);
  }
}

static inline void ev_renderer_unload_base_shaders()
{
  for(int i = 0; i < EV_BASE_SHADER_COUNT; ++i)
  {
    Vulkan.unloadShader(RendererData.baseShaders[i]);
  }
}

static void ev_renderer_bind()
{
  //TODO look into using secondary command buffer and recording for every pipeline

  //TODO LOOK MORE INTO THIS
  vkCmdBindPipeline(Vulkan.getCurrentFrameCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, RendererData.graphicsPipelines[EV_GRAPHICS_PIPELINE_PBR]);

  //Also not sure how we will handle this. but there it is -- will we have one big buffer with all meshes and we will cherry pick from it for every pipeline'
  //or will we have specific buffer or each type of mesh that requires a specific type of pipeline and bind it seperatly.
  //TODO LOOK FURTHER INTO THIS
  {
    // VkBuffer vertexBuffers[] = { 0 };
    // VkDeviceSize offsets[] = { 0 };
    // vkCmdBindVertexBuffers(Vulkan.getCurrentFrameCommandBuffer(), 0, ARRAYSIZE(vertexBuffers), vertexBuffers, offsets);
    //TODO SUPPORT HAVING A MODEL WITH NO VERTEX BUFFER ?
    //vkCmdBindIndexBuffer(Vulkan.getCurrentFrameCommandBuffer(), indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
  }

  //TODO DRAW LOOP HERE nad also look into supporting shaders with no indexes
  {
    //vkCmdDrawIndexed(Vulkan.getCurrentFrameCommandBuffer(), indices_counts[TYPE][i], 1, indices_offsets[TYPE][i], vertices_offsets[TYPE][i], 0);
    vkCmdDraw(Vulkan.getCurrentFrameCommandBuffer(), 3, 1, 0, 0);
  }
}