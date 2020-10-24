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
  void (*on_update)(unsigned int entt);
} CScriptComponent;

typedef struct
{
  ECS_DECLARE_COMPONENT(ScriptComponent);
  ECS_DECLARE_COMPONENT(CScriptComponent);
  ECS_DECLARE_ENTITY(OnUpdateC);
} ScriptModule;

void ScriptModuleImport(ecs_world_t *world);

#define ScriptModuleImportHandles(module)        \
  ECS_IMPORT_COMPONENT(module, ScriptComponent); \
  ECS_IMPORT_COMPONENT(module, CScriptComponent);\
  ECS_IMPORT_ENTITY(module, OnUpdateC);          \


#endif
