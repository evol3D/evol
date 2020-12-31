#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  int uvBufferIndex;
  int materialIndex;

  mat4 model;
};

layout(set = 0, binding = 0) uniform CameraParam {
  mat4 projection;
  mat4 view;
} Camera;

layout(set = 1, binding = 0) buffer VertexBuffer {
  layout(align = 16) vec3 vertices[];
} VertexBuffers[];

layout(set = 4, binding = 0) buffer NormalBuffer {
  layout(align = 16) vec3 normals[];
} NormalsBuffers[];

layout(set = 5, binding = 0) buffer uvBuffer {
  layout(align = 16) vec2 uvs[];
} UVBuffers[];

layout (location = 0) out vec2 outuv;

void main()
{

	vec3 a = NormalsBuffers[nonuniformEXT(0)].normals[gl_VertexIndex];
	outuv =  UVBuffers[nonuniformEXT(uvBufferIndex)].uvs[gl_VertexIndex];
	//vec3 pos = VertexBuffers[nonuniformEXT(vertexBufferIndex)].vertices[gl_VertexIndex].xyz;

	gl_Position = Camera.projection * Camera.view   * model * vec4(VertexBuffers[nonuniformEXT(vertexBufferIndex)].vertices[gl_VertexIndex], 1);
}