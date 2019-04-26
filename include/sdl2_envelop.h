#ifndef SDL2_ENVELOP_H
#define SDL2_ENVELOP_H

#include <stdio.h>
#include <cstdlib>
#include "SDL2/SDL.h"

class Envelop
{
public:
    // time parameters
    double t_attack, t_decay, t_release;
    Uint64 t_keydown, t_keyup;
    // amplitude parameters
    double amp_start, amp_sustain;
    // flag to check if key is pressed
    bool b_keydown;

    // default constructor
    Envelop()
    {
        t_attack = 0.1;
        t_decay = 0.01;
        t_release = 0.2;
        amp_start = 1.0;
        amp_sustain = 0.8;
        b_keydown = false;
        t_keydown = 0.0;
        t_keyup = 0.0;
    }

    void onKeydown()
    {
        if(!b_keydown)
        {
            t_keydown = SDL_GetPerformanceCounter();
            b_keydown   = true;
        }
    }

    void onKeyup()
    {
        t_keyup = SDL_GetPerformanceCounter();
        b_keydown = false;
    }

    double getAmplitude()
    {
        Uint64 t = SDL_GetPerformanceCounter();
        double amp = 0.0;

        if(b_keydown)
        {
            double t_rel = (double)((t - t_keydown)) / SDL_GetPerformanceFrequency();

            if(t_rel <= t_attack) // attack phase
                amp = (t_rel / t_attack) * amp_start;
            else if(t_rel > t_attack && t_rel <= (t_attack + t_decay)) // decay phase
                amp = amp_start + ((t_rel - t_attack) / t_decay) * (amp_sustain - amp_start);
            else // sustain phase
                amp = amp_sustain; // model sustain phase with constant amplitude for simplicity

        }
        else // release phase
        {
            // key is released, so let tone fade out
            double t_rel = (double)((t - t_keyup)) / SDL_GetPerformanceFrequency();
            amp = amp_sustain - ((t_rel / t_release) * amp_sustain);
        }

        // avoid negative amplitudes, so cut off early on
        if(amp < 1e-5) amp = 0.0;

        /* return 0.0; */
        return amp;
    }

};

#endif
