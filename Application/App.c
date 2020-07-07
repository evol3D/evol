#include "App.h"

static int start();

struct App app = {
    .name = "evol",
    .start = start,
};

static int start()
{
    return 0;
}

