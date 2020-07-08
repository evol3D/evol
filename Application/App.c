#include "App.h"
#include "Window/Window.h"
#include "ev_log/ev_log.h"

static int start();
static int destroy();

struct _ev_app_struct App = {
    .name = "evol",
    .start = start,
    .destroy = destroy,
};

static int start()
{
    ev_log_info("Application Started");

    { // Window Initialization
        Window.init();
        Window.setTitle("evol3D");
        Window.setSize(800, 600);
        Window.createWindow();
    }


    // Do Stuff

    return 0;
}

static int destroy()
{
    Window.deinit();
    return 0;
}