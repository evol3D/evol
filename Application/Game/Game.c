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
      printf("Key is space\n");
      Physics.step();
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

static int ev_game_init()
{
  /* ImportModule(TransformModule); */
  /* ImportModule(ScriptModule); */

  /* Entity box = CreateEntity(); */

  ACTIVATE_EVENT_HANDLER(physics_step_game_space, KeyPressedEvent);

  Physics.setGravity(0, -1, 0);

  RigidBody box_rb;
  box_rb.position = (PhysicsPosition){0, 10, 0};
  box_rb.rotation = (PhysicsEulerRotation){0, 0, 0};
  box_rb.collisionShape = Physics.createBox(1, 1, 1);
  box_rb.mass = 10;

  RigidBody sphere_rb;
  sphere_rb.position = (PhysicsPosition) {0, -30, 0};
  sphere_rb.rotation = (PhysicsEulerRotation) {0, 0, 0};
  sphere_rb.collisionShape = Physics.createSphere(20);
  sphere_rb.mass = 0;

  Physics.addRigidBody(&box_rb);
  Physics.addRigidBody(&sphere_rb);

  /* Physics.step(); */

  /* Entity_AddComponentSet(sphere, Transform); */
  /* Entity_SetComponent(sphere, ScriptComponent, {"path/to/script"}); */

  /* ecs_set_target_fps(World.instance, 1); */

  /* printf("Components in cube: %s\n", Entity_PrintComponents(cube)); */
  /* printf("Content of ScriptComponent in cube: %s\n", Entity_GetComponent(cube, ScriptComponent)->script_path); */

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
  ev_log_info("Physics Step");
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
  unsigned int physics_steprate = 60;
  double new;
  while(!App.closeSystem)
  {
    new = Window.getTime();
    double timeStep = new - old;
    double remainingTime = (1.f/(double)physics_steprate) - timeStep;

    old = new;
  }
  ev_log_info("Exiting game loop");
}
