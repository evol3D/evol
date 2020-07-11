#include "App.h"
#include "Window/Window.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"
#include "Input/Input.h"
#include "EventDebug/EventDebug.h"

static int start();
static int destroy();
static int game_loop();

struct ev_app_struct App = {
    .name = "evol",
    .start = start,
    .destroy = destroy,
};

static int start()
{
    ev_log_info("Application Started");

    { // EventSystem Initialization
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
        Input.init();
    }



    game_loop();

    // Do Stuff

    return 0;
}

static int game_loop()
{
    while(!Window.shouldClose())
    {
        Window.pollEvents();

        // EventSystem handles all buffered events
        EventSystem.update();
    }
    return 0;
}

static int destroy()
{
    {
        Input.deinit();
    }

    { // Window termination
        Window.deinit();
    }

    {
        EventDebug.deinit();
    }

    { // EventSystem termination
        EventSystem.deinit();
    }
    return 0;
}