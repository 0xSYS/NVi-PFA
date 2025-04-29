#pragma once

#include "extern/audio/bass.h"
#include "extern/audio/bassmidi.h"



// Main audio stream
inline HSTREAM   Stm;


// Limiter settings
inline float limiter_threshold = 0.8f;
inline float limiter_knee = 0.05f;
inline float limiter_attack = 0.1f;     // attack speed (0 = instant, 1 = never)
inline float limiter_release = 0.0005f; // release speed
inline float current_gain = 1.0f;


void bassevt(DWORD type, DWORD param, DWORD chan, DWORD tick, DWORD time);

// Audio limiter
void CALLBACK dsp_limiter(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user);

// Ignore notes with specific velocity
BOOL CALLBACK filter(HSTREAM S, DWORD trk, BASS_MIDI_EVENT *E, BOOL sk, void *u);