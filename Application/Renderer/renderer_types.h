#ifndef EVOL_RENDERERTYPES_HEADER
#define EVOL_RENDERERTYPES_HEADER

typedef enum
{
  EV_DESCRIPTOR_SET_LAYOUT_TEXTURE,
  EV_DESCRIPTOR_SET_LAYOUT_RIG,

  // Add before this line
  DESCRIPTOR_SET_LAYOUT_COUNT,
} DescriptorSetLayoutType;

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
} GraphicsPipelineType;

#endif
