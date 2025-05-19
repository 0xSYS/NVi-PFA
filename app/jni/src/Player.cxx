#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef __linux__
    #include <unistd.h>
#endif


#include "extern/audio/bassmidi.h"
#include "extern/imgui_sdl3/imgui_impl_sdl3.h"
#include "extern/cpptoml/cpptoml.h"




//#define NON_ANDROID // Temporary defined in here
//#define DEV_TEST // Use this for development purposes only

#include "common.hxx"
#include "audio_effects.hxx"
#include "Utils.hxx"
#include "file_utils.hxx"
#include "Config_Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"
#include "Gui.hxx"
#include "canvas.hxx"

#ifdef DEV_TEST
    #include "dev_test.h"
#endif


#if defined(_WIN32) || defined(_WIN64)
    #define BASSMIDI_LIB "bassmidi.dll"
#else
    #define BASSMIDI_LIB "libbassmidi.so"
#endif





std::vector<BASS_MIDI_FONT> font_list;
QWORD saved_position = 0; // For storing position when paused
const double SEEK_AMOUNT = 3.0; // 3 seconds for seeking
bool is_paused = false;

unsigned char events[6144];
int eventCount = 0;
NVnoteList nv_list;

const int frameRate = 60;
const int frameDelay = 1000 / frameRate;  // Calculate delay time for 60 FPS

Uint32 frameStart;
int frameTime;


SDL_Event Evt;


NVmidiFile M;
NVsequencer S;
NVi::u32_t nowtick;



SDL_Mutex *bass_mutex = nullptr;






void LoadInitialSoundfonts()
{
    std::vector<BASS_MIDI_FONT> fontSet;
    // Load the selected soundfonts
    if(checked_soundfonts.size() == 0)
    {
#ifndef NON_ANDROID
        std::string default_sf_path = NVFileUtils::GetFilePathA("piano_maganda.sf2", "rb");
        std::string default_gm_sf_path = NVFileUtils::GetFilePathA("gm_generic.sf2", "rbf");
        
        HSOUNDFONT Sf1 = BASS_MIDI_FontInit(default_gm_sf_path.c_str(), 0);
        HSOUNDFONT Sf2 = BASS_MIDI_FontInit(default_sf_path.c_str(), 0);
        
        fontSet.push_back({Sf2, -1, 0});    // override preset 40 (Violin) in bank 0
        fontSet.push_back({Sf1, -1, 0});
        
        BASS_MIDI_FontSetVolume(Sf1, 0.15);
        BASS_MIDI_FontSetVolume(Sf2, 0.15);
#else
        HSOUNDFONT Sf1 = BASS_MIDI_FontInit(DEFAULT_GM_SOUNDFONT, 0);
        HSOUNDFONT Sf2 = BASS_MIDI_FontInit(DEFAULT_SOUNDFONT, 0);
        
        fontSet.push_back({Sf2, -1, 0});    // override preset 40 (Violin) in bank 0
        fontSet.push_back({Sf1, -1, 0});
        
        BASS_MIDI_FontSetVolume(Sf1, 0.15);
        BASS_MIDI_FontSetVolume(Sf2, 0.15);
        
#endif
        
    } 
    else 
    {
        // Load all checked soundfonts
        fontSet.reserve(checked_soundfonts.size());
        
        for(int i = 0; i < checked_soundfonts.size(); i++)
        {
            HSOUNDFONT Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
            if(Sf)
            {
                BASS_MIDI_FontSetVolume(Sf, 0.15);
                BASS_MIDI_FONT font = {Sf, -1, 0};
                fontSet.push_back(font);
            }
            
            std::ostringstream sf_file_info_text;
            NVFileUtils::FileInfo sf_file = NVFileUtils::GetFileInfo(checked_soundfonts[i]);
            sf_file_info_text_arr.clear();
            sf_file_info_text << "File Name:         " << sf_file.file_name << "\n";
            sf_file_info_text << "Last Modified:   " << sf_file.last_mod << "\n";
            sf_file_info_text << "Size:                    " << sf_file.size << "\n";
            
            sf_file_info_text_arr.push_back(sf_file_info_text.str());
        } 
    }
    
    if(!fontSet.empty())
    {
        BASS_MIDI_StreamSetFonts(Stm, fontSet.data(), fontSet.size());
    }
}

