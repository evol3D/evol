#include "Input.h"
#include "Window/Window.h"
#include "assert.h"
#include "ev_log/ev_log.h"
#include "EventSystem.h"


// This is bad. This is so bad. If you ever plan on getting this engine big, you should
// start by removing this weird include and try structuring the code in a more sane way.
#include "GLFW/glfw3.h"

static int ev_input_init();
static int ev_input_deinit();
static KeyState ev_input_get_key(KeyCode keyCode);
static bool ev_input_get_key_down(KeyCode keyCode);
static MousePosition ev_input_get_mouse_position();
static MouseBtnState ev_input_get_mouse_btn(MouseButton);
static bool ev_input_get_mouse_btn_down(MouseButton);
static void ev_input_setup_callbacks();

struct ev_Input Input = {
        .init = ev_input_init,
        .deinit = ev_input_deinit,
        .getKey = ev_input_get_key,
        .getKeyDown = ev_input_get_key_down,
        .getMousePosition = ev_input_get_mouse_position,
        .getMouseBtn = ev_input_get_mouse_btn,
        .getMouseBtnDown = ev_input_get_mouse_btn_down,
};

struct ev_Input_Data {
    void *windowHandle;
} InputData;

static int ev_input_init()
{
    if(!Window.isCreated())
    {
        ev_log_fatal("Window needs to be created before Input is initialized");
        assert(0);
    }

    InputData.windowHandle = Window.getWindowHandle();
    ev_input_setup_callbacks();
    return 0;
}

static KeyState ev_input_get_key(KeyCode keyCode)
{
    return glfwGetKey(InputData.windowHandle, keyCode);
}

static bool ev_input_get_key_down(KeyCode keyCode)
{
    return ev_input_get_key(keyCode) == KEY_DOWN;
}

static MousePosition ev_input_get_mouse_position()
{
    MousePosition res;
    glfwGetCursorPos(InputData.windowHandle, &res.x, &res.y);
    return res;
}

static MouseBtnState ev_input_get_mouse_btn(MouseButton button)
{
    return glfwGetMouseButton(InputData.windowHandle, button);
}

static bool ev_input_get_mouse_btn_down(MouseButton button)
{
    return ev_input_get_mouse_btn(button) == MOUSE_BTN_DOWN;
}

static int ev_input_deinit()
{
    return 0;
}

static inline void mouse_move_event_dispatch(GLFWwindow* windowHandle, double x, double y)
{
    MouseEvent e = CreateMouseEvent( MouseMoved,((MouseEventData){(MousePosition){x, y}, 0}));
    EventSystem.dispatch(&e);
}

static inline void mouse_button_event_dispatch(GLFWwindow* windowHandle, int button, int action, int mods)
{
    MouseEvent e = CreateMouseEvent(
            ((action == MOUSE_BTN_DOWN)?MouseButtonPressed:MouseButtonReleased),
            ((MouseEventData) {
                .button = button,
                .mods = mods,
            })
    );
    EventSystem.dispatch(&e);
}

static inline void key_event_dispatch(GLFWwindow *windowHandle, int key, int scancode, int action, int mods)
{
    KeyEvent e = CreateKeyEvent(
            (
                    action == KEY_DOWN
                        ?KeyPressed
                        :action == KEY_UP
                            ?KeyReleased
                            :KeyRepeat
            ),
            ((KeyEventData) {
                .key = key,
                .mods = mods,
            })
    );

    EventSystem.dispatch(&e);
}

static void ev_input_setup_callbacks()
{
    { // Mouse-related callbacks
        glfwSetCursorPosCallback(InputData.windowHandle, mouse_move_event_dispatch);
        glfwSetMouseButtonCallback(InputData.windowHandle, mouse_button_event_dispatch);
    }

    { // Keyboard-related callbacks
        glfwSetKeyCallback(InputData.windowHandle, key_event_dispatch);
    }
}

