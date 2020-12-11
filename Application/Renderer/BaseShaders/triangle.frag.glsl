#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 inuv;
layout(location = 0) out vec4 outputColor;

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

layout(set = 2, binding = 0) buffer materials
{
    material m[];
};

layout(set = 3, binding = 0) uniform sampler2D texSampler[];

layout(set = 5, binding = 0) buffer uvBuffer {
  layout(align = 16) vec2 uvs[];
} UVBuffers[];

void main()
{
	vec2 a = vec2(0,0);
    outputColor = vec4(texture(texSampler[m[0].albedoTexture],inuv).xyz,1);
}