#include <Asset/material.h>

#include "Renderer/Renderer.h"



uint32_t material_system_init();
uint32_t material_system_deinit();

uint32_t material_system_register_GLTF_material(cgltf_material* m_gltf);

material_vec_t material_system_get_materials();
texture_vec_t  material_system_get_textures ();
sampler_vec_t  material_system_get_samplers ();
vec_str_t      material_system_get_images   ();

struct ev_MaterialSystem MaterialSystem = 
{
  .init = material_system_init,
  .deinit = material_system_deinit,

  .registerGltfMaterial = material_system_register_GLTF_material,

  .getMaterials = material_system_get_materials,
  .getTextures  = material_system_get_textures,
  .getSamplers  = material_system_get_samplers,
  .getImages    = material_system_get_images,
};
struct ev_MaterialSystem_Data {
	vec_str_t      images;
	sampler_vec_t  samplers;
	texture_vec_t  textures;
	material_vec_t materials;
} MaterialSystemData;

uint32_t material_system_register_image(const char* imageuri);
uint32_t material_system_register_sampler(int mag, int min);
uint32_t material_system_register_texture(uint32_t samplerIdx, uint32_t imageIdx);
uint32_t material_system_register_material(Material* newMaterial);

uint32_t create_image(const char* imageuri);
uint32_t create_sampler(int mag, int min);
uint32_t create_texture(uint32_t samplerIdx, uint32_t imageIdx);
uint32_t create_material(Material* newMaterial);

uint32_t material_system_register_GLTF_texture(cgltf_texture* t);










uint32_t material_system_init()
{
	vec_init(&MaterialSystemData.images);
	vec_init(&MaterialSystemData.samplers);
	vec_init(&MaterialSystemData.textures);
	vec_init(&MaterialSystemData.materials);
}
uint32_t material_system_deinit()
{
	vec_deinit(&MaterialSystemData.images);
	vec_deinit(&MaterialSystemData.samplers);
	vec_deinit(&MaterialSystemData.textures);
	vec_deinit(&MaterialSystemData.materials);
}

uint32_t material_system_register_GLTF_material(cgltf_material* m_gltf) 
{
	Material m =
	{
		.albedoTexture = material_system_register_GLTF_texture(m_gltf->pbr_metallic_roughness.base_color_texture.texture),
		.albdoFactor[0] = m_gltf->pbr_metallic_roughness.base_color_factor[0],
		.albdoFactor[1] = m_gltf->pbr_metallic_roughness.base_color_factor[1],
		.albdoFactor[2] = m_gltf->pbr_metallic_roughness.base_color_factor[2],
		.albdoFactor[3] = m_gltf->pbr_metallic_roughness.base_color_factor[3],

		.metalic_RoughnessTexture = material_system_register_GLTF_texture(m_gltf->pbr_metallic_roughness.metallic_roughness_texture.texture),
		.metalicFactor = m_gltf->pbr_metallic_roughness.metallic_factor,
		.roughnessFactor = m_gltf->pbr_metallic_roughness.roughness_factor,

		.normalTexture = material_system_register_GLTF_texture(m_gltf->normal_texture.texture),
		.normalScale = m_gltf->normal_texture.scale,

		.occlusionTexture = material_system_register_GLTF_texture(m_gltf->occlusion_texture.texcoord),
		.occlusionStrength = m_gltf->occlusion_texture.scale,

		.emissiveTexture = material_system_register_GLTF_texture(m_gltf->emissive_texture.texture),
		.emissiveFactor[0] = m_gltf->emissive_factor[0],
		.emissiveFactor[1] = m_gltf->emissive_factor[1],
		.emissiveFactor[2] = m_gltf->emissive_factor[2],
	};

	return material_system_register_material(&m);
}
uint32_t material_system_register_GLTF_texture(cgltf_texture* t) 
{
	if (!t)
		return ~0u;

	cgltf_sampler* sampler = t->sampler;
	cgltf_image* image = t->image;

	uint32_t samplerIdx;
	{
		if (!sampler)
			samplerIdx = material_system_register_sampler(0, 0);
		else
			samplerIdx = material_system_register_sampler(sampler->mag_filter, sampler->min_filter);
	}
	//printf("\n%s\n", image->uri);
	uint32_t imageIdx = material_system_register_image(image->uri);
	
	int idx; char* val;
	vec_foreach(&MaterialSystemData.images, val, idx) {
	
		printf("\n%s\n",val);
	}

	return material_system_register_texture(samplerIdx, imageIdx);
}

