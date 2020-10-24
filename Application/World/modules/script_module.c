//TODO Comments / Logging
#include "script_module.h"

void RunOnUpdateC(ecs_iter_t *it)
{
  ECS_COLUMN(it, CScriptComponent, script, 1);

  for(int i = 0; i < it->count; ++i)
  {
    script[i].on_update(it->entities[i]);
  }
}

void OnAddCScriptComponent(ecs_iter_t *it)
{
  ECS_COLUMN(it, CScriptComponent, script, 1);
  for(int i = 0; i < it->count; ++i)
  {
    script[i].on_update = NULL;
  }
}

void OnSetCScriptComponent(ecs_iter_t *it)
{
  ECS_IMPORT(it->world, ScriptModule);
  CScriptComponent *script = ecs_column(it, CScriptComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    if(script[i].on_update)
    {
      ecs_add(it->world, it->entities[i], OnUpdateC);
    }
    else
    {
      ecs_remove(it->world, it->entities[i], OnUpdateC);
    }
  }
}

void ScriptModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, ScriptModule);

  ECS_COMPONENT(world, ScriptComponent);
  ECS_COMPONENT(world, CScriptComponent);

  ECS_TAG(world, OnUpdateC);

  ECS_SYSTEM(world, RunOnUpdateC, EcsOnUpdate, CScriptComponent, OnUpdateC);

  ECS_EXPORT_COMPONENT(ScriptComponent);
  ECS_EXPORT_COMPONENT(CScriptComponent);
  ECS_EXPORT_ENTITY(OnUpdateC);

  ECS_TRIGGER(world, OnAddCScriptComponent, EcsOnAdd, CScriptComponent);

  ECS_SYSTEM(world, OnSetCScriptComponent, EcsOnSet, CScriptComponent);
}
