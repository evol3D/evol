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

Entity box;

static int ev_game_init()
{
  ImportModule(TransformModule);
  ImportModule(PhysicsModule);

  Physics.setGravity(0, -10, 0);

  box = CreateEntity();
  Entity_SetComponent(box, 
      TransformComponent, {
        .position = { 0,  10,  0},
        .rotation = { 0,  0,  0},
        .scale    = { 1,  1,  1},
      });
  Entity_SetComponent(box,
      RigidBodyComponent, {
        .mass = 1,
        .collisionShape = Physics.createBox(1, 1, 1),
      });

  Entity sphere = CreateEntity();
  Entity_SetComponent(sphere, 
      TransformComponent, {
        .position = {0, -30, 0},
        .rotation = {0, 0, 0},
        .scale    = {1, 1, 1},
      });
  Entity_SetComponent(sphere,
      RigidBodyComponent, {
        .mass = 0,
        .collisionShape = Physics.createSphere(30),
      });

  return 0;
}

static int ev_game_create_entity(EntityCreationInfo *ec_info)
{
}

static int ev_game_deinit()
{
  return 0;
}

void ev_game_loop_physics(real dt)
{
  Physics.step_dt(dt);

  ImportModule(TransformModule);
  ev_log_info("Physics Step. dt = %f", dt);
}

void update()
{
}

void fixedUpdate()
{
}

static void ev_game_loop()
{
  double old = 0;
  double new;

  while(!App.closeSystem)
  {
    new = Window.getTime();
    double timeStep = new - old;

    ev_game_loop_physics(timeStep);


    old = new;
  }
  ev_log_info("Exiting game loop");
}