void reloadSoundfonts()
{
    if (!Stm || !BASS_ChannelIsActive(Stm))
    {
        return; // No active stream to modify
    }
    
    // Save current playback position and state
    QWORD position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
    bool was_playing = !is_paused;
    
    // Get current midi file path
    std::string current_midi = parsed_config.last_midi_path;
    
    // Stop and free the current stream
    BASS_ChannelStop(Stm);
    BASS_StreamFree(Stm);
    
    // Create new stream with the same MIDI file
    Stm = BASS_StreamCreateFile(0, current_midi.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelSetDSP(Stm, &dsp_limiter, 0, 0);
    
    LoadInitialSoundfonts();
    
    // Restore other settings
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    
    // Restore position
    BASS_ChannelSetPosition(Stm, position, BASS_POS_BYTE);
    
    // Resume playback if it was playing before
    if(was_playing)
    {
        BASS_ChannelPlay(Stm, FALSE);
        is_paused = false;
    } 
    else
    {
        is_paused = true;
    }
    
    // Update our playback time
    Tplay = BASS_ChannelBytes2Seconds(Stm, position);

    NVConf::CreateSoundfontList(live_soundfont_list);
    
    // Log the change
    NVi::info("Player", "Soundfonts reloaded\n");
}


void updateBassVoiceCount(int voiceCount)
{
    if(Stm && BASS_ChannelIsActive(Stm))
    {
        // Update the voice count for the current stream
        BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, voiceCount);
        
        // Update the configuration
        parsed_config.bass_voice_count = voiceCount;
        
        // Optional: Log the change
        NVi::info("Player", "Voice count updated to %d\n", voiceCount);
    }
}


void loadMidiFile(const std::string& midi_path)
{
    
    SDL_LockMutex(bass_mutex);
    
    // Make sure the main window is closed before loading a new midi
    if(main_gui_window)
        main_gui_window = false;
    
    // Parse new MIDI file
    if(!MIDI.start_parse(midi_path.c_str()))
    {
        std::ostringstream temp_msg;
        temp_msg << "Failed to load '" << midi_path << "'";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", temp_msg.str().c_str(), nullptr);
        return;
    }
    
    // Create new stream
    Stm = BASS_StreamCreateFile(0, midi_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelSetDSP(Stm, &dsp_limiter, 0, 0);
    
    LoadInitialSoundfonts();
    
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);
    if(live_conf.vel_filter == true)
        BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    
    //sleep(1); // Sleep 1 second before starting playback
    
    // Start playback
    BASS_ChannelPlay(Stm, 1);
    
    // Reset playback variables
    Tplay = 0.0;
    is_paused = false;
    playback_ended = false; // Allow the playback to start with the audio playback
    
    // Update current midi path
    parsed_config.last_midi_path = midi_path;
    
    SDL_UnlockMutex(bass_mutex);
}

void NVi::CloseMIDI()
{
    // Stop current playback and free resources
    if(Stm)
    {
        BASS_ChannelStop(Stm);
        BASS_StreamFree(Stm);
    }
    
    // Reset note lists
    for (int i = 0; i < 128; ++i)
    {
        MIDI.L[i].clear();
    }
    
    // Clear previous channel / track colors
    CvWin->ClearTrackChannelColors();
}


// Function to handle seeking
void seek_playback(double seconds)
{
    QWORD current_byte_pos = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
    double current_time = BASS_ChannelBytes2Seconds(Stm, current_byte_pos);
    double new_time = current_time + seconds;
    
    // Ensure we don't seek before the beginning
    if(new_time < 0)
        new_time = 0;
    
    // Convert back to bytes and set position
    QWORD new_pos = BASS_ChannelSeconds2Bytes(Stm, new_time);
    BASS_ChannelSetPosition(Stm, new_pos, BASS_POS_BYTE);
    
    // Update our playback time
    Tplay = BASS_ChannelBytes2Seconds(Stm, new_pos);
    
    // When seeking backwards, reload the note data
    if(seconds < 0)
    {
        // Clear the note list
        for(int i = 0; i < 128; ++i)
        {
            MIDI.L[i].clear();
        }
        
        // Seek the list to the new position and update
        MIDI.list_seek(Tplay);
        MIDI.update_to(Tplay + Tscr);
    }
    
    if (playback_ended) 
	{
		// If at the end and trying to seek back, restart playback from desired position
		double new_time = Tplay + (-SEEK_AMOUNT);
		if(new_time < 0)
		    new_time = 0;
		
		QWORD new_pos = BASS_ChannelSeconds2Bytes(Stm, new_time);
		BASS_ChannelSetPosition(Stm, new_pos, BASS_POS_BYTE);
		BASS_ChannelPlay(Stm, FALSE);
		
		// Reset visualization properly
		for(int i = 0; i < 128; ++i) 
		{
			MIDI.L[i].clear();
		}
		
		// Update time and reset flags
		Tplay = new_time;
		MIDI.list_seek(Tplay);
		playback_ended = false;
		is_paused = false;
	}
}

