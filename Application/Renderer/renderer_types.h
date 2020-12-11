#ifndef EVOL_RENDERERTYPES_HEADER
#define EVOL_RENDERERTYPES_HEADER

#include <vec.h>
#include <types.h>

typedef enum
{
  EV_DESCRIPTOR_SET_LAYOUT_CAMERA_PARAM,
  EV_DESCRIPTOR_SET_LAYOUT_BUFFER_ARR,
  EV_DESCRIPTOR_SET_LAYOUT_BUFFER_MAT,
  EV_DESCRIPTOR_SET_LAYOUT_BUFFER_TEX,
  EV_DESCRIPTOR_SET_LAYOUT_BUFFER_NOR,
  EV_DESCRIPTOR_SET_LAYOUT_BUFFER_UV,

  // Add before this line
  DESCRIPTOR_SET_LAYOUT_COUNT
} DescriptorSetLayoutType;

typedef enum
{
  EV_BASE_SHADER_PBR_VERT, EV_BASE_SHADER_PBR_FRAG,
  EV_BASE_SHADER_DUMMY_VERT, EV_BASE_SHADER_DUMMY_FRAG,

  // Add before this line
  EV_BASE_SHADER_COUNT
} EvBaseShaderIdentifiers;

typedef enum
{
  EV_GRAPHICS_PIPELINE_BASE,
  EV_GRAPHICS_PIPELINE_PBR,

  // Add before this line
  GRAPHICS_PIPELINES_COUNT
} GraphicsPipelineType;

typedef struct {
  unsigned int indexBufferId;
  unsigned int vertexBufferId;
  unsigned int normalBufferId;
  unsigned int uvBufferId;

  unsigned int indexCount;
} PrimitiveRenderData;

typedef struct {
  ev_Matrix4 projectionMatrix;
  ev_Matrix4 viewMatrix;
} ev_RenderCamera;

/* typedef vec_t(PrimitiveRenderData) MeshRenderData; */
typedef struct {
  vec_t(PrimitiveRenderData) primitives;
  GraphicsPipelineType pipelineType;
} MeshRenderData;

#endif
