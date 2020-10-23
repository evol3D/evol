//TODO Comments / Logging
#include "Game.h"

#include "App.h"
#include "Physics/Physics.h"
#include "Window.h"
#include "World/World.h"
#include "World/WorldModules.h"

#include "cglm/call/mat4.h"
#include "cglm/mat4.h"
#include "flecs.h"

#include "EventSystem.h"
#include "events/events.h"

#include "ev_log/ev_log.h"

#include "physics_types.h"
#include "utils.h"

// Game Systems
void SyncWorldToRenderer(SystemArgs *args);
void MaintainTransformConstraints(SystemArgs *args);

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

DECLARE_EVENT_HANDLER( SceneUpdatedEventHandler, (SceneUpdatedEvent *event) {
    // TODO: Call callable system to sync renderer
    /* ecs_run(World.getInstance(), SyncWorldToRenderer, 0, NULL); */
});

static int ev_game_init()
{
  ev_log_trace("Started initializing the game");

  ev_log_trace("Activating event handlers");
  ACTIVATE_EVENT_HANDLER(SceneUpdatedEventHandler, SceneUpdatedEvent);
  ev_log_trace("Activated event handlers");

  ev_log_trace("Starting a new scene");
  World.newScene();
  ev_log_trace("New scene started");

  ev_log_trace("Registering systems");
  ImportModule(GeometryModule);
  ImportModule(TransformModule);
  /* RegisterCallableSystem(SyncWorldToRenderer, SHARED: MeshComponent); */
#ifdef FLECS_DASHBOARD
  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: TransformComponent, TransformComponent);
#else
  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: transform.module.TransformComponent, transform.module.TransformComponent);
#endif
  ev_log_trace("Registered systems");

  ev_log_trace("Loading GLTF file");
  /* AssetLoader.loadGLTF("Triangle.gltf"); */
  /* AssetLoader.loadGLTF("Cube.gltf"); */
  /* AssetLoader.loadGLTF("InterpolationTest.gltf"); */
  AssetLoader.loadGLTF("CesiumMilkTruck.gltf");
  /* AssetLoader.loadGLTF("RiggedFigure.gltf"); */
  /* AssetLoader.loadGLTF("CesiumMan.gltf"); */
  /* AssetLoader.loadGLTF("WaterBottle.gltf"); */
  /* AssetLoader.loadGLTF("SciFiHelmet.gltf"); */
  /* AssetLoader.loadGLTF("Duck.gltf"); */
  ev_log_trace("Loaded GLTF file");

  ev_log_trace("Dispatching SceneUpdatedEvent");
  DISPATCH_EVENT( SceneUpdatedEvent, {});
  ev_log_trace("Dispatched SceneUpdatedEvent");

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
  /* ev_log_info("Physics Step. dt = %f", dt); */
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
  /* World.lockSceneAccess(); */
  /* ImportModule(TransformModule); */
  /* ImportModule(PhysicsModule); */
  /* World.unlockSceneAccess(); */

  /* double old = Window.getTime(); */
  /* unsigned int spawnrate = 50; */
  /* double new; */

  /* /1* int spawned = 0; *1/ */

  /* while(!App.closeSystem) */
  /* { */
  /*   new = Window.getTime(); */
  /*   double timeStep = new - old; */
  /*   double remainingTime = (1.f/(double)spawnrate) - timeStep; */

  /*   if(remainingTime <= 0) */
  /*   { */
  /*     World.lockSceneAccess(); */
  /*     Entity sphere = CreateEntity(); */
  /*     Entity_SetComponent(sphere, EcsName, {"sphere_1"}); */
  /*     Entity_SetComponent(sphere, */
  /*         TransformComponent, { */
  /*           .position = {0, 0, 0, 0}, */
  /*           .rotation = {0, 0, 0, 1}, */
  /*           .scale    = {1, 1, 1}, */
  /*         }); */
  /*     Entity_SetComponent(sphere, */
  /*         RigidBodyComponent, { */
  /*           .mass = 1, */
  /*           .restitution = 0.5, */
  /*           .collisionShape = Physics.createSphere(1), */
  /*         }); */
  /*     World.unlockSceneAccess(); */
  /*     old = new; */
  /*     // ev_log_info("Spheres spawned: %d", spawned++); */
  /*   } */
  /* } */
}


