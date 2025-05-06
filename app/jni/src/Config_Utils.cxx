#include <unistd.h>
#include <SDL3/SDL.h>
#include "extern/cpptoml/cpptoml.h"


//#define NON_ANDROID

#include "Utils.hxx"
#include "Config_Utils.hxx"





/*
Changed my mind bree
I won't use json anymore >:D
*/

void NVConf::WriteConfig(configuration cfg)
{
    auto out_cfg = cpptoml::make_table();
    
    auto audio = cpptoml::make_table();
    audio->insert("VoiceCount", cfg.bass_voice_count);
    audio->insert("LastMIDIpath", cfg.last_midi_path);
    audio->insert("DeviceID", cfg.audio_device_index);
    out_cfg->insert("Audio", audio);
    
    auto vis = cpptoml::make_table();
    vis->insert("NoteSpeed", cfg.note_speed);
    vis->insert("Window_w", cfg.window_w);
    vis->insert("Window_h", cfg.window_h);
    out_cfg->insert("Visual", vis);
    
    auto bg_col = cpptoml::make_table();
    bg_col->insert("R", cfg.bg_R);
    bg_col->insert("G", cfg.bg_G);
    bg_col->insert("B", cfg.bg_B);
    bg_col->insert("A", cfg.bg_A);
    out_cfg->insert("BackgroundColor", bg_col);
    
    auto midi_paths_out = cpptoml::make_array();
    
    for (const auto& path : cfg.extra_midi_paths) 
    {
        midi_paths_out->push_back(path);
    }
    
    out_cfg->insert("midi_paths", midi_paths_out);
    
    auto sf_paths_out = cpptoml::make_array();
    
    for (const auto& path : cfg.extra_sf_paths) 
    {
        sf_paths_out->push_back(path);
    }
    
    out_cfg->insert("soundfonts_paths", sf_paths_out);
    
    std::ofstream out(CONFIG_PATH);
    out << (*out_cfg); // cpptoml overloads the << operator
    out.close();
    NVi::info("Config_Utils", "Settings saved\n");
}

NVConf::configuration NVConf::ReadConfig()
{
    NVConf::configuration in_cfg;
    auto cfg = cpptoml::parse_file(CONFIG_PATH);
    
    auto audio = cfg->get_table("Audio");
    in_cfg.bass_voice_count = *audio->get_as<int>("VoiceCount");
    in_cfg.last_midi_path = *audio->get_as<std::string>("LastMIDIpath");
    in_cfg.audio_device_index = *audio->get_as<int>("DeviceID");
    
    auto vis = cfg->get_table("Visual");
    in_cfg.window_w = *vis->get_as<int>("Window_w");
    in_cfg.window_h = *vis->get_as<int>("Window_h");
    in_cfg.note_speed = *vis->get_as<int>("NoteSpeed");
    
    auto bg_col = cfg->get_table("BackgroundColor");
    in_cfg.bg_R = *bg_col->get_as<int>("R");
    in_cfg.bg_G = *bg_col->get_as<int>("G");
    in_cfg.bg_B = *bg_col->get_as<int>("B");
    in_cfg.bg_A = *bg_col->get_as<int>("A");
    
    auto midi_paths = cfg->get_array_of<std::string>("midi_paths");
    in_cfg.extra_midi_paths = *midi_paths;
    
    auto sf_paths = cfg->get_array_of<std::string>("soundfonts_paths");
    in_cfg.extra_sf_paths = *sf_paths;
   
    
    return in_cfg;
}


void NVConf::CreateSoundfontList(std::vector<SoundfontItem> lst)
{
    auto out_sf_list= cpptoml::make_table();
    
    auto sf_path = cpptoml::make_array();
    for (const auto& path : lst) 
    {
        sf_path->push_back(path.label);
    }
    
    auto enabled = cpptoml::make_array();
    for(const auto& en : lst)
    {
        enabled->push_back(en.checked);
    }

    out_sf_list->insert("paths", sf_path);
    out_sf_list->insert("enabled_disabled", enabled);
    
    std::ofstream out(SF_LIST);
    if (out.is_open()) 
    {
        out << (*out_sf_list);
    } 
    else 
    {
        NVi::error("Config_Utils", "Failed to write 'soundfonts.toml'\n");
    }
}

std::vector<SoundfontItem> NVConf::ReadSoundfontList()
{
    std::vector<SoundfontItem> lst;
    auto in_sf_list = cpptoml::parse_file(SF_LIST);
    
    auto paths = in_sf_list->get_array_of<std::string>("paths");
    auto enabled = in_sf_list->get_array_of<bool>("enabled_disabled");
    
    if (!paths || !enabled || paths->size() != enabled->size()) 
    {
        NVi::error("Config_Utils", "Invalid or mismatched arrays in 'soundfonts.toml'\n");
        return lst;
    }
    
    for (size_t i = 0; i < paths->size(); ++i) 
    {
        lst.push_back(SoundfontItem{(*paths)[i], (*enabled)[i]});
    }
    return lst;
}