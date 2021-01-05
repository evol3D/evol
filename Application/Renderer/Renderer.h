#ifndef EVOL_RENDERER_H
#define EVOL_RENDERER_H

#include "renderer_types.h"
#include "RendererBackend.h"

typedef enum
{
  INDEXBUFFER,
  RESOURCEBUFFER
} RendererRegisterTypes;

extern struct ev_Renderer {
  int (*init)();
  int (*deinit)();

  unsigned int (*registerBuffer)(RendererRegisterTypes type, void* data, unsigned long long size);
  unsigned int (*registerMaterial)(void* pixels, uint32_t width, uint32_t height);

  int (*startFrame)(ev_RenderCamera *camera);
  int (*endFrame)();

  void (*draw)(MeshRenderData meshRenderData, ev_Matrix4 transformMatrix);
} Renderer;

#endif //EVOL_RENDERER_H
