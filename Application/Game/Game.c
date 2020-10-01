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

DECLARE_EVENT_HANDLER(physics_step_game_space, (KeyPressedEvent *event) {
    printf("Key event received\n");
    if(event->keyCode == KEY_SPACE)
    {
    }
});

enum PhysicsType
{
  PhysicsType_None,
  PhysicsType_Dynamic,
  PhysicsType_Kinematic,
  PhysicsType_Static,
};

typedef struct
{
  float position[3];
  float rotation[3];
  float scale[3];
  enum PhysicsType physicsType;
} EntityCreationInfo;


ev_Vector3 vec3(real x, real y, real z)
{
  return (ev_Vector3) {
    x, y, z, 0
  };
}


static int ev_game_init()
{
  ImportModule(TransformModule);
  ImportModule(PhysicsModule);

  Physics.setGravity(0, -10, 0);

  Entity box = CreateEntity();
  Entity_SetComponent(box, 
      TransformComponent, {
        .position = { 3,  10,  0},
        .rotation = { 0,  45,  0},
        .scale    = { 1,  1,  1},
      });
  Entity_SetComponent(box,
      RigidBodyComponent, {
        .mass = 1,
        .collisionShape = Physics.createBox(1, 1, 1),
      });
    printf("TransformComponent: %p\n", Entity_GetComponent(box, TransformComponent));

  Entity box_2 = CreateEntity();
  Entity_SetComponent(box_2, 
      TransformComponent, {
        .position = { 0,  10,  0},
        .rotation = { 0,  45,  0},
        .scale    = { 1,  1,  1},
      });
  Entity_SetComponent(box_2,
      RigidBodyComponent, {
        .mass = 1,
        .collisionShape = Physics.createBox(1, 1, 1),
      });
    printf("TransformComponent: %p\n", Entity_GetComponent(box_2, TransformComponent));

  Entity box_3 = CreateEntity();
  Entity_SetComponent(box_3, 
      TransformComponent, {
        .position = { 3,  10,  0},
        .rotation = { 0,  45,  0},
        .scale    = { 1,  1,  1},
      });
  Entity_SetComponent(box_3,
      RigidBodyComponent, {
        .mass = 1,
        .collisionShape = Physics.createBox(1, 1, 1),
      });
    printf("TransformComponent: %p\n", Entity_GetComponent(box_3, TransformComponent));

  Entity sphere = CreateEntity();
  Entity_SetComponent(sphere, 
      TransformComponent, {
        .position = {3, -35, 0},
        .rotation = {0, 45, 0},
        .scale    = {1, 1, 1},
      });
  Entity_SetComponent(sphere,
      RigidBodyComponent, {
        .mass = 0,
        .collisionShape = Physics.createSphere(30),
      });
    printf("TransformComponent: %p\n", Entity_GetComponent(sphere, TransformComponent));

  /* CollisionShape sphere_shape_1 = Physics.createSphere(1); */

  /* for(int i = 0; i < 2; ++i) */
  /* { */
  /*   printf("i: %d\n", i); */
  /*   Entity entt = ecs_new(World.instance, TransformComponent); */

  /*   printf("entt id: %ld\n", entt); */

  /*   Entity_SetComponent(entt, */
  /*       RigidBodyComponent, { */
  /*         .mass = 1, */
  /*         .collisionShape = sphere_shape_1, */
  /*       }); */
  /*   printf("RigidBodyComponent: %p\n", Entity_GetComponent(entt, RigidBodyComponent)); */

  /*   Entity_SetComponent(entt, TransformComponent, { */
  /*         .position = vec3( 0,  10,  0 ), */
  /*         .rotation = vec3( 0,  45,  0 ), */
  /*         .scale    = vec3( 1,  1,  1 ), */
  /*       }); */
  /*   printf("TransformComponent: %p\n", Entity_GetComponent(entt, TransformComponent)); */

  /*   printf("RigidBody Handle: %p\n", Entity_GetComponent(entt, RigidBodyHandleComponent)->handle); */
  /* } */

  return 0;
}

static int ev_game_create_entity(EntityCreationInfo *ec_info)
{
}

static int ev_game_deinit()
{
  return 0;
}

unsigned int iterations = 0;
void ev_game_loop_physics(real dt)
{

  ImportModule(TransformModule);
  ImportModule(PhysicsModule);

  Entity sphere = CreateEntity();
  Entity_SetComponent(sphere, 
      TransformComponent, {
        .position = {3, 15, 0},
        .rotation = {0, 45, 0},
        .scale    = {1, 1, 1},
      });
  Entity_SetComponent(sphere,
      RigidBodyComponent, {
        .mass = 1,
        .collisionShape = Physics.createSphere(3),
      });
    printf("TransformComponent: %p\n", Entity_GetComponent(sphere, TransformComponent));

  /* ecs_progress(World.instance, dt); */

  Physics.step_dt(dt);
  /* Physics.step(); */
  ev_log_info("Physics Step. dt = %f", dt);
  ev_log_info("Iterations: %u", ++iterations);

  /* ImportModule(TransformModule); */
  /* const TransformComponent *tr = Entity_GetComponent(box, TransformComponent); */
  /* printf("Position: (%f, %f, %f)\n", tr->position.x, tr->position.y, tr->position.z); */
  /* printf("Rotation: (%f, %f, %f)\n", tr->rotation.x, tr->rotation.y, tr->rotation.z); */
}

void update()
{
}

void fixedUpdate()
{
}


static void ev_game_loop()
{
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
  }
  ev_log_info("Exiting game loop");
}
