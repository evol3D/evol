#ifndef WORLD_SCRIPT_MODULE_H
#define WORLD_SCRIPT_MODULE_H

#include "flecs.h"
#include <Input/Input.h>

typedef struct
{
  char* script_path;
} ScriptComponent;

typedef struct
{
  void (*fn)(unsigned int entt);
} CScriptOnUpdate;

typedef struct
{
  void (*fn) (void *collisionData);
} CScriptOnCollision;

ECS_COMPONENT_EXTERN(ScriptComponent);
ECS_COMPONENT_EXTERN(CScriptOnUpdate);
ECS_COMPONENT_EXTERN(CScriptOnCollision);

void RunOnUpdateC(ecs_iter_t *it);

#define DEFINE_COMPONENTS_SCRIPT(world) \
  ECS_COMPONENT_DEFINE(world, ScriptComponent); \
  ECS_COMPONENT_DEFINE(world, CScriptOnUpdate); \
  ECS_COMPONENT_DEFINE(world, CScriptOnCollision)

#define REGISTER_SYSTEMS_SCRIPT(world) \
  ECS_SYSTEM(world, RunOnUpdateC, EcsOnUpdate, CScriptOnUpdate)


#endif
