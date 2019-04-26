#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <SDL2/SDL.h>
#include "sdl2_eventhandler.h"
#include "sdl2_envelop.h"

// function prototypes
void audioCallback(void* userdata, Uint8* stream, int len);
double sineWave(double time, double freq);
double squareWave(double time, double freq);
double triangleWave(double time, double freq);
double sawWave(double time, double freq);
void setVolume(double vol);
void printInfo();

// globals
int sample_rate = 44100;
double audio_volume = 0.2;
// values needed to calculate tones
std::vector<unsigned int> audio_positions(15, 0);
double base_freq = 110.0;
double twelveRoot = pow(2.0, 1.0/12.0);
// oscilloscope data
enum {OSC_SINE, OSC_TRIANGLE, OSC_SAW, OSC_SQUARE};
int oscillator = OSC_SINE;
const char* oscNames[4] = {"Sine Wave", "Triangle Wave", "Saw Wave", "Square Wave"};
// support for multiple voices
std::vector<double> voices(15, 0.0);
// set envelops
std::vector<Envelop> envelops(15, Envelop());

int main(int argc, char** argv)
{
    // init SDL2 audio and event handling subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

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
    want.freq = sample_rate;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 256;
    want.callback = audioCallback;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(!dev)
    {
        printf("SDL2 failed on opening an audio device: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // printf device info
    printf("### Device Info ###\n");
    printf("Sample Frequency: %i Hz\nSample Format: %i Bits per Sample\nChannels: %i\nSample Buffer Size: %i Bytes\n",
           have.freq, SDL_AUDIO_BITSIZE(have.format), have.channels, have.size);
    printf("### Synthesizer Setup ###\n");

    // setup playback
    sample_rate = have.freq; // the sample rate could have been changed by SDL so we should update it

    // start playing tone
    SDL_PauseAudioDevice(dev, 0);

    // define piano keys with keyboard scancodes
    int piano[15] = {29, 22, 27, 6, 9, 25, 10, 5, 17, 13, 16, 14, 54, 15, 55};

    // start event loop
    Event_Handler event_handler;
    while(event_handler.is_running())
    {
        // update events
        event_handler.update_events();

        // check for piano key
        for(int i=0; i<15; ++i)
        {
            // set note frequency on key down
            if(event_handler.is_key_pressed(piano[i]))
            {
                voices[i] = base_freq * pow(twelveRoot, i);
                envelops[i].onKeydown();
            }
            // set silent frequency on key release
            if(event_handler.is_key_released(piano[i]))
            {
                audio_positions[i] = 0;
                envelops[i].onKeyup();
            }
        }

        // handle volume
        if(event_handler.is_key_released(SDL_SCANCODE_UP))
            setVolume(0.1);
        else if(event_handler.is_key_released(SDL_SCANCODE_DOWN))
            setVolume(-0.1);

        // handle tone pitch
        if(event_handler.is_key_released(SDL_SCANCODE_LEFT))
            base_freq -= 10.0;
        if(event_handler.is_key_released(SDL_SCANCODE_RIGHT))
            base_freq += 10.0;

        // handle oscillator
        if(event_handler.is_key_released(SDL_SCANCODE_1))
            oscillator = OSC_SINE;
        if(event_handler.is_key_released(SDL_SCANCODE_2))
            oscillator = OSC_TRIANGLE;
        if(event_handler.is_key_released(SDL_SCANCODE_3))
            oscillator = OSC_SAW;
        if(event_handler.is_key_released(SDL_SCANCODE_4))
            oscillator = OSC_SQUARE;

        // print synthesizer info
        printInfo();

        // render something simple into window -> cosmetics..
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // clean and exit
    SDL_CloseAudioDevice(dev);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void printInfo()
{
    printf("\033[svolume: %2i%% | Base Frequency: %3.1f Hz | Oscillator: %13s\033[u",
           int(audio_volume * 100.0), base_freq, oscNames[oscillator]);
}

void setVolume(double vol)
{
    double tmp = audio_volume + vol;
    if(tmp > 1.0)
        audio_volume = 1.0;
    else if(tmp < 0.0)
        audio_volume = 0.0;
    else
        audio_volume = tmp;
}

// function that samples a sine wave with given frequency and amplitude at given time
double sineWave(double time, double freq)
{
    double adaptFreq = freq / sample_rate;
    return sin(M_PI * 2 * adaptFreq * time);
}

// function that samples a square wave with given frequency and amplitude at given time
double squareWave(double time, double freq)
{
    double adaptFreq = freq / sample_rate;
    double result = sin(M_PI * 2 * adaptFreq * time);
    if(result > 0)
        return 1.0;
    else
        return -1.0;
}

// function that samples a triangle wave with given frequency and amplitude at given time
double triangleWave(double time, double freq)
{
    double adaptFreq = freq / sample_rate;
    return asin(sin(M_PI * 2 * adaptFreq * time));
}

// function that samples a saw wave with given frequency and amplitude at given time
double sawWave(double time, double freq)
{
    double adaptFreq = freq / sample_rate;
    return (2.0/M_PI) * (adaptFreq * M_PI * fmod(time, 1.0/adaptFreq) - (M_PI/2.0));
}

// SDL2 audio callback
void audioCallback(void* userdata, Uint8* stream, int len) // userdate can be used e.g. for passing the frequency, etc.
{
    len /= 2; // len is in bytes. We are using 16 bit signal <=> 2 Bytes one sample -> len == number of samples
    int16_t* buf = (int16_t*) stream;
    int16_t vol = audio_volume * INT16_MAX;
    for(int i = 0; i < len; ++i)
    {
        switch(oscillator)
        {
        case OSC_SINE:
        {
            double sum = 0.0;
            for(int n=0; n<15; ++n)
                sum += envelops[n].getAmplitude() * sineWave(audio_positions[n]++, voices[n]);
            buf[i] = vol * sum;
            break;
        }
        case OSC_TRIANGLE:
        {
            double sum = 0.0;
            for(int n=0; n<15; ++n)
                sum += envelops[n].getAmplitude() * triangleWave(audio_positions[n]++, voices[n]);
            buf[i] = vol * sum;
            break;
        }
        case OSC_SAW:
        {
            double sum = 0.0;
            for(int n=0; n<15; ++n)
                sum += envelops[n].getAmplitude() * sawWave(audio_positions[n]++, voices[n]);
            buf[i] = vol * sum;
            break;
        }
        case OSC_SQUARE:
        {
            double sum = 0.0;
            for(int n=0; n<15; ++n)
                sum += envelops[n].getAmplitude() * squareWave(audio_positions[n]++, voices[n]);
            buf[i] = vol * sum;
        }
        }
    }
}
