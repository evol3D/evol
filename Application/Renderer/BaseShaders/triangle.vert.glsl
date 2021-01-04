#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  int normalBufferIndex;
  int uvBufferIndex;

  mat4 model;
} RenderData;

layout(set = 0, binding = 0) uniform CameraParam {
  mat4 projection;
  mat4 view;
} Camera;

layout(set = 1, binding = 0) buffer VertexBuffer {
  layout(align = 16) vec3 vertices[];
} VertexBuffers[];

layout(set = 1, binding = 1) buffer NormalBuffer {
  layout(align = 16) vec3 normals[];
} NormalBuffers[];

layout(set = 1, binding = 2) buffer UVBuffer {
  layout(align = 16) vec4 uvs[];
} UVBuffers[];

//layout(location=0) out vec4 color;
layout(location=0) out vec2 uv;

vec3 lightDir = vec3(1, 0, 1);
float ambientLight = 0.1;

void main()
{
  gl_Position = Camera.projection * Camera.view * RenderData.model * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);

  //vec3 normalizedLight = normalize(lightDir);
  //vec3 normal = NormalBuffers[nonuniformEXT(RenderData.normalBufferIndex)].normals[gl_VertexIndex];
  //vec3 rotatedNormal = vec3(RenderData.model * vec4(normal, 0));
  //float lightIntensity = dot(normalizedLight, normalize(rotatedNormal)) + ambientLight;

  //color = vec4(1, 1, 1, 1) * lightIntensity;
  uv = UVBuffers[nonuniformEXT(RenderData.uvBufferIndex)].uvs[gl_VertexIndex].xy;
}
