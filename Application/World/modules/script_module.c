//TODO Comments / Logging
#include "script_module.h"

void RunOnUpdateC(ecs_iter_t *it)
{
  ECS_COLUMN(it, CScriptOnUpdate, script, 1);

  for(int i = 0; i < it->count; ++i)
  {
    script[i].fn(it->entities[i]);
  }
}

void ScriptModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, ScriptModule);

  ECS_COMPONENT(world, ScriptComponent);
  ECS_COMPONENT(world, CScriptOnUpdate);

  ECS_SYSTEM(world, RunOnUpdateC, EcsOnUpdate, CScriptOnUpdate);

  ECS_EXPORT_COMPONENT(ScriptComponent);
  ECS_EXPORT_COMPONENT(CScriptOnUpdate);
}
