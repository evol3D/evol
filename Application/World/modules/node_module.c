//TODO Comments / Logging
#include "node_module.h"
#include "utils.h"

void AddNodeComponent(ecs_iter_t *it)
{
  NodeComponent *node = ecs_column(it, NodeComponent, 1);
  vec_init(&node->children);
}

void RemoveNodeComponent(ecs_iter_t *it)
{
  NodeComponent *node = ecs_column(it, NodeComponent, 1);
  vec_deinit(&node->children);
}

void NodeModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, NodeModule);

  ECS_COMPONENT(world, NodeComponent);

  ECS_TRIGGER(world, AddNodeComponent, EcsOnAdd, NodeComponent);
  ECS_TRIGGER(world, RemoveNodeComponent, EcsOnRemove, NodeComponent);

  ECS_EXPORT_COMPONENT(NodeComponent);
}
