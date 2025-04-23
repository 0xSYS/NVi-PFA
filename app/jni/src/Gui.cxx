

#include <iostream>
#include <string>

//#include <android/asset_manager.h>
//#include <android/asset_manager_jni.h>
//#include <SDL_android.h>   // Add this for SDL_AndroidGetAssetManager

#include "Gui.hxx"
#include "Config_Utils.hxx"
#include "Utils.hxx"


#include "extern/imgui/imgui.h"
#include "extern/imgui_sdl3/imgui_impl_sdl3.h"
#include "extern/imgui_sdl3/imgui_impl_sdlrenderer3.h"







//bool show_demo_window = true;
bool main_gui_window = false;

int selIndex = 0;
static char midi_search[128];
static char sf_search[128];
static std::string midi_search_text;
static std::string sf_search_text;

//ImGuiIO& io = ImGui::GetIO();



void NVGui::SetDefaultTheme()
{
    /*
    Theme: Moonlight
    Author: deathsu/madam-herta
    Original source: https://github.com/Madam-Herta/Moonlight
    */
    ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha                    = 1.0f;
	style.DisabledAlpha            = 1.0f;
	style.WindowPadding            = ImVec2(12.0f, 12.0f);
	style.WindowRounding           = 11.5f;
	style.WindowBorderSize         = 0.0f;
	style.WindowMinSize            = ImVec2(20.0f, 20.0f);
	style.WindowTitleAlign         = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding            = 0.0f;
	style.ChildBorderSize          = 1.0f;
	style.PopupRounding            = 0.0f;
	style.PopupBorderSize          = 1.0f;
	style.FramePadding             = ImVec2(20.0f, 3.400000095367432f);
	style.FrameRounding            = 11.89999961853027f;
	style.FrameBorderSize          = 0.0f;
	style.ItemSpacing              = ImVec2(4.300000190734863f, 5.5f);
	style.ItemInnerSpacing         = ImVec2(7.099999904632568f, 1.799999952316284f);
	style.CellPadding              = ImVec2(12.10000038146973f, 9.199999809265137f);
	style.IndentSpacing            = 0.0f;
	style.ColumnsMinSpacing        = 4.900000095367432f;
	style.ScrollbarSize            = 20.60000038146973f;  // Modified
	style.ScrollbarRounding        = 15.89999961853027f;
	style.GrabMinSize              = 12.700000047683716f; // Modified
	style.GrabRounding             = 8.0f;                // Modified
	style.TabRounding              = 8.89999961853027f;
	style.TabBorderSize            = 0.0f;
	// style.TabMinWidthForCloseButton = 0.0f; // This seems to be deprecated in Imgui v1.91.9b
	style.ColorButtonPosition      = ImGuiDir_Right;
	style.ButtonTextAlign          = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign      = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.09250493347644806f, 0.100297249853611f, 0.1158798336982727f, 1.0f);
	style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_Border]                = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.1120669096708298f, 0.1262156516313553f, 0.1545064449310303f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.9725490212440491f, 1.0f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.971993625164032f, 1.0f, 0.4980392456054688f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.0f, 0.7953379154205322f, 0.4980392456054688f, 1.0f);
	style.Colors[ImGuiCol_Button]                = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.1821731775999069f, 0.1897992044687271f, 0.1974248886108398f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.1545050293207169f, 0.1545048952102661f, 0.1545064449310303f, 1.0f);
	style.Colors[ImGuiCol_Header]                = ImVec4(0.1414651423692703f, 0.1629818230867386f, 0.2060086131095886f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.1072951927781105f, 0.107295036315918f, 0.1072961091995239f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_Separator]             = ImVec4(0.1293079704046249f, 0.1479243338108063f, 0.1931330561637878f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.1459212601184845f, 0.1459220051765442f, 0.1459227204322815f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.9725490212440491f, 1.0f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.999999463558197f, 1.0f, 0.9999899864196777f, 1.0f);
	style.Colors[ImGuiCol_Tab]                   = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabActive]             = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.1249424293637276f, 0.2735691666603088f, 0.5708154439926147f, 1.0f);
	style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.8841201663017273f, 0.7941429018974304f, 0.5615870356559753f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.9570815563201904f, 0.9570719599723816f, 0.9570761322975159f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.9356134533882141f, 0.9356129765510559f, 0.9356223344802856f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.266094446182251f, 0.2890366911888123f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
	style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}

