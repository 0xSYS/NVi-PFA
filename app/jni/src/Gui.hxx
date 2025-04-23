#pragma once

#include <SDL3/SDL.h>
#include "extern/imgui/imgui.h"
#include "Utils.hxx"
#include "Config_Utils.hxx"
#include "Nlist.hxx"





inline NVConf::configuration live_conf;
inline ImVec4 clear_color;
inline NVi::RGBAint liveColor;

inline std::vector<SoundfontItem> live_soundfont_list;
inline std::vector<std::string> checked_soundfonts;

static NVnoteList MIDI;


class NVGui
{
    public:
    
    //static ImGuiIO& io ;
    static void Setup(SDL_Window *w, SDL_Renderer *r);
    static void SetDefaultTheme();
    static void Run(SDL_Renderer *r);
    static std::vector<std::string> GetCheckedSoundfonts(const std::vector<SoundfontItem>& items);
};