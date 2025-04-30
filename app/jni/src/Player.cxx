#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <limits>


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
bool playback_ended = false;

unsigned char events[6144];
int eventCount = 0;


SDL_Event Evt;


NVmidiFile M;
NVsequencer S;
NVi::u32_t nowtick;
//Canvas cv;






void reloadSoundfonts() {
    if (!Stm || !BASS_ChannelIsActive(Stm)) {
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
    
    // Load the selected soundfonts
    if (checked_soundfonts.size() == 0) {
#ifndef NON_ANDROID
        std::string default_sf_path = NVFileUtils::GetFilePathA("piano_maganda.sf2", "rb");
        HSOUNDFONT Sf = BASS_MIDI_FontInit(default_sf_path.c_str(), 0);
#else
        HSOUNDFONT Sf = BASS_MIDI_FontInit(DEFAULT_SOUNDFONT, 0);
#endif
        
        BASS_MIDI_FONT FontSet{Sf, -1, 0};
        BASS_MIDI_FontSetVolume(Sf, 0.15);
        BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);
    } 
    else {
        // Load all checked soundfonts
        std::vector<BASS_MIDI_FONT> fontSet;
        fontSet.reserve(checked_soundfonts.size());
        
        for (int i = 0; i < checked_soundfonts.size(); i++) {
            HSOUNDFONT Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
            if (Sf) {
                BASS_MIDI_FontSetVolume(Sf, 0.15);
                BASS_MIDI_FONT font = {Sf, -1, 0};
                fontSet.push_back(font);
            }
        }
        
        if (!fontSet.empty()) {
            BASS_MIDI_StreamSetFonts(Stm, fontSet.data(), fontSet.size());
        }
    }
    
    // Restore other settings
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    
    // Restore position
    BASS_ChannelSetPosition(Stm, position, BASS_POS_BYTE);
    
    // Resume playback if it was playing before
    if (was_playing) {
        BASS_ChannelPlay(Stm, FALSE);
        is_paused = false;
    } else {
        is_paused = true;
    }
    
    // Update our playback time
    Tplay = BASS_ChannelBytes2Seconds(Stm, position);
    
    // Log the change
    NVi::info("Player", "Soundfonts reloaded\n");
}


void updateBassVoiceCount(int voiceCount) {
    if (Stm && BASS_ChannelIsActive(Stm)) {
        // Update the voice count for the current stream
        BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, voiceCount);
        
        // Update the configuration
        parsed_config.bass_voice_count = voiceCount;
        
        // Optional: Log the change
        NVi::info("Player", "Voice count updated to %d\n", voiceCount);
    }
}


void loadMidiFile(const std::string& midi_path) {
    // Stop current playback and free resources
    if (Stm) {
        BASS_ChannelStop(Stm);
        BASS_StreamFree(Stm);
    }
    
    // Reset note lists
    for (int i = 0; i < 128; ++i) {
        MIDI.L[i].clear();
    }
    
    // Parse new MIDI file
    if (!MIDI.start_parse(midi_path.c_str())) {
        Canvas C;
        std::ostringstream temp_msg;
        temp_msg << "Failed to load '" << midi_path << "'";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", temp_msg.str().c_str(), nullptr);
        return;
    }
    
    // Create new stream
    Stm = BASS_StreamCreateFile(0, midi_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelSetDSP(Stm, &dsp_limiter, 0, 0);
    
    // Setup soundfonts
    if (checked_soundfonts.size() == 0) {
#ifndef NON_ANDROID
        std::string default_sf_path = NVFileUtils::GetFilePathA("piano_maganda.sf2", "rb");
        HSOUNDFONT Sf = BASS_MIDI_FontInit(default_sf_path.c_str(), 0);
#else
        HSOUNDFONT Sf = BASS_MIDI_FontInit(DEFAULT_SOUNDFONT, 0);
#endif
        BASS_MIDI_FONT FontSet{Sf, -1, 0};
        BASS_MIDI_FontSetVolume(Sf, 0.15);
        BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);
    } 
    else 
    {
        // Load all checked soundfonts
        std::vector<BASS_MIDI_FONT> fontSet;
        fontSet.reserve(checked_soundfonts.size());
        
        for (int i = 0; i < checked_soundfonts.size(); i++) 
        {
            HSOUNDFONT Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
            if (Sf) 
            {
                BASS_MIDI_FontSetVolume(Sf, 0.15);
                BASS_MIDI_FONT font = {Sf, -1, 0};
                fontSet.push_back(font);
            }
        }
        
        if (!fontSet.empty()) 
        {
            BASS_MIDI_StreamSetFonts(Stm, fontSet.data(), fontSet.size());
        }
    }
    
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    
    // Start playback
    BASS_ChannelPlay(Stm, 1);
    
    // Reset playback variables
    Tplay = 0.0;
    is_paused = false;
    
    // Update current midi path
    parsed_config.last_midi_path = midi_path;
}


