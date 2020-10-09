#include "World.h"
#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include <evolpthreads.h>
#include "flecs_threads.h"


static int ev_world_init();
static int ev_world_deinit();
static int ev_world_progress();
static int ev_world_loadscene(const char *path);
static int ev_world_savescene(const char *path);
static int ev_world_newscene();

static void inline ev_world_unlocksceneaccess();
static void inline ev_world_locksceneaccess();
static SceneInstance inline ev_world_getinstance();

typedef struct Scene 
{
  // Handle to flecs' world
  SceneInstance instance;

  // What should a scene contain?
  // 1. Entities, Components, and Systems (ECS world) [x]
  // 2. Resources
  //    2.1. Each Entity will have an index to its mesh. Meshes should be included in the scene
  //    // Does that mean that we'll need to have scenes be self incubated?
  //    // I think a better way to architecture it (for now) is to keep it self incubated, but
  //    // have the asset loader be an external module that has the resources and marks them 
  //    // by the scenes in which they're needed. This approach will make future optimizations
  //    // in which some resources are shared between scenes a bit less painful.
} Scene;

struct ev_World World = {
  .init   = ev_world_init,
  .deinit = ev_world_deinit,
  .progress = ev_world_progress,
  .loadScene = ev_world_loadscene,
  .saveScene = ev_world_savescene,
  .newScene = ev_world_newscene,
  .lockSceneAccess = ev_world_locksceneaccess,
  .unlockSceneAccess = ev_world_unlocksceneaccess,
  .getInstance = ev_world_getinstance,
};

struct ev_World_Data {
  Scene *activeScene;
} WorldData;


void ev_flecs_dash_init();

// Initialize the ECS world
static int ev_world_init()
{
  WorldData.activeScene = 0;

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;
  ev_flecs_init_os_api(&api);
  ecs_os_set_api(&api);

  return 0;
}

// Terminate the ECS world
static int ev_world_deinit()
{

  // Free active scene
  if(WorldData.activeScene)
    ecs_fini(WorldData.activeScene->instance);

  return 0;
}

static int ev_world_newscene()
{
  if(WorldData.activeScene)
    assert(!"Unload active scene before creating a new one");

  WorldData.activeScene = malloc(sizeof(Scene));
  WorldData.activeScene->instance = ecs_init();

#ifdef DEBUG
  ev_flecs_dash_init();
#endif
}


// Take a step in the world. (Run systems)
static int ev_world_progress()
{
  if(!WorldData.activeScene)
    assert(!"World.progress() with no scene loaded into the world");

  return ecs_progress(WorldData.activeScene->instance, 0);
}

static void inline ev_world_unlocksceneaccess()
{
  ecs_lock(WorldData.activeScene->instance);
}

static void inline ev_world_locksceneaccess()
{
  ecs_unlock(WorldData.activeScene->instance);
}

static SceneInstance inline ev_world_getinstance()
{
  return WorldData.activeScene->instance;
}


#ifdef DEBUG
#include "World/modules/transform_module.h"
#include "World/modules/physics_module.h"
#include "World/modules/geometry_module.h"
void ev_flecs_dash_init()
{
  ECS_IMPORT(WorldData.activeScene->instance, FlecsMeta);
  ECS_IMPORT(WorldData.activeScene->instance, FlecsSystemsCivetweb);
  ECS_IMPORT(WorldData.activeScene->instance, FlecsDash);

  ECS_META(WorldData.activeScene->instance, TransformComponent);
  ECS_META(WorldData.activeScene->instance, RigidBodyComponent);
  ECS_META(WorldData.activeScene->instance, RigidBodyHandleComponent);

  ECS_META(WorldData.activeScene->instance, MeshPrimitive);
  ECS_META(WorldData.activeScene->instance, MeshComponent);

  ecs_set(WorldData.activeScene->instance, 0, EcsDashServer, {.port = 8080});
}

static int ev_world_loadscene(const char *path)
{
}

static int ev_world_savescene(const char *path)
{
}

#endif
