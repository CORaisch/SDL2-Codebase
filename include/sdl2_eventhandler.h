#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SDL2/SDL.h>

class Event_Handler
{
public:
    Event_Handler();
    ~Event_Handler();

    void update_events();
    bool is_key_pressed(int scancode);
    bool is_key_pressed(const char* keyname);
    bool is_any_key_pressed();
    bool is_running();

    // added for debugging
    void print_keymap();

private:
    bool _isRunning;
    SDL_Event _event;
    bool *_scancodes;
};

#endif
