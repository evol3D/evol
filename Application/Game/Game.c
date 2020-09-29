#include "Game.h"
#include "Physics.h"
#include "World/World.h"
#include "World/modules/transform_module.h"
#include "World/modules/script_module.h"
#include "World/modules/physics_module.h"

#include "EventSystem.h"
#include "events/KeyEvent.h"
#include "Input/Input.h"

static int ev_game_init();
static int ev_game_deinit();
static Entity ev_game_create_entity();

struct ev_Game Game = {
  .init   = ev_game_init,
  .deinit = ev_game_deinit,
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

static int ev_game_init()
{
  /* ImportModule(TransformModule); */
  /* ImportModule(ScriptModule); */

  /* Entity box = CreateEntity(); */

  ACTIVATE_EVENT_HANDLER(physics_step_game_space, KeyPressedEvent);

  RigidBody box_rb;
  box_rb.collisionShape = Physics.createBox(1, 1, 1);
  box_rb.mass = 1;

  Physics.addRigidBody(&box_rb);

  /* Physics.step(); */

  /* Entity_AddComponentSet(sphere, Transform); */
  /* Entity_SetComponent(sphere, ScriptComponent, {"path/to/script"}); */

  /* ecs_set_target_fps(World.instance, 1); */

  /* printf("Components in cube: %s\n", Entity_PrintComponents(cube)); */
  /* printf("Content of ScriptComponent in cube: %s\n", Entity_GetComponent(cube, ScriptComponent)->script_path); */

  return 0;
}

static int ev_game_deinit()
{
  return 0;
}
