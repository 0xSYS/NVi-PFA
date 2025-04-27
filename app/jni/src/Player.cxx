#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
//#include <android/log.h>
//#include <bassmidi.h>
//#include <bass_fx.h>

#include "extern/audio/bassmidi.h"
//#include "extern/audio/bass_fx.h"

#include <iostream>


//#define NON_ANDROID // Temporary defined in here
//#define DEV_TEST // Use this for development purposes only




#include "Utils.hxx"
#include "file_utils.hxx"
#include "Config_Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"
#include "Gui.hxx"
#include "canvas.hxx"

#include "extern/imgui_sdl3/imgui_impl_sdl3.h"
#include "extern/cpptoml/cpptoml.h"






NVmidiFile M;
NVsequencer S;
NVi::u32_t nowtick;
double Tread;
Canvas cv;
unsigned char events[6144];
int eventCount = 0;


//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


static constexpr unsigned int Col[]= {
  0xFFFF33CC,
  0xFF4EFF33,
  0xFF9933FF,
  0xFFE7FF33,
  0xFF3366FF,
  0xFFFF7E33,
  0xFF33FF66,
  0xFFFF3381,
  0xFF33E1E1,
  0xFFE433E1,
  0xFF99E133,
  0xFF4B33E1,
  0xFFFFCC33,
  0xFF33B4FF,
  0xFFFF3333,
  0xFF33FFB1
};


SDL_Event Evt;
HSTREAM   Stm;



Uint64 lastTapTime = 0;
float lastTapX = 0, lastTapY = 0;


float limiter_threshold = 0.8f; // Max sample level before limiting
float limiter_knee = 0.05f;     // Knee softness (smooth start of compression)
float limiter_release = 0.0005f; // How quickly it relaxes gain (adjust if needed)

float current_gain = 1.0f; // Persistent global gain


void bassevt(DWORD type, DWORD param, DWORD chan, DWORD tick, DWORD time)
{
    BASS_MIDI_EVENT evt = {type,param,chan,tick,time};
    BASS_MIDI_StreamEvents(Stm,BASS_MIDI_EVENTS_STRUCT|BASS_MIDI_EVENTS_NORSTATUS|BASS_MIDI_EVENTS_CANCEL,&evt,1);
}


void CALLBACK normalize_dsp_proc(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user)
{
    float *samples = (float*)buffer;
    DWORD count = length / sizeof(float);
    
    for (DWORD i = 0; i < count; ++i)
    {
        float input = samples[i];
        float abs_input = fabs(input);
    
        if (abs_input > limiter_threshold)
        {
            // Over the threshold
            float exceed = abs_input - limiter_threshold;
    
            // Soft knee compression
            float compressed = exceed / (exceed + limiter_knee);
    
            // Calculate desired gain to apply
            float desired_gain = limiter_threshold / (limiter_threshold + compressed);
    
            // Smooth the gain change (attack instantly, release slowly)
            if (desired_gain < current_gain)
                current_gain = desired_gain; // attack (instant clamp)
            else
                current_gain += (desired_gain - current_gain) * limiter_release; // release (slow)
    
        }
        else
        {
            // No peak, slowly release gain toward 1.0
            current_gain += (1.0f - current_gain) * limiter_release;
        }
    
        // Apply the current gain
        samples[i] *= current_gain;
    }
}


int    _WinH;
//static int ns = live_note_speed;
// 20000 maximum
// 100 minimum
double Tplay = 0.0, Tscr;

static void DrawNote(NVi::u16_t k, const NVnote &n, int pps)
{
    unsigned int c = Col[(n.track%16+n.chn)%16];
    int key = KeyMap[k], y_1;
    int y_0 = floor(_WinH - (n.Tstart - Tplay) * pps+0.5f);

    if (y_0 < 0)
    {
        y_0 = 0;
    }
    else
    {
        if (y_0 > _WinH)
        {
            y_0 = _WinH;
        }
    }

    if (n.Tend < Tplay + Tscr)
    {
        y_1 = floor(_WinH - (n.Tend - Tplay) * pps+0.5f);

        if (y_1 < 0)
        {
            y_1 = 0;
        }
        else
        {
            if (y_1 > _WinH)
            {
                y_1 = _WinH;
            }
        }
    }
    else
    {
        y_1 = 0;
    }

    if (n.Tstart <= Tplay && Tplay < n.Tend)
    {
        Win->KeyPress[key] = true;
        Win->KeyColor[key] = c ;
    }

    Win->Note(k, y_0, y_1, c);
}

