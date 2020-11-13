//TODO Comments / Logging
#include "rendering_module.h"

ECS_COMPONENT_DECLARE(RenderingComponent);

void OnAddRenderingComponent(ecs_iter_t *it)
{
  RenderingComponent *rendComp = ecs_column(it, RenderingComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    vec_init(&rendComp[i].meshRenderData.primitives);
  }
}

void OnRemoveRenderingComponent(ecs_iter_t *it)
{
  RenderingComponent *rendComp = ecs_column(it, RenderingComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    vec_deinit(&rendComp[i].meshRenderData.primitives);
  }
}

