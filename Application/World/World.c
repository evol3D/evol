#include "World.h"
#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"

static int ev_world_init();
static int ev_world_deinit();
static int ev_world_progress();

struct ev_World World = {
  .init   = ev_world_init,
  .deinit = ev_world_deinit,
  .progress = ev_world_progress,
};

struct ev_World_Data {
  int placeholder;
} WorldData;

static int ev_world_init()
{
  World.instance = ecs_init();

  return 0;
}

static int ev_world_deinit()
{
  ecs_fini(World.instance);
  return 0;
}

static int ev_world_progress()
{
  return ecs_progress(World.instance, 0);
}