// Function to toggle pause/play
void toggle_pause()
{
    if (is_paused)
    {
        // Resume playback
        BASS_ChannelPlay(Stm, false); // false means don't restart from beginning
        is_paused = false;
    } 
    else
    {
        // Pause playback
        saved_position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
        BASS_ChannelPause(Stm);
        is_paused = true;
    }
    
    if(playback_ended)
	{
		// If at the end and we press space, restart from beginning
		BASS_ChannelSetPosition(Stm, 0, BASS_POS_BYTE);
		BASS_ChannelPlay(Stm, FALSE);
		Tplay = 0.0;
		playback_ended = false;
		is_paused = false;
		
		// Reset visualization state
		for(int i = 0; i < 128; ++i)
		{
			MIDI.L[i].clear();
		}
		MIDI.list_seek(0);
	}
}


void NVi::Quit()
{
    // The canvas destructor occurs automatically so no need to delete it
    BASS_Free();
    BASS_PluginFree(0);
    MIDI.destroy_all();
    SDL_DestroyMutex(bass_mutex);
    exit(0);
}



void NVi::CreateMidiList()
{
    // Insert all midi and soundfont file paths
    
    std::vector<std::string> all_midi_files;
    std::ostringstream base_dir;
#ifdef NON_ANDROID
    base_dir << NVi::GetHomeDir() << "/Desktop/";
#else
    base_dir << BASE_DIRECTORY;
#endif

    for(const auto& path : live_conf.extra_midi_paths) 
    {
        auto mid_files       = NVFileUtils::GetFilesByExtension(path, ".mid");
        auto midi_files      = NVFileUtils::GetFilesByExtension(path, ".midi");
        auto smf_files       = NVFileUtils::GetFilesByExtension(path, ".smf");
        auto mid_caps_files  = NVFileUtils::GetFilesByExtension(path, ".MID");
        auto midi_caps_files = NVFileUtils::GetFilesByExtension(path, ".MIDI");
        auto smf_caps_files  = NVFileUtils::GetFilesByExtension(path, ".SMF");
        
        all_midi_files.insert(all_midi_files.end(), mid_files.begin(), mid_files.end());
        all_midi_files.insert(all_midi_files.end(), midi_files.begin(), midi_files.end());
        all_midi_files.insert(all_midi_files.end(), smf_files.begin(), smf_files.end());
        all_midi_files.insert(all_midi_files.end(), mid_caps_files.begin(), mid_caps_files.end());
        all_midi_files.insert(all_midi_files.end(), midi_caps_files.begin(), midi_caps_files.end());
        all_midi_files.insert(all_midi_files.end(), smf_caps_files.begin(), smf_caps_files.end());
    }
    
    if(live_conf.use_default_paths)
    {
        NVi::info("Player", "Default paths\n");
        //All possibilities
        auto mid_files       = NVFileUtils::GetFilesByExtension(base_dir.str(), ".mid");
        auto midi_files      = NVFileUtils::GetFilesByExtension(base_dir.str(), ".midi");
        auto smf_files       = NVFileUtils::GetFilesByExtension(base_dir.str(), ".smf");
        auto mid_caps_files  = NVFileUtils::GetFilesByExtension(base_dir.str(), ".MID");
        auto midi_caps_files = NVFileUtils::GetFilesByExtension(base_dir.str(), ".MIDI");
        auto smf_caps_files  = NVFileUtils::GetFilesByExtension(base_dir.str(), ".SMF");
        
        all_midi_files.insert(all_midi_files.end(), mid_files.begin(), mid_files.end());
        all_midi_files.insert(all_midi_files.end(), midi_files.begin(), midi_files.end());
        all_midi_files.insert(all_midi_files.end(), smf_files.begin(), smf_files.end());
        all_midi_files.insert(all_midi_files.end(), mid_caps_files.begin(), mid_caps_files.end());
        all_midi_files.insert(all_midi_files.end(), midi_caps_files.begin(), midi_caps_files.end());
        all_midi_files.insert(all_midi_files.end(), smf_caps_files.begin(), smf_caps_files.end());
    }
    
    live_midi_list.resize(all_midi_files.size());
    
    for(int i = 0; i < live_midi_list.size(); i++)
    {
        live_midi_list[i] = all_midi_files[i];
    }
     
    auto root = cpptoml::make_table();
    
    auto midi_files_out = cpptoml::make_array();
    
    for(const auto& path : live_midi_list) 
    {
        midi_files_out->push_back(path);
    }
    
    root->insert("midi_files", midi_files_out);
    
    // Save list
    std::ofstream out(MIDI_LIST);
    if(out.is_open())
    {
        out << "# DO NOT EDIT THIS FILE!!!\n";
        out << "# Created by: NVi Piano From Above\n";
        out << "# Description: Contains the cached midi list for easier access\n\n\n\n";
        out << (*root);
        NVi::info("Player", "Midi list saved\n");
    } 
    else 
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "Failed to save midi list", nullptr);
    }
}

