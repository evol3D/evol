#include "App.h"
#include <utils.h>

#include <evolpthreads.h>
#include <events/events.h>

static int start();
static int destroy();
static int game_loop();

struct ev_app_struct App = {
  .name = "evol",
  .start = start,
  .destroy = destroy,
  .lastFrameTime = 0,
  .lastWindowPollTime = 0,
  .lastEventSystemUpdate = 0,
  .framerate = 144,
  .windowPollRate = 1000,
  .eventSystemUpdateRate = 500,
};

//TODO: Fix the need for this
int result;

static int start()
{
  ev_log_info("Application Started");

  { // EventSystem Initialization
    //TODO: Fix the need for this
    SET_RESULT_VAR(result);

    INIT_EVOL_EVENTS();
    EventSystem.init();
  }

  {
    EventDebug.init();
  }

  { // Window Initialization
    Window.init();
    Window.setTitle("evol3D");
    Window.setSize(800, 600);
    Window.createWindow();
  }

  /* { */
  /*   Input.init(); */
  /* } */

  {
    Physics.init();
  }

  /* { */
  /*   World.init(); */
  /* } */

  {
    // Scripting initialization
  }

  /* { */
  /*     Scratchpad.execute(); */
  /* } */

  {
      Game.init();
  }

  return game_loop();
}

bool closeSystem = false;

static void* event_system_loop()
{
  while(!closeSystem)
  {
    double time = Window.getTime();
    double timeStep = time - App.lastEventSystemUpdate;
    double remainingTime = (1.f/(double)App.eventSystemUpdateRate) - timeStep;

    if(remainingTime <= 0)
    {
      EventSystem.progress();
      App.lastEventSystemUpdate = time;
    }
    else
    {
      sleep_ms(remainingTime * 1000);
    }
  }


  return 0;
}

/* static void *render_loop() */
/* { */
/*   while(!closeSystem) */
/*   { */
/*     double time = Window.getTime(); */
/*     double timeStep = time - App.lastFrameTime; */
/*     double remainingTime = (1.f/(double)App.framerate) - timeStep; */

/*     if(remainingTime <= 0) */
/*     { */
/*       /1* ControlEvent e = CreateControlEvent( *1/ */
/*       /1*   (NewFrame), *1/ */
/*       /1*   ((ControlEventData){ *1/ */
/*       /1*     .timeStep = timeStep, *1/ */
/*       /1*   }) *1/ */
/*       /1* ); *1/ */
/*       /1* EventSystem.dispatch(&e); *1/ */
/*       App.lastFrameTime = time; */
/*     } */
/*     else */
/*     { */
/*         sleep_ms(remainingTime * 1000); */
/*     } */
/*   } */
/*   return 0; */
/* } */

static int game_loop()
{
  pthread_t eventSystem_thread;
  /* pthread_t renderLoop_thread; */

  pthread_create(&eventSystem_thread, NULL, event_system_loop, NULL);
  /* pthread_create(&renderLoop_thread, NULL, render_loop, NULL); */

  while(!Window.shouldClose())
  {
    double time = Window.getTime();
    double timeStep = time - App.lastWindowPollTime;
    double remainingTime = (1.f/(double)App.windowPollRate) - timeStep;

    if (remainingTime <= 0)
    {
      Window.pollEvents();
      App.lastWindowPollTime = time;
    }
    else
    {
      sleep_ms(remainingTime * 1000);
    }
  }
  return App.destroy();
}

static int destroy()
{
  {
    Game.deinit();
  }

  closeSystem = true;

  { // Terminating modules
    Physics.deinit();
    /* World.deinit(); */
    /* Input.deinit(); */
    Window.deinit();
    EventDebug.deinit();
    EventSystem.deinit();
  }
  return 0;
}
