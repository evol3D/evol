#include "Window.h"
#include "GLFW/glfw3.h"
#include "string.h"
#include "stdbool.h"

static int ev_window_init();
static int ev_window_create_window();
static int ev_window_deinit();
static int ev_window_set_size(int width, int height);
static void ev_window_set_callbacks();
static int ev_window_set_title(const char *title);

static inline void *ev_get_window_handle();

struct ev_Window_Data {
    uint width;
    uint height;
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
};

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
    glfwDestroyWindow(WindowData.windowHandle);
    glfwTerminate();
}

static int ev_window_create_window()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwCreateWindow(WindowData.width, WindowData.height, WindowData.windowTitle, NULL, NULL);
    WindowData.created = true;
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

static void ev_window_set_callbacks()
{

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