#include "App.h"
#include <utils.h>

#include <evolpthreads.h>
#include <events/events.h>

static int start(void);
static int destroy(void);
static int game_loop(void);

struct ev_app_struct App = {
  .name = "evol",
  .closeSystem = false,
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

static int start(void)
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

  {
    Vulkan.init();
  }

  {
    Renderer.init();
  }

  {
    Input.init();
  }

  {
    Physics.init();
  }

  {
    World.init();
  }

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


static void* event_system_loop()
{
  while(!App.closeSystem)
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

static int game_loop()
{

  pthread_create(&App.eventsystem_thread, NULL, event_system_loop, NULL);
  pthread_create(&App.gameloop_thread, NULL, (void*)Game.loop, NULL);

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

static int destroy(void)
{
  App.closeSystem = true;

  pthread_join(App.eventsystem_thread, 0);
  pthread_join(App.gameloop_thread, 0);

  {
    Game.deinit();
  }

  { // Terminating modules
    Physics.deinit();
    Input.deinit();
    Renderer.deinit();
    Vulkan.deinit();
    Window.deinit();
    World.deinit();
    EventDebug.deinit();
    EventSystem.deinit();
  }
  return 0;
}