void NVi::ReadMidiList()
{
    try 
    {
        auto config = cpptoml::parse_file(MIDI_LIST);
        if(auto array = config->get_array_of<std::string>("midi_files"))
        {
            live_midi_list = *array;
        } 
        else 
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "'midi_files' array not found!!", nullptr);
            NVi::error("Player", "'midi_files' array not found or invalid");
            //exit(1); // Kinda stupid
        }
    } // Try to catch parsing errros
    catch(const cpptoml::parse_exception& e)
    {
        std::ostringstream msg;
        msg << "TOML Parse Error: " << e.what();
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", msg.str().c_str(), nullptr);
        NVi::error("Player", "TOML Parse error: %s", e.what());
        exit(1);
    }
}


void NVi::RefreshSFList()
{
    std::ostringstream base_dir;
#ifdef NON_ANDROID
    base_dir << NVi::GetHomeDir() << "/Desktop/";
#else 
    base_dir << BASE_DIRECTORY;
#endif

    live_soundfont_files.clear();
    live_soundfont_list.clear();    

    for(const auto& path : live_conf.extra_sf_paths)
    {
        auto sf2_files = NVFileUtils::GetFilesByExtension(path, ".sf2");
        live_soundfont_files.insert(live_soundfont_files.end(), sf2_files.begin(), sf2_files.end());
    
        auto sfz_files = NVFileUtils::GetFilesByExtension(path, ".sfz");
        live_soundfont_files.insert(live_soundfont_files.end(), sfz_files.begin(), sfz_files.end());
    }


    if(live_conf.use_default_paths)
    {
        auto sf2_files = NVFileUtils::GetFilesByExtension(base_dir.str(), ".sf2");
        live_soundfont_files.insert(live_soundfont_files.end(), sf2_files.begin(), sf2_files.end());
        
        auto sfz_files = NVFileUtils::GetFilesByExtension(base_dir.str(), ".sfz");
        live_soundfont_files.insert(live_soundfont_files.end(), sfz_files.begin(), sfz_files.end());
        
        live_soundfont_list.resize(live_soundfont_files.size());
        for(size_t i = 0; i < live_soundfont_files.size(); i++)
        {
            live_soundfont_list[i] = {live_soundfont_files[i], false};
        }
    }
    NVConf::CreateSoundfontList(live_soundfont_list);
}


std::vector<NVi::AudioDevice> NVi::GetAudioOutputs()
{
    std::vector<AudioDevice> res;
    BASS_DEVICEINFO dev_info;
    int deviceIndex = 0;
    
    while(BASS_GetDeviceInfo(deviceIndex, &dev_info))
    {
        deviceIndex++;
        res.push_back({deviceIndex, dev_info.name, dev_info.driver, (dev_info.flags & BASS_DEVICE_DEFAULT) ? true : false, (dev_info.flags & BASS_DEVICE_ENABLED) ? true : false});
    }
   
    return res;
}





