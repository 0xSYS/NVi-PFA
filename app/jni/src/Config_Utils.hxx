// Author: 0xsys (20.04.2025)
#pragma once


//#include "Gui.hxx"
#include <vector>
#include <string>
#include <stdint.h>

//#define NON_ANDROID

#ifdef NON_ANDROID
    #define CONFIG_PATH "config.toml"
    #define MIDI_LIST "midi_list.toml"
    #define SF_LIST "soundfonts.toml"
    #define DEFAULT_SOUNDFONT "piano_maganda.sf2"
    #define DEFAULT_GM_SOUNDFONT "gm_generic.sf2"
    #define DEFAULT_MIDI "pfa_intro.mid"
#else
    #define CONFIG_PATH "/data/data/com.qsp.nvpfa/files/config.toml"
    #define MIDI_LIST "/data/data/com.qsp.nvpfa/files/midi_list.toml"
    #define SF_LIST "/data/data/com.qsp.nvpfa/files/soundfonts.toml"
    #define DEFAULT_SOUNDFONT "/data/data/com.qsp.nvpfa/files/piano_maganda.sf2"
    #define DEFAULT_GM_SOUNDFONT "/data/data/com.qsp.nvpfa/files/gm_generic.sf2"
    #define DEFAULT_MIDI "/data/data/com.qsp.nvpfa/files/pfa_intro.mid"
#endif




struct SoundfontItem 
{
    std::string label;
    bool checked = false;
};



class NVConf
{
    public:
    typedef struct
    {
        int bass_voice_count;
        int audio_device_index;
        int note_speed;
        // Deprecated
        //int window_w;
        //int window_h;
        int bg_R;
        int bg_G;
        int bg_B;
        int bg_A;
        unsigned int channel_colors[16];
        int vel_min;
        int vel_max;
        int midi_index;
        bool audio_limiter;
        bool vel_filter;
        bool use_default_paths;
        bool is_custom_ch_colors; // Only for internal use!!!
        bool use_default_colors; // Only for internal use!!!
        bool loop_colors;
        bool OR; // Overlap remover
        std::string last_midi_path;
        std::vector<std::string> current_soundfonts;
        std::vector<std::string> extra_midi_paths;
        std::vector<std::string> extra_sf_paths;
    }configuration;
    
    static configuration ReadConfig();
    static void WriteConfig(configuration cfg);
    
    static void CreateSoundfontList(std::vector<SoundfontItem> lst);
    static std::vector<SoundfontItem> ReadSoundfontList();
    
};
