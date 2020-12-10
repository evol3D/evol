#ifndef EVOL_MATERIAL_HEADER
#define EVOL_MATERIAL_HEADER

#include <cglm\types.h>
#include <vec.h>
#include <cgltf.h>
#include <stdint.h>

typedef struct
{
	uint32_t min;
	uint32_t mag;
} Sampler;
typedef struct
{
	uint32_t imageIndex;
	uint32_t samplerIndex;
} Texture;
typedef struct
{
	uint32_t albedoTexture;
	vec4 albdoFactor;

	uint32_t metalic_RoughnessTexture;
	float metalicFactor;
	float roughnessFactor;

	uint32_t normalTexture;
	float normalScale;

	uint32_t occlusionTexture;
	float occlusionStrength;

	uint32_t emissiveTexture;
	vec3 emissiveFactor;
} Material;

extern struct ev_MaterialSystem
{
	uint32_t(*init)();
	uint32_t(*deinit)();

	uint32_t (*registerGltfMaterial)(cgltf_material* m_gltf);
} MaterialSystem;

Material* get_material_array();
#endif