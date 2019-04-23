#include <stdio.h>
#include "sdl2_eventhandler.h"

int main(int argc, char** argv)
{
    // init SDL2 audio and event handling subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // open window to allow event handling
    SDL_Window *window;
    window = SDL_CreateWindow("SDL2 Window Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if(!window)
    {
        printf("SDL2 failed to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // get SDL2 renderer that window is at least filled with a color -> cosmetics..
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        printf("SDL2 failed to create a renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // init event handler
    Event_Handler event_handler;

    // main loop
    while(event_handler.is_running())
    {
        // check for new events
        event_handler.update_events();

        // if(event_handler.is_key_pressed(4))
        //     printf("pressed a\n");

        // if(event_handler.is_key_pressed("B"))
        //     printf("pressed \"B\"\n");

        // beg DEBUGGING
        event_handler.print_keymap();
        // end DEBUGGING

        // render something simple into window -> cosmetics..
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // clean and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
