#include "EventDebug.h"
#include "stdbool.h"
#include "ev_log/ev_log.h"


void ev_Event_Debug_init();
bool ev_Event_Debug_mouse_event_printer(MouseEvent *event);
bool ev_Event_Debug_key_event_printer(KeyEvent *event);
bool ev_Event_Debug_window_event_printer(WindowEvent *event);
void ev_Event_Debug_deinit();

struct ev_Event_Debug EventDebug = {
        .init = ev_Event_Debug_init,
        .deinit = ev_Event_Debug_deinit,
};


void ev_Event_Debug_init()
{
    event_listener_init(&EventDebug.listener);

    event_listener_subscribe(&EventDebug.listener, MOUSE_EVENT, ev_Event_Debug_mouse_event_printer);
    event_listener_subscribe(&EventDebug.listener, KEY_EVENT, ev_Event_Debug_key_event_printer);
    event_listener_subscribe(&EventDebug.listener, WINDOW_EVENT, ev_Event_Debug_window_event_printer);

}

bool ev_Event_Debug_mouse_event_printer(MouseEvent *event)
{
    switch(event->variant)
    {
        case MouseMoved:
            printf(
                    "MouseMoved: MousePosition(%.0f, %.0f)\n",
                    event->data.position.x, event->data.position.y
                    );
            break;
        case MouseButtonPressed:
            printf(
                    "MouseButtonPressed: Button #%d, Mods: %d\n",
                    event->data.button, event->data.mods
            );
            break;
        case MouseButtonReleased:
            printf(
                    "MouseButtonReleased: Button #%d, Mods: %d\n",
                    event->data.button, event->data.mods
            );
            break;
        default:
            printf("Unsupported MouseEvent variant\n");
            break;
    }
    return false;
}

bool ev_Event_Debug_key_event_printer(KeyEvent *event)
{
    switch(event->variant)
    {
        case KeyPressed:
            printf(
                    "KeyPressed: KeyCode: %d, Mods: %d\n",
                    event->data.key, event->data.mods
            );
            break;
        case KeyReleased:
            printf(
                    "KeyReleased: KeyCode: %d, Mods: %d\n",
                    event->data.key, event->data.mods
            );
            break;
        case KeyRepeat:
            printf(
                    "KeyRepeat: KeyCode: %d, Mods: %d\n",
                    event->data.key, event->data.mods
            );
            break;
        default:
            printf("Unsupported KeyEvent variant\n");
            break;
    }
    return false;
}

bool ev_Event_Debug_window_event_printer(WindowEvent *event)
{
    switch(event->variant)
    {
        case WindowResized:
            printf(
                    "WindowResized: NewSize(%d, %d)\n",
                    event->data.width, event->data.height
            );
            break;
        default:
            printf("Unsupported WindowEvent variant\n");
            break;
    }

    return false;
}

void ev_Event_Debug_deinit()
{
    event_listener_deinit(&EventDebug.listener);
}
