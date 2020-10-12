//TODO Comments / Logging
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
  .eventSystemUpdateRate = 1000,
};

//TODO: Fix the need for this
int result;

FILE *logFile;
static int start(void)
{
  logFile = fopen("evol.logs", "w");
  log_add_fp(logFile, EV_LOG_TRACE);
  /* ev_log_setlevel(EV_LOG_TRACE); */
  ev_log_info("Application Started");

  { // EventSystem Initialization
    //TODO: Fix the need for this
    SET_RESULT_VAR(result);

    INIT_EVOL_EVENTS();
    EventSystem.init();
  }

#ifdef DEBUG
    EventDebug.init();
#endif

  { // Window Initialization
    Window.init();
    Window.setTitle("evol3D");
    Window.setSize(800, 600);
    Window.createWindow();
  }

  {
    ev_log_debug("Initializing Vulkan");
    Vulkan.init();
    ev_log_debug("Initialized Vulkan");
  }

  {
    ev_log_debug("Initializing Renderer");
    Renderer.init();
    ev_log_debug("Initialized Renderer");
  }

  {
    ev_log_debug("Initializing Input");
    Input.init();
    ev_log_debug("Initialized Input");
  }

  {
    ev_log_debug("Initializing Physics");
    Physics.init();
    ev_log_debug("Initialized Physics");
  }

  {
    ev_log_debug("Initializing World");
    World.init();
    ev_log_debug("Initialized World");
  }

  /* { */
  /*   // Scripting initialization */
  /* } */

  { // Asset system initialization
    ev_log_debug("Initializing AssetLoader");
    AssetLoader.init();
    ev_log_debug("Initialized AssetLoader");
  }

  {
    ev_log_debug("Initializing Game");
    Game.init();
    ev_log_debug("Initialized Game");
  }

  ev_log_debug("Starting the game loop");
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

    AssetLoader.deinit();

    Renderer.deinit();
    Vulkan.deinit();

    Input.deinit();
    Window.deinit();

    World.deinit();

#ifdef DEBUG
    EventDebug.deinit();
#endif

    EventSystem.deinit();
  }
  fclose(logFile);
  return 0;
}