#ifdef NON_ANDROID
int main(int ac, char **av)
#else
int SDL_main(int ac, char **av)
#endif
{
#ifndef DEV_TEST
    const char * midi_path = av[1];
    const char * sf_path = av[2];
   
// Get all needed assets after installing the app 
#ifndef NON_ANDROID
    std::string default_sf_path = NVFileUtils::GetFilePathA("piano_maganda.sf2", "rb");
    std::string default_midi_path = NVFileUtils::GetFilePathA("pfa_intro.mid", "rb");
    std::string ui_font = NVFileUtils::GetFilePathA("ui_font.ttf", "rb");
#endif
    if(NVFileUtils::FileExists(CONFIG_PATH) == true)
    {
        // Read Config and assign settings structure
        is_defaultconfig = false;
        parsed_config = NVConf::ReadConfig();
        live_conf = parsed_config;
        liveColor.r = parsed_config.bg_R;
        liveColor.g = parsed_config.bg_G;
        liveColor.b = parsed_config.bg_B;
        liveColor.a = parsed_config.bg_A;
        live_note_speed = parsed_config.note_speed;
        current_audio_dev = live_conf.audio_device_index; // Used to set the last selected audio device in the combobox
        loop_colors = live_conf.loop_colors;
        overlap_remover = live_conf.OR;
        use_default_media_paths = live_conf.use_default_paths;
        
        selIndex = live_conf.midi_index;
    }
    else
    {
        // If not assign default configuration
        is_defaultconfig = true;
        default_config.bass_voice_count = 500;
#ifndef NON_ANDROID
        default_config.current_soundfonts = {default_sf_path};
#endif
        default_config.bg_R = 48;
        default_config.bg_G = 48;
        default_config.bg_B = 48;
        default_config.bg_A = 255;
        default_config.loop_colors = false;
        default_config.note_speed = 6000;
        default_config.audio_device_index = -1; // Also set default audio device output
        default_config.vel_filter = true;
        default_config.vel_min = 0;
        default_config.vel_max = 32;
        selIndex = 0;
        //default_config.channel_colors[15] = Col[15];
        for(int i = 0; i < 15; i++)
        {
            ui_chcolors[i] = UIntToImVec4(Col[i]);
        }
        liveColor.r = default_config.bg_R;
        liveColor.g = default_config.bg_G;
        liveColor.b = default_config.bg_B;
        liveColor.a = default_config.bg_A;
        live_conf = default_config;
        
        // Update settings to UI
        live_note_speed = default_config.note_speed; // Note speed should never reset to 0
        velocity_filter = default_config.vel_filter;
        min_velocity = default_config.vel_min;
        max_velocity = default_config.vel_max;
    }
    
    if(NVFileUtils::FileExists(MIDI_LIST) == true)
    {
        NVi::ReadMidiList();
    }
    else
    {
        NVi::CreateMidiList();
    }
    
    if(NVFileUtils::FileExists(SF_LIST) == true)
    {
        live_soundfont_list = NVConf::ReadSoundfontList();
        checked_soundfonts = NVGui::GetCheckedSoundfonts(live_soundfont_list);
    }
    else
    {
        NVi::RefreshSFList();
    }
    
    if(parsed_config.last_midi_path.length() == 0)
    {
        parsed_config.last_midi_path = DEFAULT_MIDI;
    }
    
    // 0 note speed results in invisible notes and extreme lag so it's best to show a message box error instead
    if(live_note_speed == 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Note speed cannot be zero !!!!", NULL);
        exit(1);
    }
    
    bass_mutex = SDL_CreateMutex();
    

    CvWin = new Canvas;
    
    double StartDelay = 1.0;
    
    // Set the last background color
    SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
    
    availableAudioDevices = NVi::GetAudioOutputs();

    // Bass, bassmidi stuff
    BASS_PluginLoad(BASSMIDI_LIB, 0);
    BASS_SetConfig(BASS_CONFIG_BUFFER, 5000); // Set buffer size to 500ms
    BASS_SetConfig(BASS_CONFIG_MIDI_AUTOFONT, 0);
    //NVi::info("Player", "array size: %d\n", availableAudioDevices.size());
    
    if(availableAudioDevices.size() == 0)
    {
        NVi::warn("Player", "No audio devices found!!!\n");
        NVi::info("Player", "Using default audio device (-1)\n");
        BASS_Init(-1, 44100, 0, 0, nullptr);
    }
    else
    {
        if(!BASS_Init(live_conf.audio_device_index, 44100, 0, 0, nullptr))
        {
            // Initialization failed, get the error code
           int errorCode = BASS_ErrorGetCode();
           std::ostringstream msg_str;
    
            // Handle specific error codes
            switch(errorCode)
            {
                case BASS_ERROR_DEVICE:
                    msg_str << "Device index is invalid\n";
                    break;
                case BASS_ERROR_ALREADY:
                    msg_str << "BASS Already initialized\n";
                    break;
                case BASS_ERROR_DRIVER:
                    msg_str << "Unavailable device driver\n";
                    break;
                case BASS_ERROR_FORMAT:
                    msg_str << "Unsupported format by device\n";
                    break;
                case BASS_ERROR_MEM:
                    msg_str << "Insufficient memory\n";
                    break;
                case BASS_ERROR_NO3D:
                    msg_str << "Failed to initialize 3D support\n";
                    break;
                case BASS_ERROR_UNKNOWN:
                    msg_str << "Unknown error occured!!!\n";
                    break;
                default:
                    msg_str << "Unhandled error code!!!\n";
                    break;
            }
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Bass Init Error", msg_str.str().c_str(), NULL);
        }
        else
        {
            int currentDeviceIndex = BASS_GetDevice();
            BASS_DEVICEINFO deviceInfo;
            if (BASS_GetDeviceInfo(currentDeviceIndex, &deviceInfo))
            {
                NVi::info("Player", "BASS Successfully Initialized with audio device:\nName: %s\nDriver: %s\nDefault: %s\nEnabled: %s\nIndex: %d\n", deviceInfo.name, deviceInfo.driver, (deviceInfo.flags & BASS_DEVICE_DEFAULT) ? "Yes" : "No", (deviceInfo.flags & BASS_DEVICE_ENABLED) ? "Yes" : "No", currentDeviceIndex);
            }
        }
    }
    
    // Load the last midi file
    loadMidiFile(parsed_config.last_midi_path);
    
    if(live_conf.OR)
        nv_list.OR(); // Overlap remover in action
    
    
    const unsigned int FPS=1000000/CvWin->mod->refresh_rate;// 20 may be replaced with a limited frame rate

    
    // Allow live change of the backgoround color
   	SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
    
    /*
    App Main loop
    */
    while(true) // Keep running regardless of playback state
	{
		// Check if playback just ended (and we need to handle that)
		if(!playback_ended && BASS_ChannelIsActive(Stm) == BASS_ACTIVE_STOPPED)
		{
			playback_ended = true;
			Tplay = 1.0; // Add a bit more to fully finish the note visualization
			is_paused = true; // Just mark as paused when it ends
			// Save the position at the end
			saved_position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
		}
		
		_WinH = CvWin->WinH - CvWin->WinW * 80 / 1000; Tscr = (double)_WinH / live_note_speed;
		
		// Start visualizing only if bass thread is ready
		if(BASS_ChannelIsActive(Stm))
		{
		    is_playback_started = true;
		    MIDI.update_to(Tplay + Tscr);
		    MIDI.remove_to(Tplay);
		}
		else
		{
		    is_playback_started = false;
		}
		
		CvWin->canvas_clear();
		
		while(SDL_PollEvent(&Evt))
		{
            frameStart = SDL_GetTicks();  // Get the current time in milliseconds
			ImGui_ImplSDL3_ProcessEvent(&Evt);
			if (Evt.type == SDL_EVENT_QUIT)
				NVi::Quit();
		    	
#ifdef NON_ANDROID
            // Allow keyboard input only if the main window is not on display
            // This avoids playback control interferance when typing into the search boxes
            if(!main_gui_window)
            {
			    if(Evt.type == SDL_EVENT_KEY_DOWN) 
			    {
		    	    switch(Evt.key.key)
		    	    {
	    		        case SDLK_SPACE:
				        	toggle_pause();
				        	break;
	    		        case SDLK_LEFT:
				        	seek_playback(-SEEK_AMOUNT);
				        	break;
	    		        case SDLK_RIGHT:
				        	seek_playback(SEEK_AMOUNT);
				        	break;
	    		        case SDLK_Q:
				        	NVi::Quit();
				        	break;
		    	    }
			    }
            }
#endif
		}
		
		SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
		
		// Always draw notes
		for(int i = 0; i != 128; ++i)
		{
			for(const NVnote &n : MIDI.L[KeyMap[i]])
			{
				CvWin->DrawNote(i, n, live_note_speed);
			}
		}
		
		CvWin->DrawKeyBoard();
		NVGui::Run(CvWin->Ren);
        frameTime = SDL_GetTicks() - frameStart;  // Time taken for one frame

        if(frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);  // Delay to maintain consistent frame rate
        }
		SDL_RenderPresent(CvWin->Ren);
		
		// Only update Tplay if actively playing and not at the end
		if (!is_paused && !playback_ended)
		{
			Tplay = BASS_ChannelBytes2Seconds(Stm, BASS_ChannelGetPosition(Stm, BASS_POS_BYTE));
		}
		
		const char * sdl_err = SDL_GetError();
		if(strlen(sdl_err) << 1)
		{
			SDL_Log("SDL Error: %s", sdl_err);
		}
	}
#else
    TestStuff();
#endif
    return 0;
}