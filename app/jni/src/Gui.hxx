#pragma once

#include <SDL3/SDL.h>
#include "extern/imgui/imgui.h"
#include "Utils.hxx"
#include "Config_Utils.hxx"
#include "Nlist.hxx"





// Add these declarations for the playback control
extern void reloadSoundfonts();
extern void updateBassVoiceCount(int voiceCount);
extern void loadMidiFile(const std::string& midi_path);

extern bool is_paused;
extern const double SEEK_AMOUNT; // Add this line
extern void seek_playback(double seconds);
extern void toggle_pause();

inline std::vector<NVi::AudioDevice> availableAudioDevices;
inline NVConf::configuration live_conf;
inline ImVec4 clear_color;
inline NVi::RGBAint liveColor;
inline int live_note_speed = 6000;
inline int current_audio_dev;

inline std::vector<SoundfontItem> live_soundfont_list;
inline std::vector<std::string> checked_soundfonts;

static NVnoteList MIDI;

class NVGui
{
    public:
        static void Setup(SDL_Window *w, SDL_Renderer *r);
        static void SetDefaultTheme();
        static void Run(SDL_Renderer *r);
        static std::vector<std::string> GetCheckedSoundfonts(const std::vector<SoundfontItem>& items);
};