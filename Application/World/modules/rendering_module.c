//TODO Comments / Logging
#include "rendering_module.h"

void OnAddRenderingComponent(ecs_iter_t *it)
{
  RenderingComponent *rendComp = ecs_column(it, RenderingComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    vec_init(&rendComp[i].meshRenderData);
  }
}

void OnRemoveRenderingComponent(ecs_iter_t *it)
{
  RenderingComponent *rendComp = ecs_column(it, RenderingComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    vec_deinit(&rendComp[i].meshRenderData);
  }
}

void RenderingModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, RenderingModule);

  ECS_COMPONENT(world, RenderingComponent);

  ECS_TRIGGER(world, OnAddRenderingComponent, EcsOnAdd, RenderingComponent);
  ECS_TRIGGER(world, OnRemoveRenderingComponent, EcsOnRemove, RenderingComponent);

  /* ECS_TYPE(world, type, comp1, comp2, comp3); */

  /* ECS_EXPORT_ENTITY(Renderable); */
  ECS_EXPORT_COMPONENT(RenderingComponent);
  /* ECS_EXPORT_TYPE(type); */
}
