#include <stdio.h>
#include <cstdlib>
#include <SDL2/SDL.h>

// function prototypes
void renderFont(uint8_t* font, int fx, int fy, int fw, int fh, uint8_t* screen, int sw, int sh, SDL_Color color);

// globals
struct wndParams
{
    int x = 0;
    int y = 0;
};

int main(int argc, char** argv)
{
    // init SDL2 video and event handling subsystems
    SDL_Init(SDL_INIT_VIDEO);

    // create SDL2 window
    int pixelscale = 20;
    struct wndParams wnd;
    wnd.x = 64*pixelscale; wnd.y = 32*pixelscale;
    SDL_Window *window;
    window = SDL_CreateWindow("SDL2 Window Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wnd.x, wnd.y, 0);
    if(!window)
    {
        printf("SDL2 failed to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // create SDL2 rendering context
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        printf("SDL2 failed to create a renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // retrieve information on the renderer
    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(renderer, &info) < 0)
    {
        printf("SDL2 failed to retrieve information about the renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    printf("SDL2 renderer is set to: %s\n", info.name);

    // create SDL2 texture used as drawing area
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, wnd.x, wnd.y);
    uint8_t *screen = new uint8_t[wnd.x * wnd.y * 4];
    uint8_t *origscreen = new uint8_t[64*32*4];

    // set clear color of renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // define font
    uint8_t fS[8] = {0x00, 0x1C, 0x20, 0x10, 0x08, 0x04, 0x38, 0x00};
    uint8_t fD[8] = {0x00, 0x38, 0x24, 0x24, 0x24, 0x24, 0x38, 0x00};
    uint8_t fL[8] = {0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x00};
    uint8_t f2[8] = {0x00, 0x3C, 0x04, 0x08, 0x10, 0x20, 0x3C, 0x00};

    /* BEGIN OF RENDER LOOP */
    int icolor = 0; int sign = 1;
    SDL_Event event;
    while(true)
    {
        // exit loop by clicking 'x'
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
            break;

        // NOTE all the drawing must happen between SDL_RenderClear and SDL_RenderPresent
        // clear screen once
        SDL_RenderClear(renderer);

        // draw colored noise at background of original screen
        for(int i = 0; i < 64*32; ++i)
        {
            int x = i%64; int y = i/64;
            origscreen[y*64*4 + x*4 + 0] = rand() % 256;     // blue
            origscreen[y*64*4 + x*4 + 1] = rand() % 256;     // green
            origscreen[y*64*4 + x*4 + 2] = rand() % 256;     // red
            origscreen[y*64*4 + x*4 + 3] = SDL_ALPHA_OPAQUE; // alpha
        }
        // render string "SDL2" at foreground of original screen
        SDL_Color color; color.r = icolor; color.g = icolor; color.b = icolor; color.a = SDL_ALPHA_OPAQUE;
        icolor += sign*1;
        if(icolor>=255 || icolor<=0) sign *= -1;
        // render S at (16,12)
        renderFont(fS, 16, 12, 8, 8, origscreen, 64, 32, color);
        // render D at (24,12)
        renderFont(fD, 24, 12, 8, 8, origscreen, 64, 32, color);
        // render L at (32,12)
        renderFont(fL, 32, 12, 8, 8, origscreen, 64, 32, color);
        // render 2 at (40,12)
        renderFont(f2, 40, 12, 8, 8, origscreen, 64, 32, color);

        // render colored noise with scaled pixels
        for(int i = 0; i < wnd.x*wnd.y; ++i)
        {
            int x = i%wnd.x; int y = i/wnd.x;
            int ox = x/pixelscale; int oy = y/pixelscale;
            screen[wnd.x*4*y + 4*x + 0] = origscreen[64*4*oy + 4*ox + 0];
            screen[wnd.x*4*y + 4*x + 1] = origscreen[64*4*oy + 4*ox + 1];
            screen[wnd.x*4*y + 4*x + 2] = origscreen[64*4*oy + 4*ox + 2];
            screen[wnd.x*4*y + 4*x + 3] = origscreen[64*4*oy + 4*ox + 3];
        }

        // update texture
        SDL_UpdateTexture(texture, NULL, screen, wnd.x*4);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // present rendered screen (swap double buffers)
        SDL_RenderPresent(renderer);
    }
    /* END OF RENDER LOOP */

    // clean up and exit
    delete[] screen;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

void renderFont(uint8_t* font, int fx, int fy, int fw, int fh, uint8_t* screen, int sw, int sh, SDL_Color color)
{
    for(int my = fy; my < fy+fh; ++my)
        for(int mx = fx; mx < fx+fw; ++mx)
        {
            // dont draw if font exceeds border
            if(mx<sw && mx>=0 && my<sh && my>=0)
            {
                // draw pixel whenever fonts' bitmap is set
                int x = mx-fx; int y = my-fy;
                if(font[y] & (0x80 >> x))
                {
                    screen[sw*4*my + 4*mx + 0] = color.b;
                    screen[sw*4*my + 4*mx + 1] = color.g;
                    screen[sw*4*my + 4*mx + 2] = color.r;
                    screen[sw*4*my + 4*mx + 3] = color.a;
                }
            }
        }
}
