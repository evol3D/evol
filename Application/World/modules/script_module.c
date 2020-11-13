//TODO Comments / Logging
#include "script_module.h"

ECS_COMPONENT_DECLARE(ScriptComponent);
ECS_COMPONENT_DECLARE(CScriptOnUpdate);
ECS_COMPONENT_DECLARE(CScriptOnCollision);

void RunOnUpdateC(ecs_iter_t *it)
{
  ECS_COLUMN(it, CScriptOnUpdate, script, 1);

  for(int i = 0; i < it->count; ++i)
  {
    script[i].fn(it->entities[i]);
  }
}


