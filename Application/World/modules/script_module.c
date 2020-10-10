//TODO Comments / Logging
#include "script_module.h"

void RunOnUpdate(ecs_iter_t *it)
{
  ECS_COLUMN(it, ScriptComponent, script, 1);

  // Somehow run the OnUpdate function in the script
  printf("Scriptname: %s, OnUpdate\n", script->script_path);
}

void ScriptModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, ScriptModule);

  ECS_COMPONENT(world, ScriptComponent);

  ECS_SYSTEM(world, RunOnUpdate, EcsOnUpdate, ScriptComponent);

  ECS_EXPORT_COMPONENT(ScriptComponent);
  ECS_EXPORT_ENTITY(RunOnUpdate);
}
