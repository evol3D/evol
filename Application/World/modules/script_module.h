#ifndef WORLD_SCRIPT_MODULE_H
#define WORLD_SCRIPT_MODULE_H

#include "flecs.h"

typedef struct
{
  char* script_path;
} ScriptComponent;

typedef struct
{
  ECS_DECLARE_COMPONENT(ScriptComponent);
  ECS_DECLARE_ENTITY(RunOnUpdate);
} ScriptModule;

void ScriptModuleImport(ecs_world_t *world);

#define ScriptModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, ScriptComponent);\
  ECS_IMPORT_ENTITY(module, RunOnUpdate);\

#endif
