#include <stdio.h>
#include <cstdlib>
#include <SDL2/SDL.h>

// function prototypes
void audioCallback(void* userdata, Uint8* stream, int len);
double sineWave(double time, double freq);
double squareWave(double time, double freq);
double triangleWave(double time, double freq);
double sawWave(double time, double freq);

// globals
double signalFreq = 0.0; // set sine wave of 200Hz
int sampleRate = 44100;
double audio_volume;
unsigned int audio_pos;
double audio_freq;
// values needed to calculate tones
double baseFreq = 110.0;
double twelveRoot = pow(2.0, 1.0/12.0);

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

    // setup audio device
    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = sampleRate;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 128;
    want.callback = audioCallback;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(!dev)
    {
        printf("SDL2 failed on opening an audio device: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // printf device info
    printf("Dev. Freq.: %i\nDev. Format: %i\nDev. Channels: %i\nDev. Samples: %i\nDev. Size: %i\n",
           have.freq, have.format, have.channels, have.samples, have.size);

    // setup playback
    audio_pos = 0; // start playback at beginning
    sampleRate = have.freq; // the sample rate could have been changed by SDL so we should update it
    audio_volume = 0.2;

    // start playing tone
    SDL_PauseAudioDevice(dev, 0);

    // define piano keys with keyboard scancodes
    int piano[15] = {29, 22, 27, 6, 9, 25, 10, 5, 17, 13, 16, 14, 54, 15, 55};

    // start event loop
    SDL_Event event;
    bool isRunning = true;
    while(isRunning)
    {
        bool isKeyPressed = false;
        // handle keyboard events
        while(SDL_PollEvent(&event))
        {
            // handle quit event
            if(event.type == SDL_QUIT)
            {
                isRunning = false;
                break;
            }

            // handle keydown events
            for(int i = 0; i < 15; ++i)
            {
                if((event.key.state == SDL_PRESSED) && (event.key.keysym.scancode == piano[i]))
                {
                    signalFreq = baseFreq * pow(twelveRoot, i);
                    isKeyPressed = true;
                }
            }

            // increase volume on arrow_up
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == 82))
            {
                if(audio_volume+0.1 > 1.0)
                    audio_volume = 1.0;
                else
                    audio_volume += 0.1;
                printf("volume: %f\n", audio_volume);
            }
            // decrease volume on arrow_down
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == 81))
            {
                if(audio_volume-0.1 < 0.0)
                    audio_volume = 0.0;
                else
                    audio_volume -= 0.1;
                printf("volume: %f\n", audio_volume);
            }
            // pitch up tone on arrow_right
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == 79))
            {
                baseFreq += 10.0;
                printf("base frequency: %f\n", baseFreq);
            }
            // pitch down tone on arrow_left
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == 80))
            {
                baseFreq -= 10.0;
                printf("base frequency: %f\n", baseFreq);
            }

            // if no key is pressed make speaker silent
            if(!isKeyPressed)
            {
                signalFreq = 0.0;
                audio_pos = 0;
            }
        }

        // render something simple into window -> cosmetics..
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // clean and exit
    SDL_CloseAudioDevice(dev);
    SDL_Quit();

    return EXIT_SUCCESS;
}

// function that samples a sine wave with given frequency and amplitude at given time
double sineWave(double time, double freq)
{
    double adaptFreq = 1.0 / (sampleRate / freq);
    return sin(M_PI * 2 * adaptFreq * time);
}

// function that samples a square wave with given frequency and amplitude at given time
double squareWave(double time, double freq)
{
    double adaptFreq = 1.0 / (sampleRate / freq);
    double result = sin(M_PI * 2 * adaptFreq * time);
    if(result > 0)
        return 1.0;
    else
        return -1.0;
}

// function that samples a triangle wave with given frequency and amplitude at given time
double triangleWave(double time, double freq)
{
    double adaptFreq = 1.0 / (sampleRate / freq);
    return asin(sin(M_PI * 2 * adaptFreq * time));
}

// function that samples a saw wave with given frequency and amplitude at given time
double sawWave(double time, double freq)
{
    double adaptFreq = 1.0 / (sampleRate / freq);
    return (2.0/M_PI) * (adaptFreq * M_PI * fmod(time, 1.0/adaptFreq) - (M_PI/2.0));
}

// SDL2 audio callback
void audioCallback(void* userdata, Uint8* stream, int len) // userdate can be used e.g. for passing the frequency, etc.
{
    len /= 2; // len is in bytes. We are using 16 bit signal <=> 2 Bytes one sample -> len == number of samples
    int16_t* buf = (int16_t*) stream;
    for(int i = 0; i < len; ++i)
    {
        int16_t amp = audio_volume * INT16_MAX;
        buf[i] = amp * sineWave(audio_pos, signalFreq);
        // buf[i] = amp * squareWave(audio_pos, signalFreq);
        // buf[i] = amp * triangleWave(audio_pos, signalFreq);
        // buf[i] = amp * sawWave(audio_pos, signalFreq);
        audio_pos++;
    }
}
