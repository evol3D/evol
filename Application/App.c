#include "App.h"
#include "ev_log/ev_log.h"

static int start();

struct App app = {
    .name = "evol",
    .start = start,
};

static int start()
{
    ev_log_info("Application Started");

    // Do Stuff

    return 0;
}

