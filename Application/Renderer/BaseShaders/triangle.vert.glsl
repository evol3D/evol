#version 450

#extension GL_EXT_nonuniform_qualifier : require

struct material
{
	uint albedoTexture;
	vec4 albdoFactor;

	uint metalic_RoughnessTexture;
	float metalicFactor;
	float roughnessFactor;

	uint normalTexture;
	float normalScale;

	uint occlusionTexture;
	float occlusionStrength;

	uint emissiveTexture;
	vec3 emissiveFactor;
};

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

layout(set = 2, binding = 0) buffer materials
{
    material m[];
};

layout(set = 3, binding = 0) uniform sampler2D texSampler[];

layout(location=0) out vec4 color;

void main()
{
	material m1 = m[0];
	vec2 a = vec2(0,0);
	texture(texSampler[0], a);
  gl_Position = Camera.projection * Camera.view * RenderData.model * vec4(VertexBuffers[nonuniformEXT(RenderData.vertexBufferIndex)].vertices[gl_VertexIndex], 1);
  color = gl_Position;
}