void NVGui::Setup(SDL_Window *w, SDL_Renderer *r)
{
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
#ifndef NON_ANDROID
    io.IniFilename = "/data/data/com.qsp.nvpfa/files/imgui.ini"; // Custom path for imgui ini
#endif
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    SetDefaultTheme(); // Setting a nice looking GUI :3
    
    
        //ImFont* font = io.Fonts->AddFontFromMemoryTTF(fontBuffer, fontSize, 19.0f);
      
    
    //Font suggested by Nerdly
    //Metrophobic-Regular.ttf
    //io.Fonts->AddFontFromFileTTF("/sdcard/Download/Roboto-Regular.ttf", 19.0f);
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(w, r);
    ImGui_ImplSDLRenderer3_Init(r);
}

/*
Functions for internal use only
*/

// So many of you wanted this
// And I can relate to such a problem

// Filter only the filenames before showing them on the midi and soundfont lists or it will cause a godamn stupid chaos
std::string FilenameOnly(const std::string& path) 
{
    size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos) ? path : path.substr(slash + 1);
}

void RenderMidiList(const std::vector<std::string>& items, int& selectedIndex, std::string find_item)
{
    ImGui::BeginChild("ListRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
    
        for (int i = 0; i < items.size(); ++i) 
        {
            std::string filename = FilenameOnly(items[i]);
    
            // Filter check (case-insensitive optional)
            if (!find_item.empty() && filename.find(find_item) == std::string::npos)
                continue;
    
            bool isSelected = (i == selectedIndex);
    
            if (ImGui::Selectable((filename + "##" + std::to_string(i)).c_str(), isSelected)) 
            {
                selectedIndex = i;
            }
    
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
    
        ImGui::EndChild();
}

void RenderSoundfontList(std::vector<SoundfontItem>& items, std::string find_item)
{
    ImGui::BeginChild("ListRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (int i = 0; i < items.size(); ++i) 
    {
        std::string filename = FilenameOnly(items[i].label);
    
        // Filter check
        if (!find_item.empty() && filename.find(find_item) == std::string::npos)
            continue;
    
        // Unique ID to avoid conflicts
        ImGui::Checkbox((filename + "##" + std::to_string(i)).c_str(), &items[i].checked);
    }
    
    ImGui::EndChild();
}

std::vector<std::string> NVGui::GetCheckedSoundfonts(const std::vector<SoundfontItem>& items)
{
    std::vector<std::string> checkedItems;
    
    for (const auto& item : items) 
    {
        if (item.checked) 
        {
            checkedItems.push_back(item.label);
        }
    }
    
    return checkedItems;
}

void NVGui::Run(SDL_Renderer *r)
{
    NVGui nvg;
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetIO().MouseClickedCount[ImGuiMouseButton_Left] == 2) 
    {
        main_gui_window = true;
    }
    
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);
    
            // Reserved for future use
            //ImGui::Text("%.1f FPS", nvg.io.Framerate);
    
    
            // Show the main GUI window
            if (main_gui_window)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
                ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f)); // Pivot 0.5 = center
                ImGui::SetNextWindowSizeConstraints(ImVec2(700, 380), ImVec2(FLT_MAX, FLT_MAX));
                ImGui::Begin("NVi PFA", &main_gui_window);
                
                //ImGui::SameLine();
                
                if (ImGui::Button("Quit"))
                    NVi::Quit();
                    
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Quit NVi PFA");
                    ImGui::EndTooltip();
                }
                //ImGui::SameLine();
                
                if(ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Play MIDI Files"))
                    {
                        ImGui::SetNextItemWidth(300);
                        ImGui::InputText("##A", midi_search, 128);
                        
                        midi_search_text = midi_search;
                        
                        // Draw placeholder text if empty and not focused
                        if (strlen(midi_search) == 0 && !ImGui::IsItemActive()) {
                            ImVec2 pos = ImGui::GetItemRectMin();
                            ImVec2 text_pos = ImVec2(pos.x + ImGui::GetStyle().FramePadding.x, pos.y + ImGui::GetStyle().FramePadding.y);
                            ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search midis");
                        }
                        
                        ImGui::SameLine();
                        
                        if(ImGui::Button("Refresh List"))
                        {
                            NVi::CreateMidiList(); // It simply overwrites to the present midi list
                        }
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Synchronize the midi file list with the new files created");
                            ImGui::EndTooltip();
                        }
                        
                        ImGui::SameLine();
                        
                        if (ImGui::Button("Save to config"))
                        {
                            live_conf.last_midi_path = live_midi_list[selIndex];
                            NVConf::WriteConfig(live_conf);
                            //std::cout << "Loading mede feile\n";
                        }
                            
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Saves and play the selected midi on the next run");
                            ImGui::EndTooltip();
                        }
                        
                        ImGui::SameLine();
                        
                        if (ImGui::Button("Close"))
                            //NVi::CloseMidiPlayback();
                            std::cout << "Close midi playback\n";
                        
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Close and reset midi playback");
                            ImGui::EndTooltip();
                        }
                        
                        RenderMidiList(live_midi_list, selIndex, midi_search_text);
                        //std::cout << "Selection index: " << live_midi_list[selIndex] << "\n";
                        ImGui::EndTabItem();
                    }
                    
                    if (ImGui::BeginTabItem("Soundfonts"))
                    {
                        ImGui::SetNextItemWidth(300);
                        ImGui::InputText("##E", sf_search, 128);
                        
                        sf_search_text = sf_search;
                        
                        // Draw placeholder text if empty and not focused
                        if (strlen(midi_search) == 0 && !ImGui::IsItemActive()) 
                        {
                            ImVec2 pos = ImGui::GetItemRectMin();
                            ImVec2 text_pos = ImVec2(pos.x + ImGui::GetStyle().FramePadding.x, pos.y + ImGui::GetStyle().FramePadding.y);
                            ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search soundfonts");
                        }
                        
                        ImGui::SameLine();
                        
                        if(ImGui::Button("Refresh List"))
                        {
                            NVi::RefreshSFList();
                        }
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Synchronize the soundfont file list with the new files created");
                            ImGui::EndTooltip();
                        }
                        
                        ImGui::SameLine();
                        
                        if(ImGui::Button("Save Soundfont List"))
                        {
                            NVConf::CreateSoundfontList(live_soundfont_list);
                        }
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Save the modified soundfont list");
                            ImGui::EndTooltip();
                        }
                        RenderSoundfontList(live_soundfont_list, sf_search_text);
                        //checked_soundfonts = NVGui::GetCheckedSoundfonts(live_soundfont_list);
                        //for(int i = 0; i < checked_soundfonts.size(); i++)
                            //std::cout << "from gui: " << checked_soundfonts.size() << "\n";
                            
                        ImGui::EndTabItem();
                    }
                    
                    if (ImGui::BeginTabItem("Settings"))
                    {
                        ImGui::Text("Settings marked with * require restart of NV PFA\n\n");
                        ImGui::Text("Background Color");
                        clear_color = ImVec4(live_conf.bg_R / 255.0f, live_conf.bg_G / 255.0f, live_conf.bg_B / 255.0f, live_conf.bg_A / 255.0f);
                        ImGui::ColorEdit3("clear color", (float*)&clear_color);
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Change the background color of the main scene");
                            ImGui::EndTooltip();
                        }
                        liveColor = NVi::Frgba2Irgba(clear_color);
                        ImGui::Text("\n");
                        ImGui::Text("Voice Count *");
                        static int i0 = 123;
                        ImGui::InputInt("input int", &live_conf.bass_voice_count);
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Set how many notes can be played on specific instruments");
                            ImGui::EndTooltip();
                        }
                        
                        // Keeping the background color updated
                        live_conf.bg_R = liveColor.r;
                        live_conf.bg_G = liveColor.g;
                        live_conf.bg_B = liveColor.b;
                        live_conf.bg_A = liveColor.a;
                        //live_conf.current_soundfonts = {"/home/0xsys/Desktop/piano maganda (26,893kb).sf2"};
                        
                        if(ImGui::Button("Save"))
                        {
                            NVConf::WriteConfig(live_conf);
                        }
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Save settings to configuration file");
                            ImGui::EndTooltip();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("About"))
                    {
                        ImGui::Text("A clone of the original Piano From Above for mobile based on Qishipai's midi processing library.");
                        ImGui::Text("Authors:");
                        ImGui::Text("NVirsual: Qishipai");
                        ImGui::Text("Piano From Above imitation: Tweak");
                        ImGui::Text("Improved by 0xsys\n\n");
                        ImGui::Text("Powered by: SDL3, Imgui, bass and bass plugins");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::End();
            }
    
            // Rendering
            ImGui::Render();
            //SDL_RenderClear(r); // Don't clear the render bc no midi scene will show up
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), r);
}