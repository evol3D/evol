//TODO Comments / Logging
#include "Window.h"
#include "GLFW/glfw3.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "EventSystem.h"
#include <Input/Input.h>
#include "events/events.h"
#include <ev_log/ev_log.h>

static int ev_window_init();
static int ev_window_create_window();
static int ev_window_deinit();
static int ev_window_set_size(unsigned int width, unsigned int height);
static void ev_window_set_callbacks();
static int ev_window_set_title(const char *title);
static void ev_window_poll_events();
static bool ev_window_should_close();
static inline bool ev_window_is_created();
static inline double ev_window_get_time();
static inline void ev_window_set_should_close(bool flag);
static VkResult ev_create_vulkan_surface(VkInstance instance, VkSurfaceKHR* surface);
static void ev_window_get_size(unsigned int *width, unsigned int *height);

static inline void *ev_get_window_handle();

struct ev_Window_Data {
    unsigned int width;
    unsigned int height;

    char windowTitle[MAX_WINDOW_TITLE_LENGTH];
    void *windowHandle;

    bool created;
} WindowData;

struct _ev_Window Window = {
        .init = ev_window_init,
        .createWindow = ev_window_create_window,
        .setSize = ev_window_set_size,
        .deinit = ev_window_deinit,
        .getWindowHandle = ev_get_window_handle,
        .setTitle = ev_window_set_title,
        .shouldClose = ev_window_should_close,
        .pollEvents = ev_window_poll_events,
        .isCreated = ev_window_is_created,
        .getTime = ev_window_get_time,
        .setShouldClose = ev_window_set_should_close,
        .createVulkanSurface = ev_create_vulkan_surface,
        .getSize = ev_window_get_size,
};

// Did the window receive a closing event?
static bool ev_window_should_close()
{
    return glfwWindowShouldClose(WindowData.windowHandle);
}

// Handle all pending window events
static void ev_window_poll_events()
{
    glfwPollEvents();
}

static int ev_window_init()
{
    glfwInit();

    WindowData.width = DEFAULT_WINDOW_WIDTH;
    WindowData.height = DEFAULT_WINDOW_HEIGHT;
    strcpy(WindowData.windowTitle, DEFAULT_WINDOW_TITLE);
    WindowData.windowHandle = NULL;
    WindowData.created = false;

    return 0;
}

static int ev_window_deinit()
{
    glfwDestroyWindow(WindowData.windowHandle);
    glfwTerminate();

    return 0;
}

DECLARE_EVENT_HANDLER(EV_WINDOW_KeyEventHandler, (KeyPressedEvent *event) {
    if(event->keyCode == KEY_ESCAPE)
    {
      Window.setShouldClose(1);
    }
});

static int ev_window_create_window()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    WindowData.windowHandle = glfwCreateWindow(WindowData.width, WindowData.height, WindowData.windowTitle, NULL, NULL);
    WindowData.created = true;

    ev_window_set_callbacks();

    ACTIVATE_EVENT_HANDLER(EV_WINDOW_KeyEventHandler, KeyPressedEvent);

    return 0;
}

static int ev_window_set_size(unsigned int width, unsigned int height)
{
    WindowData.width = width;
    WindowData.height = height;

    if(WindowData.created)
    {
        glfwSetWindowSize(WindowData.windowHandle, width, height);
    }

    return 0;
}

static inline void *ev_get_window_handle()
{
    return WindowData.windowHandle;
}

static void ev_framebuffer_size_event_dispatch(GLFWwindow *windowHandle, int width, int height)
{
  DISPATCH_EVENT(WindowResizedEvent, {
      .newWindowWidth = width,
      .newWindowHeight = height,
      });
}

static inline double ev_window_get_time()
{
    return glfwGetTime();
}

static void ev_window_set_callbacks()
{
    glfwSetFramebufferSizeCallback(WindowData.windowHandle, ev_framebuffer_size_event_dispatch);
    glfwSetWindowSizeCallback(WindowData.windowHandle, ev_framebuffer_size_event_dispatch);
}

static int ev_window_set_title(const char *title)
{
    strcpy(WindowData.windowTitle, title);

    if(WindowData.created)
    {
        glfwSetWindowTitle(WindowData.windowHandle, title);
    }
    return 0;
}

static inline bool ev_window_is_created()
{
    return WindowData.created;
}

static inline void ev_window_set_should_close(bool flag)
{
  glfwSetWindowShouldClose(Window.getWindowHandle(), flag);
}

VkResult ev_create_vulkan_surface(VkInstance instance, VkSurfaceKHR* surface)
{
    return glfwCreateWindowSurface(instance, WindowData.windowHandle, NULL, surface);
}

static void ev_window_get_size(unsigned int *width, unsigned int *height)
{
    *width = WindowData.width;
    *height = WindowData.height;
}
