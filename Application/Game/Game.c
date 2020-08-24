#include "Game.h"
#include "World/World.h"
#include "World/modules/transform_module.h"

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
  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

