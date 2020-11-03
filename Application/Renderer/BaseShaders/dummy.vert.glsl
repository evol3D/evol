#version 450

#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  mat4 model;
} RenderData;

layout(set = 0, binding = 0) uniform CameraParam {
  mat4 projection;
  mat4 view;
} Camera;

layout(set = 1, binding = 0) buffer VertexBuffer {
  layout(align = 16) vec3 vertices[];
} VertexBuffers[];


layout(location=0) out vec4 color;

void main()
{
  gl_Position = Camera.projection * Camera.view * RenderData.model * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);
  color = vec4(abs(Camera.view[3][2] - RenderData.model[3][2])  * vec3(1, 1, 1) * 0.1, 1);
}
