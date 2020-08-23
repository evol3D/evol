#include "Scratchpad.h"
#include "App.h"

static int ev_scratchpad_execute();

struct _ev_Scratchpad Scratchpad = {
        .execute = ev_scratchpad_execute,
};

  typedef struct {
    int x; int y; int z;
  } Position;
  typedef struct {
    int x; int y; int z;
  } Rotation;
  typedef struct {
    int x; int y; int z;
  } Scale;

void PrintTransforms(ecs_iter_t *it)
{
  Position *p = ecs_column(it, Position, 1);
  Rotation *r = ecs_column(it, Rotation, 2);
  Scale *s = ecs_column(it, Scale, 3);
  
  printf("Time since last frame: %f\n Time since last system call: %f\n", it->delta_time, it->delta_system_time);
}

static int ev_scratchpad_execute()
{
  // Write Code to be tested

  RegisterComponent(Position);
  RegisterComponent(Rotation);
  RegisterComponent(Scale);

  RegisterComponentSet(Transform, Position, Scale, Rotation);

  Entity e1 = CreateEntity();
  Entity_AddComponentSet(e1, Transform);

  Entity e2 = CreateEntity_from(e1);

  RegisterSystem_OnUpdate(PrintTransforms, Position, Rotation, Scale);

  printf("%s\n", Entity_PrintComponents(e1));

  int i = 0;

  while(ecs_progress(World.instance, 0)) {if(i++ >= 1) break;}

  return 0;
}
