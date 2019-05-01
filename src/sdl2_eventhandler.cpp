#include "sdl2_eventhandler.h"

Event_Handler::Event_Handler()
{
    _scancodes_pressed = new bool[285];
    memset(_scancodes_pressed, 0, sizeof(bool)*285);
    _scancodes_released = new bool[285];
    memset(_scancodes_released, 0, sizeof(bool)*285);
    _isRunning = true;
}

Event_Handler::~Event_Handler()
{
    delete[] _scancodes_pressed;
    delete[] _scancodes_released;
}

void Event_Handler::update_events()
{
    if(SDL_PollEvent(&_event))
    {
        switch(_event.type)
        {
        case SDL_QUIT:
            _isRunning = false;
            break;
        case SDL_KEYDOWN:
            _scancodes_pressed[_event.key.keysym.scancode] = 1;
            break;
        case SDL_KEYUP:
            _scancodes_pressed[_event.key.keysym.scancode] = 0;
            _scancodes_released[_event.key.keysym.scancode] = 1;
            break;
        }
    }
}

bool Event_Handler::is_running()
{
    return _isRunning;
}

bool Event_Handler::is_key_pressed(int scancode)
{
    return _scancodes_pressed[scancode];
}

bool Event_Handler::is_key_pressed(const char* keyname)
{
    return _scancodes_pressed[SDL_GetScancodeFromName(keyname)];
}

bool Event_Handler::is_key_released(int scancode)
{
    int tmp = _scancodes_released[scancode];
    _scancodes_released[scancode] = 0;
    return tmp;
}

bool Event_Handler::is_key_released(const char* keyname)
{
    int tmp = _scancodes_released[SDL_GetScancodeFromName(keyname)];
    _scancodes_released[SDL_GetScancodeFromName(keyname)] = 0;
    return tmp;
}

bool Event_Handler::is_any_key_pressed()
{
    for(int i=0; i<285; ++i)
        if(_scancodes_pressed[i])
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
        {
            if((iy*cols+ix) < 285)
                printf("%i, ", _scancodes_pressed[iy*cols + ix]);
        }
        printf("\n");
    }
}
