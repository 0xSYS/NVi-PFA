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
    #define DEFAULT_MIDI "pfa_intro.mid"
#else
    #define CONFIG_PATH "/data/data/com.qsp.nvpfa/files/config.toml"
    #define MIDI_LIST "/data/data/com.qsp.nvpfa/files/midi_list.toml"
    #define SF_LIST "/data/data/com.qsp.nvpfa/files/soundfonts.toml"
    #define DEFAULT_SOUNDFONT "/data/data/com.qsp.nvpfa/files/piano_maganda.sf2"
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
        int window_w;
        int window_h;
        int bg_R;
        int bg_G;
        int bg_B;
        int bg_A;
        int vel_min;
        int vel_max;
        bool audio_limiter;
        bool vel_filter;
        bool use_default_paths;
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
