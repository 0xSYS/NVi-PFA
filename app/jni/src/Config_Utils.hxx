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
    #define DEFAULT_MIDI "PFA_Intro.mid"
#else
    #define CONFIG_PATH "/data/data/com.qsp.nvpfa/files/config.toml"
    #define MIDI_LIST "/data/data/com.qsp.nvpfa/files/midi_list.toml"
    #define SF_LIST "/data/data/com.qsp.nvpfa/files/soundfonts.toml"
    #define DEFAULT_SOUNDFONT "/data/data/com.qsp.nvpfa/files/piano_maganda.sfS2"
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
        int window_w; // Custom width and height for the SDL window
        int window_h;
        int bg_R;
        int bg_G;
        int bg_B;
        int bg_A;
        std::string last_midi_path;
        std::vector<std::string> current_soundfonts;
    }configuration;
    
    static configuration ReadConfig();
    static void WriteConfig(configuration cfg);
    //static void ReadMidiLists();
    
    static void CreateSoundfontList(std::vector<SoundfontItem> lst);
    static std::vector<SoundfontItem> ReadSoundfontList();
    
    //static void CreateMidiFileList(std::vector<std::string> lst);
    //static std::vector<std::string> ReadMidiList();
    
};
