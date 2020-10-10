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
  ev_log_trace("Started initializing the world");
  WorldData.activeScene = 0;

  ev_log_trace("Setting Flecs' OS API to defaults");
  ecs_os_set_api_defaults();
  ev_log_trace("Flecs' OS API defaults set");
  ecs_os_api_t api = ecs_os_api;

  ev_log_trace("Initializing Flecs' OS API");
  ev_flecs_init_os_api(&api);
  ev_log_trace("Flecs' OS API initialized");

  ecs_os_set_api(&api);

  ev_log_trace("Finished initializing the world");
  return 0;
}

// Terminate the ECS world
static int ev_world_deinit()
{
  ev_log_trace("Started destroying the world");

  // Free active scene
  if(WorldData.activeScene)
  {
    ev_log_trace("World has an active scene, destroying the scene");
    //TODO: There should be a dedicated function for destroying a scene
    ecs_fini(WorldData.activeScene->instance);
    ev_log_trace("Active scene destroyed");
  }

  ev_log_trace("Finished destroying the world");
  return 0;
}

static int ev_world_newscene()
{
  ev_log_trace("Started creating a new scene");

  if(WorldData.activeScene)
  {
    ev_log_trace("Active scene found, destroying");
    assert(!"Unload active scene before creating a new one");
  }

  ev_log_trace("Allocating memory for the new scene");
  WorldData.activeScene = malloc(sizeof(Scene));
  ev_log_debug("Malloc'ed %u bytes", sizeof(Scene));

  ev_log_trace("Initializing Flecs world...");
  WorldData.activeScene->instance = ecs_init();
  ev_log_trace("Initialized Flecs world");

#ifdef DEBUG
  ev_log_trace("DEBUG mode: Initializing Flecs dashboard for new scene");
  ev_flecs_dash_init();
  ev_log_trace("Initialized Flecs dashboard for new scene");
#endif

  ev_log_trace("Finished creating a new scene");
}


// Take a step in the world. (Run systems)
static int ev_world_progress()
{
  ev_log_trace("Taking a step in the world");
  if(!WorldData.activeScene)
  {
    ev_log_trace("No active scene found");

    // TODO Handle progress with no active scene appropriately
    assert(!"World.progress() with no scene loaded into the world");
  }

  int result = ecs_progress(WorldData.activeScene->instance, 0);
  ev_log_trace("World step taken");

  return result;
}

static void inline ev_world_unlocksceneaccess()
{
  ev_log_trace("Requesting scene access lock");
  ecs_lock(WorldData.activeScene->instance);
  ev_log_trace("Scene locked");
}

static void inline ev_world_locksceneaccess()
{
  ev_log_trace("Requesting scene access unlock");
  ecs_unlock(WorldData.activeScene->instance);
  ev_log_trace("Scene unlocked");
}

static SceneInstance inline ev_world_getinstance()
{
  return WorldData.activeScene->instance;
}


#ifdef DEBUG
# include "World/modules/transform_module.h"
# include "World/modules/physics_module.h"
# include "World/modules/geometry_module.h"
void ev_flecs_dash_init()
{
  ECS_IMPORT(WorldData.activeScene->instance, FlecsMeta);
  ECS_IMPORT(WorldData.activeScene->instance, FlecsSystemsCivetweb);
  ECS_IMPORT(WorldData.activeScene->instance, FlecsDash);

  ECS_META(WorldData.activeScene->instance, TransformComponent);
  ev_log_trace("FlecsMeta Entry: TransformComponent");
  ECS_META(WorldData.activeScene->instance, RigidBodyComponent);
  ev_log_trace("FlecsMeta Entry: RigidBodyComponent");
  ECS_META(WorldData.activeScene->instance, RigidBodyHandleComponent);
  ev_log_trace("FlecsMeta Entry: RigidBodyHandleComponent");

  ECS_META(WorldData.activeScene->instance, MeshPrimitive);
  ev_log_trace("FlecsMeta Entry: MeshPrimitive");
  ECS_META(WorldData.activeScene->instance, MeshComponent);
  ev_log_trace("FlecsMeta Entry: MeshComponent");

  short port = 8080;
  ecs_set(WorldData.activeScene->instance, 0, EcsDashServer, {.port = port});
  ev_log_debug("Set Flecs Dashboard server to port: %d", port);
}
#endif

static int ev_world_loadscene(const char *path)
{
  ev_log_debug("Loading scene from path: %s", path);
  assert(!"Not implemented");
}

static int ev_world_savescene(const char *path)
{
  ev_log_debug("Saving scene to path: %s", path);
  assert(!"Not implemented");
}