BOOL CALLBACK filter(HSTREAM S, DWORD trk, BASS_MIDI_EVENT *E, BOOL sk, void *u)
{
    if (E->event == MIDI_EVENT_NOTE)
    {
        int vel = HIBYTE(E->param);
        return vel == 0 || vel > 9;
    }

    return TRUE;
}

// Add these with your other global variables
bool is_paused = false;
QWORD saved_position = 0; // For storing position when paused
const double SEEK_AMOUNT = 3.0; // 3 seconds for seeking

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
}

// Function to toggle pause/play
void toggle_pause() {
    if (is_paused) {
        // Resume playback
        BASS_ChannelPlay(Stm, false); // false means don't restart from beginning
        is_paused = false;
    } else {
        // Pause playback
        saved_position = BASS_ChannelGetPosition(Stm, BASS_POS_BYTE);
        BASS_ChannelPause(Stm);
        is_paused = true;
    }
}

void NVi::Quit()
{
    // Causes runtime errors
    //ImGui_ImplSDLRenderer3_Shutdown();
    //ImGui_ImplSDL3_Shutdown();
    //ImGui::DestroyContext();
    delete Win;
    BASS_Free();
    BASS_PluginFree(0);
    MIDI.destroy_all();
    exit(0);
}





void NVi::CreateMidiList()
{
    // Insert all midi and soundfont file paths
    //live_midi_list = NVFileUtils::GetFilesByExtension(BASE_DIRECTORY,".mid");
    
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
        //std::cout << "Midi List: " << live_midi_list[i] << "\n";
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
            Canvas C;
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

//Maybe this will be eventually used sometime
/*
void NVi::StartBassPlayback(std::string midi_path, std::vector<std::string> soundfonts)
{
    //std::cout << "playback\n";
    Stm = BASS_StreamCreateFile(0, midi_path.c_str(), 0, 0, 0);
    HSOUNDFONT Sf;
    //std::cout << "array size: " << live_soundfont_list.size() << "\n";
    for(int i = 0; i < checked_soundfonts.size(); i++)
    {
        //std::cout << "sdfdsfdsfsdfdsfdsertertyertr\n";
        Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
        //std::cout << checked_soundfonts[i] << "dfgfdgfdgf\n";
    }
    BASS_MIDI_FONT FontSet{Sf, -1, 0};
    BASS_MIDI_FontSetVolume(Sf, 0.15);
    BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);
    
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);
    
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    BASS_ChannelPlay(Stm, 1);
}
*/

#if defined(_WIN32) || defined(_WIN64)
    #define BASSMIDI_LIB "bassmidi.dll"
#else
    #define BASSMIDI_LIB "libbassmidi.so"
#endif

#ifdef NON_ANDROID
int main(int ac, char **av)
#else
int SDL_main(int ac, char **av)
#endif
{
#ifndef DEV_TEST
    const char * midi_path = av[1];
    const char * sf_path = av[2];
    
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
        //std::cout << "No default midi\n";
    }
    
    if(!MIDI.start_parse(parsed_config.last_midi_path.c_str()))
    {
        Canvas C;
        std::ostringstream temp_msg;
        temp_msg << "Failed to load '" << parsed_config.last_midi_path << "'";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", temp_msg.str().c_str(), nullptr);
        return 1;
    }
    else {
        //std::cout << "M\n";
        NVi::info("Player", "Midi file exists\n"); // Just to not let this empty lol
    }
    

    Win = new Canvas;
    
    SDL_SetRenderDrawColor(Win->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);

    //_WinH = Win->WinH - Win->WinW * 80 / 1000; Tscr = (double)_WinH / live_note_speed;

    BASS_PluginLoad(BASSMIDI_LIB, 0);
    BASS_SetConfig(BASS_CONFIG_MIDI_AUTOFONT, 0);
    BASS_Init(-1, 44100, 0, 0, nullptr);
    
    Stm = BASS_StreamCreateFile(0, parsed_config.last_midi_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelSetDSP(Stm, &normalize_dsp_proc, 0, 0);
    HSOUNDFONT Sf;
    
    //Load all enabled soundfonts
    //std::cout << "Size: " << live_soundfont_list.size() << "\n";
    if(live_soundfont_list.size() == 0)
    {
        std::cout << "Default sf\n";
#ifndef NON_ANDROID
        Sf = BASS_MIDI_FontInit(default_sf_path.c_str(), 0);
#else
        Sf = BASS_MIDI_FontInit(DEFAULT_SOUNDFONT, 0);
#endif
    }
    else 
    {
        for(int i = 0; i < checked_soundfonts.size(); i++)
        {
            Sf = BASS_MIDI_FontInit(checked_soundfonts[i].c_str(), 0);
        }
    }
    BASS_MIDI_FONT FontSet{Sf, -1, 0};
    BASS_MIDI_FontSetVolume(Sf, 0.15);
    BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);
    
    // No more errape
    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, parsed_config.bass_voice_count);


    
    BASS_MIDI_StreamSetFilter(Stm, 0, reinterpret_cast<BOOL (*)(HSTREAM, int, BASS_MIDI_EVENT *, BOOL, void *)>(filter), nullptr);
    BASS_ChannelPlay(Stm, 1);
    
    const unsigned int FPS=1000000/Win->mod->refresh_rate;// 20 may be replaced with a limited frame rate
    //unsigned long long _FPS_Timer; // No ideea what this is for

    
    Uint64 lastTapTime = 0;
    const Uint32 doubleTapThreshold = 400; // in milliseconds
    
    
    // updated tplay method
	while (BASS_ChannelIsActive(Stm) != BASS_ACTIVE_STOPPED)
	{
	    _WinH = Win->WinH - Win->WinW * 80 / 1000; Tscr = (double)_WinH / live_note_speed;
		MIDI.update_to(Tplay + Tscr);
		MIDI.remove_to(Tplay);
		Win->canvas_clear();
		while (SDL_PollEvent(&Evt)) 
		{
			ImGui_ImplSDL3_ProcessEvent(&Evt);
			if (Evt.type == SDL_EVENT_QUIT)
				break;
				
			// Add keyboard controls for playback
			if (Evt.type == SDL_EVENT_KEY_DOWN) 
			{
				switch (Evt.key.key) {
					case SDLK_SPACE:
						toggle_pause();
						break;
						//rewind used to be here
					case SDLK_RIGHT:
						seek_playback(SEEK_AMOUNT);
						break;
					case SDLK_Q:
						NVi::Quit();
						break;
				}
			}
		}
		
		//pps = live_note_speed;
		
		SDL_SetRenderDrawColor(Win->Ren, liveColor.r, liveColor.g, liveColor.b, liveColor.a);
		for(int i=0;i!=128;++i)
		{
			for (const NVnote &n : MIDI.L[KeyMap[i]])
			{
				DrawNote(i, n, live_note_speed);
			}
		}
		Win->DrawKeyBoard();
		NVGui::Run(Win->Ren);
		SDL_RenderPresent(Win->Ren);
		
		// Only update Tplay if not paused
		if (!is_paused) {
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

/*
- - - - - - - - - - - - - - - - - - - - - - Testing new backend components - - - - - - - - - - - - - - - - - - - - - -
*/
    //std::vector<std::string> midi_files;
    //midi_files = NVFileUtils::GetFilesByExtension("/home/0xsys/Desktop",".mid");
    
    //for(int i = 0; i < midi_files.size(); i++)
    //{
    //    std::cout << "From Vector: " << midi_files[i] << "\n";
    //}

    // NVConf::configuration test;
    // NVConf::configuration test2;
    
/*
    test.bass_voice_count = 700;
    test.bg_B = 48;
    test.bg_G = 32;
    test.bg_R = 56;
    test.current_soundfont = "/home/someone/black/midi/Soundfont.sf2";
    test.window_w = 580;
    test.window_h = 430;
    NVConf::WriteConfig(test);
*/
/*
    test2 = NVConf::ReadConfig();
    std::cout << test2.bass_voice_count 
              << "\n" << test2.bg_B 
              << "\n" << test2.bg_G 
              << "\n" << test2.bg_R 
              << "\n" << test2.current_soundfont 
              << "\n" << test2.window_h 
              << "\n" << test2.window_w 
              << "\n";
*/

// NVi::info("Some Prefix", "Log idk\n");
// NVi::error("Error", "Something failed lol\n");
// NVi::warn("Warn prefix", "There might be a problem\n");
/*
Todo:
Finish midi and soundfont file scanning + implementing both midi and soudfont list files
Add support for bass_fx to reduce the loudness
Adding about info
Trying to make an apk build of this + practical tests
Releasing the source code
*/

//NVi::CreateMediaLists();
//    
//NVConf::CreateSoundfontList(live_soundfont_list);

/*
std::vector<SoundfontItem> reading = NVConf::ReadSoundfontList();
for(int i = 0; i < reading.size(); i++)
{
    std::cout << "Reading: " << reading[i].label << " | Enabled: " << reading[i].checked << "\n";
}
*/
    
#endif
    return 0;
}