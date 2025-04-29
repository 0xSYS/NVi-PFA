
#include <cmath>

#include "extern/audio/bass.h"
#include "extern/audio/bassmidi.h"

#include "audio_effects.hxx"



void bassevt(DWORD type, DWORD param, DWORD chan, DWORD tick, DWORD time)
{
    BASS_MIDI_EVENT evt = {type, param, chan, tick, time};
    BASS_MIDI_StreamEvents(Stm, BASS_MIDI_EVENTS_STRUCT | BASS_MIDI_EVENTS_NORSTATUS | BASS_MIDI_EVENTS_CANCEL, &evt, 1);
}


void CALLBACK dsp_limiter(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user)
{
    float *samples = (float*)buffer;
    DWORD count = length / sizeof(float);
   
    for (DWORD i = 0; i < count; ++i)
    {
        float input = samples[i];
        float abs_input = fabs(input);
   
        float desired_gain = 1.0f;
   
        if (abs_input > limiter_threshold)
        {
            float exceed = abs_input - limiter_threshold;
            float compressed = exceed / (exceed + limiter_knee);
            desired_gain = limiter_threshold / (limiter_threshold + compressed);
        }
   
        // Smoothly approach desired gain
        if (desired_gain < current_gain)
            current_gain += (desired_gain - current_gain) * limiter_attack; // attack smoothing
        else
            current_gain += (desired_gain - current_gain) * limiter_release; // release smoothing
   
        samples[i] *= current_gain;
    }
}


BOOL CALLBACK filter(HSTREAM S, DWORD trk, BASS_MIDI_EVENT *E, BOOL sk, void *u)
{
    if (E->event == MIDI_EVENT_NOTE)
    {
        int vel = HIBYTE(E->param);
        return vel == 0 || vel > 9;
    }
    return TRUE;
}