// Function to handle seeking
void seek_playback(double seconds) {
    QWORD current_byte_pos = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
    double current_time = BASS_ChannelBytes2Seconds(Stm, current_byte_pos);
    double new_time = current_time + seconds;
    
    // Ensure we don't seek before the beginning
    if (new_time < 0) new_time = 0;
    
    // Convert back to bytes and set position
    QWORD new_pos = BASS_ChannelSeconds2Bytes(Stm, new_time);
    BASS_ChannelSetPosition(Stm, new_pos, BASS_POS_BYTE);
    
    // Update our playback time
    Tplay = BASS_ChannelBytes2Seconds(Stm, new_pos);
    
    // When seeking backwards, reload the note data
    if (seconds < 0) {
        // Clear the note list
        for (int i = 0; i < 128; ++i) {
            MIDI.L[i].clear();
        }
        
        // Seek the list to the new position and update
        MIDI.list_seek(Tplay);
        MIDI.update_to(Tplay + Tscr);
    }
}

// Function to toggle pause/play
void toggle_pause() {
    if (is_paused) {
        // Resume playback
        BASS_ChannelPlay(Stm, false); // false means don't restart from beginning
        is_paused = false;
    } 
    else {
        // Pause playback
        saved_position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
        BASS_ChannelPause(Stm);
        is_paused = true;
    }
}

void NVi::Quit()
{
    // Caused runtime errors bc the destructor was called twice
    //ImGui_ImplSDLRenderer3_Shutdown();
    //ImGui_ImplSDL3_Shutdown();
    //ImGui::DestroyContext();
    //delete CvWin; // This results in undefined behavior for both SDL and Imgui
    // The destructor is called automatically when the object goes out of scope
    BASS_Free();
    BASS_PluginFree(0);
    MIDI.destroy_all();
    exit(0);
}





void NVi::CreateMidiList()
{
    // Insert all midi and soundfont file paths
    
    std::vector<std::string> all_midi_files;
    
    //All possibilities
    auto mid_files       = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".mid");
    auto midi_files      = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".midi");
    auto smf_files       = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".smf");
    auto mid_caps_files  = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".MID");
    auto midi_caps_files = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".MIDI");
    auto smf_caps_files  = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".SMF");
    
    all_midi_files.insert(all_midi_files.end(), mid_files.begin(), mid_files.end());
    all_midi_files.insert(all_midi_files.end(), midi_files.begin(), midi_files.end());
    all_midi_files.insert(all_midi_files.end(), smf_files.begin(), smf_files.end());
    all_midi_files.insert(all_midi_files.end(), mid_caps_files.begin(), mid_caps_files.end());
    all_midi_files.insert(all_midi_files.end(), midi_caps_files.begin(), midi_caps_files.end());
    all_midi_files.insert(all_midi_files.end(), smf_caps_files.begin(), smf_caps_files.end());
    
    live_midi_list.resize(all_midi_files.size());
    
    for(int i = 0; i < live_midi_list.size(); i++)
    {
        live_midi_list[i] = all_midi_files[i];
    }
     
    auto root = cpptoml::make_table();
    
    auto midi_files_out = cpptoml::make_array();
    
    for (const auto& path : live_midi_list) 
    {
        midi_files_out->push_back(path);
    }
    
    root->insert("midi_files", midi_files_out);
    
    // Save list
    std::ofstream out(MIDI_LIST);
    if (out.is_open()) 
    {
        out << (*root);
        std::cout << "TOML written to output.toml\n";
    } 
    else 
    {
        Canvas C;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "Failed to save midi list", nullptr);
    }
}

