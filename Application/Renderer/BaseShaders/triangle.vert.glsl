#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  int normalBufferIndex;
  int uvBufferIndex;

  int materialIndex;

  mat4 model;
} RenderData;

layout(set = 0, binding = 0) uniform CameraParam {
  mat4 projection;
  mat4 view;
} Camera;

layout(set = 1, binding = 0) buffer ResourceBuffer {
  layout(align = 16) vec4 resources[];
} ResourceBuffers[];

layout(location=0) out vec3 pos;
layout(location=1) out vec3 nor;
layout(location=2) out vec2 uv;

void main()
{
  pos = vec3(RenderData.model * vec4(ResourceBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].resources[gl_VertexIndex].xyz, 1.0));
  nor = ResourceBuffers[nonuniformEXT(RenderData.normalBufferIndex)].resources[gl_VertexIndex].xyz;
  uv  = ResourceBuffers[nonuniformEXT(RenderData.uvBufferIndex)].resources[gl_VertexIndex].xy;

  gl_Position = Camera.projection * Camera.view * vec4(pos,1);
}
