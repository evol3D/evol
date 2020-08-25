#include "Game.h"
#include "World/World.h"
#include "World/modules/transform_module.h"
#include "World/modules/script_module.h"

static int ev_game_init();
static int ev_game_deinit();

struct ev_Game Game = {
  .init   = ev_game_init,
  .deinit = ev_game_deinit,
};

struct ev_Game_Data {
  int placeholder;
} GameData;

static int ev_game_init()
{
  ImportModule(TransformModule);
  ImportModule(ScriptModule);

  Entity cube = CreateEntity();

  Entity_AddComponentSet(cube, Transform);
  Entity_SetComponent(cube, ScriptComponent, {"path/to/script"});

  ecs_set_target_fps(World.instance, 1);

  printf("Components in cube: %s\n", Entity_PrintComponents(cube));
  printf("Content of ScriptComponent in cube: %s\n", Entity_GetComponent(cube, ScriptComponent)->script_path);

  while (ecs_progress(World.instance, 0));

  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

