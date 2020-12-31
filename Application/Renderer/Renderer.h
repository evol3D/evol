#ifndef EVOL_RENDERER_H
#define EVOL_RENDERER_H

#include "renderer_types.h"
#include "RendererBackend.h"
#include "Asset/material.h"

typedef enum RendererBuffersTypes {
	INDEX_BUFFER,
	VERTEX_BUFFER,
	NORMAL_BUFFER,
	UV_BUFFER,
	MATERIAL_BUFFER,
}RendererBuffersTypes;

extern struct ev_Renderer {
  int (*init)();
  int (*deinit)();

  unsigned int (*registerBuffer)(RendererBuffersTypes bufferType, void* data, unsigned long long size);

  void (*registertexture)(Texture* texture);
  void (*registerImageslBuffer)(uint32_t imageIndex, EvImage* newImageBuffer, VkImageView* imageView);

  int (*startFrame)(ev_RenderCamera *camera);
  int (*endFrame)();

  void (*draw)(MeshRenderData meshRenderData, ev_Matrix4 transformMatrix);
} Renderer;

#endif //EVOL_RENDERER_H 
