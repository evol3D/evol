#version 450

#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;

  mat4 model;
} RenderData;

layout(set = 0, binding = 0) buffer VertexBuffer {
  layout(align = 16) vec3 vertices[];
} VertexBuffers[];

void main()
{
  gl_Position = RenderData.model * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);
}