#include <types.h>
static void ev_game_loop()
{
  /* pthread_t spawn_thread; */
  /* pthread_create(&spawn_thread, NULL, (void*)spawn, NULL); */

  World.lockSceneAccess();
  ImportModule(TransformModule);
  ImportModule(PhysicsModule);

  /* { */
  /*   ENTITY(parent); */
  /*   ev_Matrix4 *parentTransform = &(Entity_GetComponent_mut(parent, TransformComponent)->worldTransform); */
  /*   glm_mat4_identity(*parentTransform); */
  /*   ecs_modified(World.getInstance(), parent, TransformComponent); */
  /*   Entity_SetComponent(parent, */
  /*     RigidBodyComponent, { */
  /*     .mass = 3, */
  /*     .type = EV_RIGIDBODY_DYNAMIC, */
  /*     .restitution = 1.3, */
  /*     .collisionShape = Physics.createBox(1, 1, 1), */
  /*     }); */

  /*   ENTITY_CHILDOF(child, parent); */
  /*   Entity_SetComponent(child, EcsName, {"child"}); */
  /*   ev_Matrix4 *childWorldTransform = &(Entity_GetComponent_mut(child, TransformComponent)->worldTransform); */
  /*   ev_Vector3 childPosition = {4, 4, 4}; */
  /*   glm_mat4_identity(*childWorldTransform); */

  /*   glm_translate(*childWorldTransform, (real*)&childPosition); */
  /*   ecs_modified(World.getInstance(), child, TransformComponent); */

  /*   Entity_SetComponent(child, */
  /*       RigidBodyComponent, */
  /*       { */
  /*       .mass = 1, */
  /*       .type = EV_RIGIDBODY_KINEMATIC, */
  /*       .restitution = 0, */
  /*       .collisionShape = Physics.createSphere(1), */
  /*       }); */


  /*   ENTITY_CHILDOF(child2, parent); */

  /*   ev_Matrix4 *child2WorldTransform = &(Entity_GetComponent_mut(child2, TransformComponent)->worldTransform); */
  /*   ev_Vector3 child2Position = {-4, -4, -4}; */
  /*   glm_mat4_identity(*child2WorldTransform); */
  /*   glm_translate(*child2WorldTransform, (real*)&child2Position); */

  /*   ecs_modified(World.getInstance(), child2, TransformComponent); */

  /*   Entity_SetComponent(child2, */
  /*       RigidBodyComponent, */
  /*       { */
  /*       .mass = 1, */
  /*       .type = EV_RIGIDBODY_KINEMATIC, */
  /*       .restitution = 0, */
  /*       .collisionShape = Physics.createSphere(1), */
  /*       }); */
  /*   /1* ecs_add_entity(World.getInstance(), child2, ECS_CHILDOF | parent); *1/ */

  /*   ENTITY_CHILDOF(child3, parent); */
  /*   Entity_SetComponent(child3, EcsName, {"child3"}); */

  /*   ev_Matrix4 *child3WorldTransform = &(Entity_GetComponent_mut(child3, TransformComponent)->worldTransform); */
  /*   ev_Matrix4 *child3LocalTransform = &(Entity_GetComponent_mut(child3, TransformComponent)->localTransform); */
  /*   ev_Vector3 child3Position = {8, 8, 8}; */
  /*   glm_mat4_identity(*child3WorldTransform); */
  /*   glm_mat4_identity(*child3LocalTransform); */

  /*   /1* glm_translate(*child3LocalTransform, (real*)&child2Position); *1/ */
  /*   glm_translate(*child3WorldTransform, (real*)&child3Position); */
  /*   ecs_modified(World.getInstance(), child3, TransformComponent); */

  /*   Entity_SetComponent(child3, */
  /*       RigidBodyComponent, */
  /*       { */
  /*       .mass = 0, */
  /*       .type = EV_RIGIDBODY_KINEMATIC, */
  /*       .restitution = 0, */
  /*       .collisionShape = Physics.createSphere(1), */
  /*       }); */
  /* } */

  Entity sphere = CreateEntity();
  Entity_SetComponent(sphere, EcsName, {"sphere_1"});

  ev_Matrix4 *transform = &(Entity_GetComponent_mut(sphere, TransformComponent)->worldTransform);
  ev_Vector3 position = {0, -10, 0};
  glm_mat4_identity(*transform);
  glm_translate(*transform, (real*)&position);

  Entity_SetComponent(sphere,
    RigidBodyComponent, {
    .mass = 0,
    .restitution = 0.5,
    .collisionShape = Physics.createBox(2, 2, 2),
    });
  World.unlockSceneAccess();

  double old = Window.getTime();
  unsigned int physics_steprate = 60;
  double new;
  while(!App.closeSystem)
  {
    ev_log_trace("Starting gameloop iteration");
    new = Window.getTime();
    double timeStep = new - old;
    double remainingTime = (1.f/(double)physics_steprate) - timeStep;

    if(remainingTime<=0)
    {
      World.progress();
      World.lockSceneAccess();
      ev_game_loop_physics(timeStep);
      World.unlockSceneAccess();
      old = new;
    }
    else
    {
      ev_log_trace("Gameloop going to sleep for %f milliseconds", remainingTime * 1000);
      sleep_ms(remainingTime * 1000);
    }
    ev_log_trace("Finished gameloop iteration");
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

#include <cglm/cglm.h>
void MaintainTransformConstraints(SystemArgs *args)
{
  TransformComponent *parentTransform = ecs_column(args, TransformComponent, 1);
  TransformComponent *transform = ecs_column(args, TransformComponent, 2);

  if(!parentTransform)
    return;

  for(int k = 0; k < args->count; ++k)
  {
    printf("%s Local Transform:\n", Entity_GetComponent(args->entities[k], EcsName)->alloc_value);;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            printf("%f, ", transform[k].worldTransform[i][j]);
        }
      printf("\n");
    }
    printf("%s Parent World Transform:\n", Entity_GetComponent(args->entities[k], EcsName)->alloc_value);;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            printf("%f, ", parentTransform[k].worldTransform[i][j]);
        }
      printf("\n");
    }
    printf("Entity's name: %s\n", Entity_GetComponent(args->entities[k], EcsName)->value);
    glm_mat4_mul(
        transform[k].localTransform,
        parentTransform->worldTransform,
        transform[k].worldTransform
        );

    printf("%s New World Transform:\n", Entity_GetComponent(args->entities[k], EcsName)->alloc_value);;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            printf("%f, ", transform[k].worldTransform[i][j]);
        }
      printf("\n");
    }
    printf("\n");
  }
}
