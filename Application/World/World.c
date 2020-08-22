#include "World.h"
#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"

static int ev_world_init();
static int ev_world_deinit();

struct ev_World World = {
  .init   = ev_world_init,
  .deinit = ev_world_deinit,
};

struct ev_World_Data {
  ecs_world_t *instance;
} WorldData;

static int ev_world_init()
{
  WorldData.instance = ecs_init();
  return 0;
}

static int ev_world_deinit()
{
  ecs_fini(WorldData.instance);
  return 0;
}
