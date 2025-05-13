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
static inline int min_velocity;
static inline int max_velocity;
inline int current_audio_dev;
inline bool velocity_filter = true;
inline bool loop_colors = false;

inline ImVec4 ch_col1;
inline ImVec4 ch_col2;
inline ImVec4 ch_col3;
inline ImVec4 ch_col4;
inline ImVec4 ch_col5;
inline ImVec4 ch_col6;
inline ImVec4 ch_col7;
inline ImVec4 ch_col8;
inline ImVec4 ch_col9;
inline ImVec4 ch_col10;
inline ImVec4 ch_col11;
inline ImVec4 ch_col12;
inline ImVec4 ch_col13;
inline ImVec4 ch_col14;
inline ImVec4 ch_col15;
inline ImVec4 ch_col16;

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