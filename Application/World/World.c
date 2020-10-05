#include "World.h"
#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include <evolpthreads.h>
#include "flecs_threads.h"

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

ECS_STRUCT(DummyComponent, {
    float dummy;
});

void ev_flecs_dash_init();


// Initialize the ECS world
static int ev_world_init()
{
  World.instance = ecs_init();

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;
  ev_flecs_init_os_api(&api);
  ecs_os_set_api(&api);

#ifdef DEBUG
  ev_flecs_dash_init();
#endif

  return 0;
}

// Terminate the ECS world
static int ev_world_deinit()
{
  ecs_fini(World.instance);
  return 0;
}

// Take a step in the world. (Run systems)
static int ev_world_progress()
{
  return ecs_progress(World.instance, 0);
}


#ifdef DEBUG
#include "World/modules/transform_module.h"
#include "World/modules/physics_module.h"
void ev_flecs_dash_init()
{
  ECS_IMPORT(World.instance, FlecsMeta);
  ECS_IMPORT(World.instance, FlecsSystemsCivetweb);
  ECS_IMPORT(World.instance, FlecsDash);

  ECS_META(World.instance, TransformComponent);
  ECS_META(World.instance, RigidBodyComponent);
  ECS_META(World.instance, RigidBodyHandleComponent);

  ecs_set(World.instance, 0, EcsDashServer, {.port = 8080});
}
#endif
