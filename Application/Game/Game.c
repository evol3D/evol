//TODO Comments / Logging
#include "Game.h"
#include "Physics.h"
#include "World/World.h"
#include "World/modules/transform_module.h"
#include "World/modules/script_module.h"
#include "World/modules/physics_module.h"

#include "EventSystem.h"
#include "events/KeyEvent.h"

#include "utils.h"

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

#include "flecs.h"

static int ev_game_init()
{
  World.newScene();

  AssetLoader.loadGLTF("CesiumMilkTruck.gltf");

  /* ImportModule(TransformModule); */
  /* ImportModule(PhysicsModule); */

  /* CreateNamedEntity(sphere); */

  /* Entity_SetComponent(sphere, */
  /*     TransformComponent, { */
  /*       .position = {0, 25, -45}, */
  /*       .rotation = {0, 0.383, 0, 0.924}, */
  /*       .scale    = {1, 1, 1}, */
  /*     }); */
  /* Entity_SetComponent(sphere, */
  /*     RigidBodyComponent, { */
  /*       .mass = 1, */
  /*       .collisionShape = Physics.createSphere(3), */
  /*     }); */

  /* Entity ground = CreateEntity(); */
  /* Entity_SetComponent(ground, */ 
  /*     TransformComponent, { */
  /*       .position = {0, -15, -30}, */
  /*       .rotation = {0.146, 0.354, 0.354, 0.854}, */
  /*       .scale    = {1, 1, 1}, */
  /*     }); */
  /* Entity_SetComponent(ground, */
  /*     RigidBodyComponent, { */
  /*       .mass = 0, */
  /*       .collisionShape = Physics.createBox(20, 1, 20), */
  /*     }); */

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
  World.progress();
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
      ev_game_loop_physics(timeStep);
      old = new;
    }
    else
    {
      sleep_ms(remainingTime * 1000);
    }
  }
  ev_log_info("Exiting game loop");
}
