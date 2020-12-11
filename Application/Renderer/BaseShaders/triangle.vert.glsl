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

layout(set = 5, binding = 0) buffer uvBuffer {
  layout(align = 16) vec2 uvs[];
} UVBuffers[];

layout (location = 0) out vec2 outuv;

void main()
{
  outuv =  UVBuffers[nonuniformEXT(0)].uvs[gl_VertexIndex];
  //vec3 pos = VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex].xyz;
  
 gl_Position = Camera.projection * Camera.view * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);
//gl_Position =  Camera.projection * Camera.view * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);
}