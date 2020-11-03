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

typedef struct
{
  ECS_DECLARE_COMPONENT(ScriptComponent);
  ECS_DECLARE_COMPONENT(CScriptOnUpdate);
  ECS_DECLARE_COMPONENT(CScriptOnCollision);
} ScriptModule;

void ScriptModuleImport(ecs_world_t *world);

#define ScriptModuleImportHandles(module)        \
  ECS_IMPORT_COMPONENT(module, ScriptComponent); \
  ECS_IMPORT_COMPONENT(module, CScriptOnUpdate);\
  ECS_IMPORT_COMPONENT(module, CScriptOnCollision);\


#endif
