#ifndef WORLD_RENDERING_MODULE_H
#define WORLD_RENDERING_MODULE_H

#include "flecs.h"
#include <Renderer/renderer_types.h>

typedef struct
{
  MeshRenderData meshRenderData;
} RenderingComponent;

ECS_COMPONENT_EXTERN(RenderingComponent);

void OnAddRenderingComponent(ecs_iter_t *it);
void OnRemoveRenderingComponent(ecs_iter_t *it);
void RenderingModuleImport(ecs_world_t *world);

#define DEFINE_COMPONENTS_RENDERING(world) \
  ECS_COMPONENT_DEFINE(world, RenderingComponent)

#define REGISTER_SYSTEMS_RENDERING(world) \
  ECS_TRIGGER(world, OnAddRenderingComponent, EcsOnAdd, RenderingComponent); \
  ECS_TRIGGER(world, OnRemoveRenderingComponent, EcsOnRemove, RenderingComponent)


#endif
