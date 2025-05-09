// <NVirsual> Utils.hxx 2021-11-18 by 云中龙++

#pragma once

#include <cstddef>
//#include <iostream>

//#include "Gui.hxx"
#include "Config_Utils.hxx"
#include "extern/imgui/imgui.h"
#include "canvas.hxx"
//#include "Nlist.hxx"
//#include "canvas.hxx"

inline NVConf::configuration parsed_config;
inline NVConf::configuration default_config;
inline std::vector<std::string> live_midi_list;
inline std::vector<std::string> live_soundfont_files;
inline bool is_midi_loaded;
inline std::string current_midi_file;
inline bool main_gui_window = false;
static Canvas     *Win;
//static NVnoteList MIDI;



namespace  NVi   /* ===== Tool function namespace ===== */
{
    typedef struct
    {
        int r;
        int g;
        int b;
        int a;
    }RGBAint;
    
    using nv_ul64 = unsigned long long;
    using u32_t   = unsigned int;
    using u16_t   = unsigned short;
    using nv_byte = unsigned char;

    
    
    void Quit();
    
    //void StartBassPlayback(std::string midi_path, std::vector<std::string> soundfonts);
    
    void CheckAssets();
    
    void CreateMidiList();
    void ReadMidiList();
    void RefreshSFList();
    //void RefreshMIDIList();
    
    // Float rgb to int rgb
    RGBAint Frgba2Irgba(ImVec4& col);


    /* Display Error */
    void error(const char *prefix, const char *str, ...);

    /* Display Warning */
    void warn(const char *prefix, const char *str, ...);

    /* Display Info */
    void info(const char *prefix, const char *str, ...);

    using std::size_t;

    /* Mapping strings to big-endian integers */
    nv_ul64 operator"" _u64be(const char *str, size_t n);


    void revU16(u16_t &x); // Reversing the end-order of a 16-bit integer variable

    void revU32(u32_t &x); // Reversing the end-order of a 32-bit integer variable
};