#include "sdl2_eventhandler.h"

Event_Handler::Event_Handler()
{
    _scancodes = new bool[285];
    memset(_scancodes, 0, 285);
    _isRunning = true;
}

Event_Handler::~Event_Handler()
{
    delete[] _scancodes;
}

void Event_Handler::update_events()
{
    SDL_PollEvent(&_event);

    switch(_event.type)
    {
    case SDL_QUIT:
        _isRunning = false;
        break;
    case SDL_KEYDOWN:
        _scancodes[_event.key.keysym.scancode] = 1;
        break;
    case SDL_KEYUP:
        _scancodes[_event.key.keysym.scancode] = 0;
        break;
    }
}

bool Event_Handler::is_running()
{
    return _isRunning;
}

bool Event_Handler::is_key_pressed(int scancode)
{
    return _scancodes[scancode];
}

bool Event_Handler::is_key_pressed(const char* keyname)
{
    return _scancodes[SDL_GetScancodeFromName(keyname)];
}

bool Event_Handler::is_any_key_pressed()
{
    for(int i=0; i<285; ++i)
        if(_scancodes[i])
            return true;
    return false;
}

void Event_Handler::print_keymap()
{
    const int cols = 20;
    const int rows = 285/cols + 1;
    printf("### keymap ###\n");
    for(int iy=0; iy<rows; ++iy)
    {
        printf("%i: ", iy);
        for(int ix=0; ix<cols; ++ix)
            printf("%i, ", _scancodes[iy*cols + ix]);
        printf("\n");
    }
}
