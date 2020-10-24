#ifndef EVOL_RENDERER_H
#define EVOL_RENDERER_H

#include "renderer_types.h"
#include "RendererBackend.h"

extern struct ev_Renderer {
  int (*init)();
  int (*deinit)();
  unsigned int (*registerIndexBuffer)(unsigned int *indices, unsigned long long size);
  unsigned int (*registerVertexBuffer)(real *vertices, unsigned long long size);
} Renderer;

#endif //EVOL_RENDERER_H 
