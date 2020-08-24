#include "Scratchpad.h"
#include "App.h"

static int ev_scratchpad_execute();

struct _ev_Scratchpad Scratchpad = {
        .execute = ev_scratchpad_execute,
};

void PrintTransforms(ecs_iter_t *it)
{
  TransformPosition *p = ecs_column(it, TransformPosition, 1);
  TransformRotation *r = ecs_column(it, TransformRotation, 2);
  TransformScale *s    = ecs_column(it, TransformScale   , 3);
  
  printf("Time since last frame: %f\n Time since last system call: %f\n", it->delta_time, it->delta_system_time);
}

static int ev_scratchpad_execute()
{
  // Write Code to be tested
  Entity player = CreateEntity();
  Entity_AddComponent(player, TransformPosition);

  return 0;
}