uint32_t material_system_register_texture(uint32_t samplerIdx, uint32_t imageIdx) 
{
	int idx; Texture* val;
	vec_foreach_ptr(&MaterialSystemData.textures, val, idx)
	{
		if (val->imageIndex == imageIdx && val->samplerIndex == samplerIdx)
			return idx;
	}

	return create_texture(samplerIdx, imageIdx);
}
uint32_t material_system_register_image(const char *imageuri)
{
	int idx; char *val;
	vec_foreach(&MaterialSystemData.images, val, idx)
	{
		if (!strcmp(imageuri, val))
			return idx;
	}

	return create_image(imageuri);
}
uint32_t material_system_register_sampler(int mag, int min)
{
	int idx; Sampler *val;
	vec_foreach_ptr(&MaterialSystemData.samplers, val, idx)
	{
		if (val->mag == mag && val->min == min)
			return idx;
	}

	return create_sampler(mag, min);
}
uint32_t material_system_register_material(Material *newMaterial)
{
	int idx; Material* val;
	vec_foreach_ptr(&MaterialSystemData.materials, val, idx)
	{
		if (val->albdoFactor == newMaterial->albdoFactor && val->albedoTexture == newMaterial->albedoTexture &&
			val->emissiveFactor == newMaterial->emissiveFactor && val->emissiveTexture == newMaterial->emissiveTexture &&
			val->metalicFactor == newMaterial->metalicFactor && val->roughnessFactor == newMaterial->roughnessFactor && val->metalic_RoughnessTexture == newMaterial->metalic_RoughnessTexture &&
			val->normalScale == newMaterial->normalScale && val->normalTexture == newMaterial->normalTexture &&
			val->occlusionStrength == newMaterial->occlusionStrength && val->occlusionTexture == newMaterial->occlusionTexture)
			return idx;
	}

	return create_material(newMaterial);
}

uint32_t create_texture(uint32_t samplerIdx, uint32_t imageIdx) {
	uint32_t idx = MaterialSystemData.textures.length;

	Texture t = {
		.samplerIndex = samplerIdx,
		.imageIndex = imageIdx
	};
	vec_push(&MaterialSystemData.textures, t);

	Renderer.registertexture(&t);

	return idx;
}
uint32_t create_image(const char* imageuri)
{
	uint32_t idx = MaterialSystemData.images.length;

	char* img = strdup(imageuri);
	vec_push(&MaterialSystemData.images, img);

	return idx;
}
uint32_t create_sampler(int mag, int min)
{
	uint32_t idx = MaterialSystemData.samplers.length;

	Sampler s = {
		.mag = mag,
		.min = min
	};
	vec_push(&MaterialSystemData.samplers, s);
		
	return idx;
}
uint32_t create_material(Material* newMaterial)
{
	uint32_t idx = MaterialSystemData.materials.length;

	vec_push(&MaterialSystemData.materials, *newMaterial);

	return idx;
}

material_vec_t material_system_get_materials() {
	return MaterialSystemData.materials;
}
texture_vec_t material_system_get_textures() {
	return MaterialSystemData.textures;
}
sampler_vec_t material_system_get_samplers() {
	return MaterialSystemData.samplers;
}
vec_str_t material_system_get_images() {
	return MaterialSystemData.images;
}