#include "EventDebug.h"
#include "stdbool.h"
#include "ev_log/ev_log.h"


void ev_Event_Debug_init();
bool ev_Event_Debug_mouse_event_printer(MouseEvent *event);
bool ev_Event_Debug_key_event_printer(KeyEvent *event);
bool ev_Event_Debug_window_event_printer(WindowEvent *event);
bool ev_Event_Debug_control_event_printer(ControlEvent *event);
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
//    event_listener_subscribe(&EventDebug.listener, CONTROL_EVENT, ev_Event_Debug_control_event_printer);

}

bool ev_Event_Debug_mouse_event_printer(MouseEvent *event)
{
  ev_log_info("MouseEvent Received");
    switch(event->variant)
    {
        case MouseMoved:
            ev_log_debug(
                    "MouseMoved: MousePosition(%.0f, %.0f)",
                    event->data.position.x, event->data.position.y
                    );
            break;
        case MouseButtonPressed:
            ev_log_debug(
                    "MouseButtonPressed: Button #%d, Mods: %d",
                    event->data.button, event->data.mods
            );
            break;
        case MouseButtonReleased:
            ev_log_debug(
                    "MouseButtonReleased: Button #%d, Mods: %d",
                    event->data.button, event->data.mods
            );
            break;
        default:
            ev_log_debug("Unsupported MouseEvent variant");
            break;
    }
    return false;
}

bool ev_Event_Debug_key_event_printer(KeyEvent *event)
{
  ev_log_info("KeyEvent Received");
    switch(event->variant)
    {
        case KeyPressed:
            ev_log_debug(
                    "KeyPressed: KeyCode: %d, Mods: %d",
                    event->data.key, event->data.mods
            );
            break;
        case KeyReleased:
            ev_log_debug(
                    "KeyReleased: KeyCode: %d, Mods: %d",
                    event->data.key, event->data.mods
            );
            break;
        case KeyRepeat:
            ev_log_debug(
                    "KeyRepeat: KeyCode: %d, Mods: %d",
                    event->data.key, event->data.mods
            );
            break;
        default:
            ev_log_warn("Unsupported KeyEvent variant");
            break;
    }
    return false;
}

bool ev_Event_Debug_window_event_printer(WindowEvent *event)
{
  ev_log_info("WindowEvent Received");
    switch(event->variant)
    {
        case WindowResized:
            ev_log_debug(
                    "WindowResized: NewSize(%d, %d)",
                    event->data.width, event->data.height
            );
            break;
        default:
            ev_log_warn("Unsupported WindowEvent variant");
            break;
    }

    return false;
}

bool ev_Event_Debug_control_event_printer(ControlEvent *event)
{
  ev_log_info("ControlEvent Received");
    switch(event->variant)
    {
        case NewFrame:
            ev_log_debug(
                    "NewFrame: TimeStep = %f",
                    event->data.timeStep
            );
            break;
        case WindowPollSignal:
            ev_log_debug(
					"WindowPollSignal"
            );
            break;
        default:
            ev_log_warn("Unsupported ControlEvent variant");
            break;
    }

    return false;
}

void ev_Event_Debug_deinit()
{
    event_listener_deinit(&EventDebug.listener);
}
