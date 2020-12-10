#ifndef EVOL_MATERIAL_HEADER
#define EVOL_MATERIAL_HEADER

#include <cglm/types.h>
#include <stdint.h>
#include <cgltf.h>
#include <vec.h>

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

typedef vec_t(Sampler)  sampler_vec_t;
typedef vec_t(Texture)  texture_vec_t;
typedef vec_t(Material) material_vec_t;

extern struct ev_MaterialSystem
{
	uint32_t(*init)();
	uint32_t(*deinit)();

	uint32_t (*registerGltfMaterial)(cgltf_material* m_gltf);

	material_vec_t(*getMaterials)();
	texture_vec_t (*getTextures) ();
	sampler_vec_t (*getSamplers) ();
	vec_str_t	  (*getImages)   ();
} MaterialSystem;
#endif