void NVi::ReadMidiList()
{
    try 
    {
        auto config = cpptoml::parse_file(MIDI_LIST);
        if (auto array = config->get_array_of<std::string>("midi_files"))
        {
            live_midi_list = *array;
        } 
        else 
        {
            Canvas C;
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "'midi_files' array not found!!", nullptr);
            NVi::error("Player", "'midi_files' array not found or invalid");
            exit(1);
        }
    } // Try to catch parsing errros
    catch (const cpptoml::parse_exception& e) 
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
    live_soundfont_files = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".sf2");
    auto sfz_files = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY, ".sfz");
    live_soundfont_files.insert(live_soundfont_files.end(), sfz_files.begin(), sfz_files.end());
    live_soundfont_list.resize(live_soundfont_files.size());
    
    for(size_t i = 0; i < live_soundfont_files.size(); i++)
    {
        live_soundfont_list[i] = {live_soundfont_files[i], false};
    }
    NVConf::CreateSoundfontList(live_soundfont_list);
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
        parsed_config = NVConf::ReadConfig();
        live_conf = parsed_config;
        liveColor.r = parsed_config.bg_R;
        liveColor.g = parsed_config.bg_G;
        liveColor.b = parsed_config.bg_B;
        liveColor.a = parsed_config.bg_A;
        live_note_speed = parsed_config.note_speed;
    }
    else 
    {
        // If not assign default configuration
        default_config.bass_voice_count = 500;
#ifndef NON_ANDROID
        default_config.current_soundfonts = {default_sf_path};
#endif
        default_config.bg_R = 43;
        default_config.bg_G = 43;
        default_config.bg_B = 43;
        default_config.bg_A = 255;
        liveColor.r = default_config.bg_R;
        liveColor.g = default_config.bg_G;
        liveColor.b = default_config.bg_B;
        liveColor.a = default_config.bg_A;
        live_conf = default_config;
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
    else
    {
        if(!MIDI.start_parse(parsed_config.last_midi_path.c_str()))
        {
            //Canvas C;
            std::ostringstream temp_msg;
            temp_msg << "Failed to load '" << parsed_config.last_midi_path << "'";
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", temp_msg.str().c_str(), nullptr);
            return 1;
        }
        else 
        {
            NVi::info("Player", "Midi file exists\n"); // Just to not let this empty lol
        }
    }
    

    CvWin = new Canvas;
    
    // Set the last background color
    SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);

    // Bass, bassmidi stuff
    BASS_PluginLoad(BASSMIDI_LIB, 0);
    BASS_SetConfig(BASS_CONFIG_BUFFER, 500); // Set buffer size to 500ms
    BASS_SetConfig(BASS_CONFIG_MIDI_AUTOFONT, 0);
    BASS_Init(-1, 44100, 0, 0, nullptr);
    
    // Load the selected midi file
    Stm = BASS_StreamCreateFile(0, parsed_config.last_midi_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelSetDSP(Stm, &dsp_limiter, 0, 0);
    HSOUNDFONT Sf;
    
    
    if(live_soundfont_list.size() == 0)
    {
        std::cout << "Default sf\n";
#ifndef NON_ANDROID
        Sf = BASS_MIDI_FontInit(default_sf_path.c_str(), 0);
#else
        Sf = BASS_MIDI_FontInit(DEFAULT_SOUNDFONT, 0);
        BASS_MIDI_FontSetVolume(Sf, 0.15);
        font_list.push_back({Sf, -1, 0});
#endif
    }
    else 
    {
        for(int i = 0; i < checked_soundfonts.size(); i++)
        {
            Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
            if (Sf) 
            {
                BASS_MIDI_FontSetVolume(Sf, 0.15);
                font_list.push_back({Sf, -1, 0});
            }
        }
    }
   
    BASS_MIDI_StreamSetFonts(Stm, font_list.data(), font_list.size());
    
    BASS_MIDI_StreamLoadSamples(Stm);

    // Set the ammount of voices from the config
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);


    
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    BASS_ChannelPlay(Stm, 1);
    
    const unsigned int FPS=1000000/CvWin->mod->refresh_rate;// 20 may be replaced with a limited frame rate

    
    Uint64 lastTapTime = 0;
    const Uint32 doubleTapThreshold = 400; // in milliseconds
    
    // Allow live change of the backgoround color
   	SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
    
    /*
    App Main loop
    */
    while (true) // Keep running regardless of playback state
	{
		// Check if playback just ended (and we need to handle that)
		if (!playback_ended && BASS_ChannelIsActive(Stm) == BASS_ACTIVE_STOPPED) {
			playback_ended = true;
			is_paused = true; // Just mark as paused when it ends
			// Save the position at the end
			saved_position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
		}
		
		_WinH = CvWin->WinH - CvWin->WinW * 80 / 1000; Tscr = (double)_WinH / live_note_speed;
		
		// Always update MIDI notes regardless of playback state
		MIDI.update_to(Tplay + Tscr);
		MIDI.remove_to(Tplay);
		
		CvWin->canvas_clear();
		
		while (SDL_PollEvent(&Evt)) 
		{
			ImGui_ImplSDL3_ProcessEvent(&Evt);
			if (Evt.type == SDL_EVENT_QUIT)
				NVi::Quit();
				
			// Add keyboard controls for playback
			if (Evt.type == SDL_EVENT_KEY_DOWN) 
			{
				switch (Evt.key.key) 
				{
					case SDLK_SPACE:
						if (playback_ended) 
						{
							// If at the end and we press space, restart from beginning
							BASS_ChannelSetPosition(Stm, 0, BASS_POS_BYTE);
							BASS_ChannelPlay(Stm, FALSE);
							Tplay = 0.0;
							playback_ended = false;
							is_paused = false;
							
							// Reset visualization state
							for (int i = 0; i < 128; ++i) 
							{
								MIDI.L[i].clear();
							}
							MIDI.list_seek(0);
						} 
						else 
						{
							toggle_pause();
						}
						break;
					case SDLK_LEFT:
						if (playback_ended) 
						{
							// If at the end and trying to seek back, restart playback from desired position
							double new_time = Tplay + (-SEEK_AMOUNT);
							if (new_time < 0) new_time = 0;
							
							QWORD new_pos = BASS_ChannelSeconds2Bytes(Stm, new_time);
							BASS_ChannelSetPosition(Stm, new_pos, BASS_POS_BYTE);
							BASS_ChannelPlay(Stm, FALSE);
							
							// Reset visualization properly
							for (int i = 0; i < 128; ++i) 
							{
								MIDI.L[i].clear();
							}
							
							// Update time and reset flags
							Tplay = new_time;
							MIDI.list_seek(Tplay);
							playback_ended = false;
							is_paused = false;
						} 
						else 
						{
							seek_playback(-SEEK_AMOUNT);
						}
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
		
		SDL_SetRenderDrawColor(CvWin->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
		
		// Always draw notes
		for(int i = 0; i != 128; ++i)
		{
			for (const NVnote &n : MIDI.L[KeyMap[i]])
			{
				CvWin->DrawNote(i, n, live_note_speed);
			}
		}
		
		CvWin->DrawKeyBoard();
		NVGui::Run(CvWin->Ren);
		SDL_RenderPresent(CvWin->Ren);
		
		// Only update Tplay if actively playing and not at the end
		if (!is_paused && !playback_ended) {
			Tplay = BASS_ChannelBytes2Seconds(Stm, BASS_ChannelGetPosition(Stm, BASS_POS_BYTE));
		}
		
		const char * sdl_err = SDL_GetError();
		if(strlen(sdl_err) << 1)
		{
			SDL_Log("SDL Error: %s", sdl_err);
		}
	}
    
    //NVi::Quit();
#else
    TestStuff();
#endif
    return 0;
}