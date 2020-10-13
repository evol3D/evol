//TODO Comments / Logging
#include "Game.h"
#include "Physics.h"
#include "World/World.h"
#include "flecs.h"
#include "World/modules/transform_module.h"
#include "World/modules/script_module.h"
#include "World/modules/physics_module.h"
#include "World/modules/geometry_module.h"

#include "EventSystem.h"

#include "utils.h"

/* // Game Systems */
/* void SyncWorldToRenderer(SystemArgs *args); */

// API functions
static int ev_game_init();
static int ev_game_deinit();
static void ev_game_loop();

struct ev_Game Game = {
  .init   = ev_game_init,
  .deinit = ev_game_deinit,
  .loop   = ev_game_loop,
};

struct ev_Game_Data {
  int placeholder;
} GameData;

/* DECLARE_EVENT_HANDLER( SceneUpdatedEventHandler, (SceneUpdatedEvent *event) { */
/*     // TODO: Call callable system to sync renderer */
/*     /1* ecs_run(World.getInstance(), SyncWorldToRenderer, 0, NULL); *1/ */
/* }); */

static int ev_game_init()
{
  ev_log_trace("Started initializing the game");

  /* ev_log_trace("Activating event handlers"); */
  /* ACTIVATE_EVENT_HANDLER(SceneUpdatedEventHandler, SceneUpdatedEvent); */
  /* ev_log_trace("Activated event handlers"); */

  ev_log_trace("Starting a new scene");
  World.newScene();
  ev_log_trace("New scene started");

  /* ev_log_trace("Registering systems"); */
  /* RegisterCallableSystem(SyncWorldToRenderer, SHARED: MeshComponent); */
  /* ev_log_trace("Registered systems"); */

  ev_log_trace("Loading GLTF file");
  /* AssetLoader.loadGLTF("Triangle.gltf"); */
  /* AssetLoader.loadGLTF("Cube.gltf"); */
  /* AssetLoader.loadGLTF("InterpolationTest.gltf"); */
  /* AssetLoader.loadGLTF("CesiumMilkTruck.gltf"); */
  AssetLoader.loadGLTF("Duck.gltf");
  ev_log_trace("Loaded GLTF file");

  /* ev_log_trace("Dispatching SceneUpdatedEvent"); */
  /* DISPATCH_EVENT( SceneUpdatedEvent, {}); */
  /* ev_log_trace("Dispatched SceneUpdatedEvent"); */

  ev_log_trace("Finished initializing the game");
  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

unsigned int iterations = 0;
void ev_game_loop_physics(real dt)
{


  Physics.step_dt(dt);
  // ev_log_info("Physics Step. dt = %f", dt);
  /* ev_log_info("Iterations: %u", ++iterations); */

}

void update()
{
}

void fixedUpdate()
{
}


void spawn()
{
  ImportModule(TransformModule);
  ImportModule(PhysicsModule);

  double old = Window.getTime();
  unsigned int spawnrate = 50;
  double new;

  /* int spawned = 0; */

  while(!App.closeSystem)
  {
    new = Window.getTime();
    double timeStep = new - old;
    double remainingTime = (1.f/(double)spawnrate) - timeStep;

    if(remainingTime <= 0)
    {
      World.lockSceneAccess();
      Entity sphere = CreateEntity();
      Entity_SetComponent(sphere, EcsName, {"sphere_1"});
      Entity_SetComponent(sphere,
          TransformComponent, {
            .position = {0, 25, -20},
            .rotation = {0.146, 0.354, 0.354, 0.854},
            .scale    = {1, 1, 1},
          });
      Entity_SetComponent(sphere,
          RigidBodyComponent, {
            .mass = 1,
            .collisionShape = Physics.createSphere(3),
          });
      World.unlockSceneAccess();
      old = new;
      // ev_log_info("Spheres spawned: %d", spawned++);
    }
  }
}


static void ev_game_loop()
{
  /* pthread_t spawn_thread; */
  /* pthread_create(&spawn_thread, NULL, (void*)spawn, NULL); */
  double old = Window.getTime();
  unsigned int physics_steprate = 60;
  double new;
  while(!App.closeSystem)
  {
    new = Window.getTime();
    double timeStep = new - old;
    double remainingTime = (1.f/(double)physics_steprate) - timeStep;

    if(remainingTime<=0)
    {
      /* World.lockSceneAccess(); */
      /* World.progress(); */
      /* World.unlockSceneAccess(); */
      ev_game_loop_physics(timeStep);
      old = new;
    }
    else
    {
      /* sleep_ms(remainingTime * 1000); */
    }
  }
  ev_log_debug("Exiting game loop");
}

void SyncWorldToRenderer(SystemArgs *args)
{
  MeshComponent *meshes = ecs_column(args, MeshComponent, 1);
  (void)meshes;
  printf("SyncWorldToRenderer\n");

  for (int i = 0; i < args->count; ++i)
  {
    /* meshes[i]; */
  }
}

