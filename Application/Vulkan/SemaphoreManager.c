#include "SemaphoreManager.h"

// On Fences:
// https://stackoverflow.com/questions/39599633/why-is-a-vkfence-necessary-for-every-swapchain-command-buffer-when-already-using

static int ev_semaphoremanager_init();
static int ev_semaphoremanager_deinit();

struct ev_SemaphoreManager SemaphoreManager = {
  .init   = ev_semaphoremanager_init,
  .deinit = ev_semaphoremanager_deinit,
};


struct ev_SemaphoreManagerData {
} SemaphoreManagerData;

#define DATA(a) SemaphoreManagerData.a

static int ev_semaphoremanager_init()
{
  return 0;
}

static int ev_semaphoremanager_deinit()
{
  return 0;
}
