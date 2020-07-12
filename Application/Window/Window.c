#include "Window.h"
#include "GLFW/glfw3.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "EventSystem.h"
#include <ev_log/ev_log.h>

static int ev_window_init();
static int ev_window_create_window();
static int ev_window_deinit();
static int ev_window_set_size(int width, int height);
static void ev_window_set_callbacks();
static int ev_window_set_title(const char *title);
static void ev_window_poll_events();
static bool ev_window_should_close();
static inline bool ev_window_is_created();
static inline double ev_window_get_time();

static inline void *ev_get_window_handle();

struct ev_Window_Data {
    uint width;
    uint height;
    char windowTitle[MAX_WINDOW_TITLE_LENGTH];
    void *windowHandle;

    bool created;

    struct EventListener eventListener;
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
};

static bool ev_window_should_close()
{
    return glfwWindowShouldClose(WindowData.windowHandle);
}

static void ev_window_poll_events()
{
    glfwPollEvents();
}

static int ev_window_init()
{
    glfwInit();

    WindowData.width = DEFAULT_WINDOW_WIDTH;
    WindowData.height = DEFAULT_WINDOW_HEIGHT;
    strcpy_s(WindowData.windowTitle, MAX_WINDOW_TITLE_LENGTH, DEFAULT_WINDOW_TITLE);
    WindowData.windowHandle = NULL;
    WindowData.created = false;



    return 0;
}

static int ev_window_deinit()
{
    { // EventListener termination
        event_listener_deinit(&WindowData.eventListener);
    }

    glfwDestroyWindow(WindowData.windowHandle);
    glfwTerminate();

    return 0;
}

bool ev_window_control_event_handler(ControlEvent *event)
{
    switch(event->variant)
    {
//        case WindowPollSignal:
//            Window.pollEvents();
//            break;
        default:
            break;
    }

    return false;
}

static int ev_window_create_window()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    WindowData.windowHandle = glfwCreateWindow(WindowData.width, WindowData.height, WindowData.windowTitle, NULL, NULL);
    WindowData.created = true;

    { // EventListener initialization
        event_listener_init(&WindowData.eventListener);
        event_listener_subscribe(&WindowData.eventListener, CONTROL_EVENT, ev_window_control_event_handler);
    }

    ev_window_set_callbacks();

    return 0;
}

static int ev_window_set_size(int width, int height)
{
    WindowData.width = width;
    WindowData.height = height;

    if(WindowData.created)
    {
        glfwSetWindowSize(WindowData.windowHandle, width, height);
    }
}

static inline void *ev_get_window_handle()
{
    return WindowData.windowHandle;
}

static void ev_framebuffer_size_event_dispatch(GLFWwindow *windowHandle, int width, int height)
{
    WindowEvent e = CreateWindowEvent(
            (WindowResized),
            ((WindowEventData) {
                .width = width,
                .height = height,
            })
    );

    EventSystem.dispatch(&e);
}

static inline double ev_window_get_time()
{
    return glfwGetTime();
}

static void ev_window_set_callbacks()
{
    glfwSetFramebufferSizeCallback(WindowData.windowHandle, ev_framebuffer_size_event_dispatch);
}

static int ev_window_set_title(const char *title)
{
    strcpy_s(WindowData.windowTitle, MAX_WINDOW_TITLE_LENGTH, title);

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
