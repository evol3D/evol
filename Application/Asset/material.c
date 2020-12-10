#include <Asset/material.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "Renderer/Renderer.h"

typedef vec_t(Sampler)  sampler_vec_t;
typedef vec_t(Texture)  texture_vec_t;
typedef vec_t(Material) material_vec_t;

vec_str_t      images;
sampler_vec_t  samplers;
texture_vec_t  textures;
material_vec_t materials;

uint32_t material_system_init();
uint32_t material_system_deinit();

uint32_t material_system_register_image(const char* imageuri);
uint32_t material_system_register_sampler(int mag, int min);
uint32_t material_system_register_texture(uint32_t samplerIdx, uint32_t imageIdx);
uint32_t material_system_register_material(Material* newMaterial);

uint32_t create_image(const char* imageuri);
uint32_t create_sampler(int mag, int min);
uint32_t create_texture(uint32_t samplerIdx, uint32_t imageIdx);
uint32_t create_material(Material* newMaterial);

struct ev_MaterialSystem MaterialSystem = {
  .init = material_system_init,
  .deinit = material_system_deinit,

  .registerImage = material_system_register_image,
  .registerSampler = material_system_register_sampler,
  .registerTexture = material_system_register_texture,
  .registerMaterial = material_system_register_material
};
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
uint32_t material_system_init()
{
	vec_init(&images);
	vec_init(&samplers);
	vec_init(&textures);
	vec_init(&materials);
}
uint32_t material_system_deinit()
{
	vec_deinit(&images);
	vec_deinit(&samplers);
	vec_deinit(&textures);
	vec_deinit(&materials);
}

uint32_t material_system_register_image(const char *imageuri)
{
	//1. search in images for the imageuri
	//TODO look into extracting this and introducing better algorithm
	int idx; char *val;
	vec_foreach(&images, val, idx)
	{
		if (!strcmp(imageuri, val))
			return idx;
	}

	return create_image(imageuri);
}
uint32_t material_system_register_sampler(int mag, int min)
{
	//TODO look into this
	int idx; Sampler *val;
	vec_foreach_ptr(&samplers, val, idx)
	{
		if (val->mag == mag && val->min == min)
			return idx;
	}

	return create_sampler(mag, min);
}
uint32_t material_system_register_texture(uint32_t samplerIdx, uint32_t imageIdx)
{
	//TODO look into this
	int idx; Texture* val;
	vec_foreach_ptr(&textures, val, idx)
	{
		if (val->imageIndex == imageIdx && val->samplerIndex == samplerIdx)
			return idx;
	}

	return create_texture(samplerIdx, imageIdx);
}
uint32_t material_system_register_material(Material *newMaterial)
{
	//TODO look into this
	int idx; Material* val;
	vec_foreach_ptr(&materials, val, idx)
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
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
uint32_t create_image(const char* imageuri)
{
	uint32_t idx = images.length;

	vec_push(&images, imageuri);

	//Registering images onto the gpu
	{
		int texWidth = 0, texHeight = 0, texChannels = 0;
		stbi_uc* pixels = stbi_load(imageuri, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		assert(pixels);
		Renderer.registerImageslBuffer(pixels, texWidth, texHeight);
	}

	return idx;
}
uint32_t create_sampler(int mag, int min)
{
	uint32_t idx = samplers.length;

	Sampler s = {
		.mag = mag,
		.min = min
	};
	vec_push(&samplers, s);
		
	return idx;
}
uint32_t create_texture(uint32_t samplerIdx, uint32_t imageIdx)
{
	uint32_t idx = textures.length;

	Texture t = {
		.samplerIndex = samplerIdx,
		.imageIndex = imageIdx
	};
	vec_push(&textures, t);

	return idx;
}
uint32_t create_material(Material* newMaterial)
{
	uint32_t idx = materials.length;

	vec_push(&materials, *newMaterial);

	return idx;
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//remove this later on
Material* get_material(uint32_t idx)
{
	return &materials.data[idx];
}
//gltf specific
uint32_t find_gltf_texture(cgltf_texture *t)
{
	if (!t)
		return 0;

	cgltf_sampler* sampler = t->sampler;
	cgltf_image   *image   = t->image;
	
	uint32_t samplerIdx;
	{
		if (!sampler)
			samplerIdx = MaterialSystem.registerSampler(0, 0);
		else
			samplerIdx = MaterialSystem.registerSampler(sampler->mag_filter, sampler->min_filter);
	}

	uint32_t imageIdx   = MaterialSystem.registerImage(image->uri);

	return MaterialSystem.registerTexture(samplerIdx, imageIdx);
}

Material* get_material_array() 	{
	return materials.data;
}