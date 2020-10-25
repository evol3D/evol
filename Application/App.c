// TODO Comments / Logging
#include "Application/App.h"
#include "Application/Asset/AssetLoader.h"
#include "Application/Game/Game.h"
#include "Application/Input/Input.h"
#include "Application/Physics/Physics.h"
#include "Application/Renderer/Renderer.h"
#include "Application/Window/Window.h"
#include "Application/World/World.h"

#include <utils.h>

#include <EventSystem.h>
#include <events/events.h>

#ifdef DEBUG
#include <EventDebug/EventDebug.h>
#endif

#include <ev_log/ev_log.h>

static int start(int argc, char **argv);
static int destroy(void);
static int window_poll_loop(void);

static int handle_args(int argc, char **argv);

struct ev_app_struct App = {
    .name = "evol",
    .closeSystem = false,
    .start = start,
    .destroy = destroy,
    .lastFrameTime = 0,
    .framerate = 144,
    .windowPollRate = 1000,
    .eventSystemUpdateRate = 1000,
};

static int start(int argc, char **argv) {
  if (argc > 1) {
    int args_result = handle_args(argc, argv);
    if (args_result)
      return args_result;
  }

  // If a log file was opened, set it in the logger
  if (App.logs) {
    log_add_fp(App.logs, EV_LOG_TRACE);
    ev_log_info("File logging enabled. File name: %s", "log.evol");
  }

  ev_log_setlevel(EV_LOG_DEBUG);
  ev_log_info("Application Started");

  { // EventSystem Initialization
    ev_log_debug("Initializing EventSystem");

    INIT_EVOL_EVENTS();
    INITIALIZE_EVENTSYSTEM();

    if (EVENTSYSTEM_RESULT != EVENTSYSTEM_RESULT_OPERATION_SUCCESSFUL) {
      ev_log_error("EventSystem Initialization not successful: %s",
                   EVENTSYSTEM_RESULT_STRINGS[EVENTSYSTEM_RESULT]);
      return EVENTSYSTEM_RESULT;
    }

    ev_log_debug("Initialized EventSystem");
  }

#ifdef DEBUG
  ev_log_debug("DEBUG mode: Initializing EventDebugger");
  EventDebug.init();
  ev_log_debug("Initialized EventDebugger");
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
  return window_poll_loop();
}

static void *event_system_loop() {
  double lastUpdate = 0;
  while (!App.closeSystem) {
    double time = Window.getTime();
    double timeStep = time - lastUpdate;
    double remainingTime = (1.f / (double)App.eventSystemUpdateRate) - timeStep;

    if (remainingTime <= 0) {
      EventSystem.progress();
      lastUpdate = time;
    } else {
      sleep_ms(remainingTime * 1000);
    }
  }

  return 0;
}

static int window_poll_loop() {
  pthread_create(&App.eventsystem_thread, NULL, event_system_loop, NULL);
  pthread_create(&App.gameloop_thread, NULL, (void *)Game.loop, NULL);
  /* Game.loop(); */

  double lastUpdate = 0;
  while (!Window.shouldClose()) {
    double time = Window.getTime();
    double timeStep = time - lastUpdate;
    double remainingTime = (1.f / (double)App.windowPollRate) - timeStep;

    if (remainingTime <= 0) {
      Window.pollEvents();
      lastUpdate = time;
    } else {
      sleep_ms(remainingTime * 1000);
    }
  }
  return App.destroy();
}

static int destroy(void) {
  // This should start stopping other loops (threads)
  App.closeSystem = true;

  // Waiting for the other threads to finish
  pthread_join(App.eventsystem_thread, 0);
  ev_log_debug("EventSystem thread finished");
  pthread_join(App.gameloop_thread, 0);
  ev_log_debug("GameLoop thread finished");

  {
    ev_log_debug("Destroying Game");
    Game.deinit();
    ev_log_debug("Game Destroyed");
  }

  { // Terminating modules

    ev_log_debug("Destroying Physics");
    Physics.deinit();
    ev_log_debug("Physics Destroyed");

    ev_log_debug("Destroying AssetLoader");
    AssetLoader.deinit();
    ev_log_debug("AssetLoader Destroyed");

    ev_log_debug("Destroying Renderer");
    Renderer.deinit();
    ev_log_debug("Renderer Destroyed");

    ev_log_debug("Destroying Vulkan");
    Vulkan.deinit();
    ev_log_debug("Vulkan Destroyed");

    ev_log_debug("Destroying Input");
    Input.deinit();
    ev_log_debug("Input Destroyed");

    ev_log_debug("Destroying Window");
    Window.deinit();
    ev_log_debug("Window Destroyed");

    ev_log_debug("Destroying World");
    World.deinit();
    ev_log_debug("World Destroyed");

#ifdef DEBUG
    ev_log_debug("DEBUG mode: Destroying EventDebugger");
    EventDebug.deinit();
    ev_log_debug("EventDebugger Destroyed");
#endif

    ev_log_debug("Destroying EventSystem");
    EventSystem.deinit();
    ev_log_debug("EventSystem Destroyed");
  }

  ev_log_debug("Closing Log file");
  if (App.logs)
    fclose(App.logs);

  return 0;
}

static int handle_args(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-l") == 0) {
      ++i;
      if (argc == i) {
        ev_log_error("Please specify a file after the -l option");
        return 1;
      }
      App.logs = fopen(argv[i], "w");
      if (!App.logs)
        ev_log_error("Couldn't open file: %s", argv[i]);
    }
    // Template to copy:
    /* else if(strcmp(argv[i], "-l") == 0) */
    /* { */
    /*   App.logs = fopen("log.evol", "w"); */
    /* } */
  }
  return 0;
}
