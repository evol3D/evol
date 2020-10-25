#version 450

#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
} RenderData;

layout(set = 0, binding = 0) buffer VertexBuffer {
  layout(align = 16) vec3 vertices[];
} VertexBuffers[];

void main() 
{
  gl_Position = vec4(VertexBuffers[RenderData.vertexBufferIndex].vertices[gl_VertexIndex], 1);
}
