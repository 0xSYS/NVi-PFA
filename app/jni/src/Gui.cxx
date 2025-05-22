#include <string>
//#include <iostream>
#include <SDL3_image/SDL_image.h>


#include "Gui.hxx"
#include "Config_Utils.hxx"
#include "Utils.hxx"
#include "file_utils.hxx"
#include "canvas.hxx"


#include "extern/imgui/imgui.h"
#include "extern/imgui_sdl3/imgui_impl_sdl3.h"
#include "extern/imgui_sdl3/imgui_impl_sdlrenderer3.h"


//#undef NON_ANDROID


bool show_demo_window = true;
bool file_info_window = false;
//bool main_gui_window = false;

//int selIndex = 0;
bool call_once = false;
bool allow_audio_dev_ssave;
static char midi_search[128];
static char sf_search[128];
static std::string midi_search_text;
static std::string sf_search_text;
static std::string img_filename;
static std::string sf_filename;
static std::string midi_filename;
static char midi_path_entry[1024];
static char soundfons_path_entry[1024];
static char file_info_text[1500 * 50];
static int selected_image = 0;
int selected_midi_path_entry;
int selected_soundfont_path_etry;
int selected_img_path_entry;
std::string temp_widget_id;
std::ostringstream file_info_fields;
std::vector <std::string> current_soundfonts;
NVFileUtils::FileInfo current_file_info;

float image_size = 80.0f;
float padding = 8.0f;




// Add these to your other extern declarations
extern bool is_paused;
extern void seek_playback(double seconds);
extern void toggle_pause();

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
	
	// Different size for the scrollbar so user can actually grab it lol
#ifdef NON_ANDROID
	style.ScrollbarSize            = 20.60000038146973f;
#else
	style.ScrollbarSize            = 48.60000038146973f;
#endif

	style.ScrollbarRounding        = 15.89999961853027f;
#ifdef NON_ANDROID
	style.GrabMinSize              = 12.700000047683716f;
#else
	style.GrabMinSize              = 40.700000047683716f;
#endif

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
    
    //Metrophobic-Regular.ttf
    //Font suggested by Nerdly
#ifndef NON_ANDROID
    std::string ui_font_file = NVFileUtils::GetFilePathA("ui_font.ttf", "rb");
    io.Fonts->AddFontFromFileTTF(ui_font_file.c_str(), 38.0f);
#else
  io.Fonts->AddFontFromFileTTF("ui_font.ttf", 19.0f);
  //io.Fonts->AddFontFromFileTTF("ui_font.ttf", 19.0f);
#endif
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(w, r);
    ImGui_ImplSDLRenderer3_Init(r);
}


/*
Functions for internal use only
*/


int LoadMidiThreadFunc(void *data)
{
    LoadMidiArgs *args = (LoadMidiArgs*)data;
    // Optionally set high priority
    SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    loadMidiFile(args->midi_path);
    delete args;
    return 0;
}

void startLoadMidiThread(const std::string& midi_path)
{
    LoadMidiArgs *args = new LoadMidiArgs{midi_path};
    SDL_CreateThread(LoadMidiThreadFunc, "LoadMidiThread", args);
}



/*
- - - - [UI Functions] - - - -
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
    ImGui::BeginChild("##midils", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for(int i = 0; i < items.size(); ++i)
    {
        std::string midi_filename = FilenameOnly(items[i]);
    
        // Filter check (case-insensitive optional)
        if(!find_item.empty() && midi_filename.find(find_item) == std::string::npos)
            continue;
    
        bool isSelected = (i == selectedIndex);
    
        if(ImGui::Selectable((midi_filename + "##" + std::to_string(i)).c_str(), isSelected))
        {
            selectedIndex = i;
        }
    
        if(isSelected)
            ImGui::SetItemDefaultFocus();
    }
    
        ImGui::EndChild();
}

void RenderSoundfontList(std::vector<SoundfontItem>& items, std::string find_item)
{
    ImGui::BeginChild("##sfls", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    bool soundfont_changed = false;
    
    for(int i = 0; i < items.size(); ++i)
    {
        std::string sf_filename = FilenameOnly(items[i].label);
    
        // Filter check
        if(!find_item.empty() && sf_filename.find(find_item) == std::string::npos)
            continue;
            
        // Store previous checkbox state
        bool previous_state = items[i].checked;
    
        // Unique ID to avoid conflicts
        ImGui::Checkbox((sf_filename + "##" + std::to_string(i)).c_str(), &items[i].checked);
        
        // Check if state changed
        if(previous_state != items[i].checked)
        {
            soundfont_changed = true;
        }
    }
    
    ImGui::EndChild();
    
    // If any soundfont selection changed, update the checked_soundfonts list
    // and reload soundfonts immediately
    if(soundfont_changed)
    {
        checked_soundfonts = NVGui::GetCheckedSoundfonts(items);
        reloadSoundfonts();
    }
}

std::vector<std::string> NVGui::GetCheckedSoundfonts(const std::vector<SoundfontItem>& items)
{
    std::vector<std::string> checkedItems;
    
    for(const auto& item : items)
    {
        if(item.checked)
        {
            checkedItems.push_back(item.label);
        }
    }
    
    return checkedItems;
}

void RenderMidiPathsList(const std::vector<std::string>& items, int& selectedIndex)
{
    ImGui::BeginChild("##midipathls", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for(int i = 0; i < items.size(); ++i)
    {
        bool isSelected = (i == selectedIndex);
    
        if(ImGui::Selectable((items[i] + "##" + std::to_string(i)).c_str(), isSelected))
        {
            selectedIndex = i;
        }
    
        if (isSelected)
            ImGui::SetItemDefaultFocus();
    }
    
    ImGui::EndChild();
}

void RenderSoundfontsPathsList(const std::vector<std::string>& items, int& selectedIndex)
{
    ImGui::BeginChild("##soundfontspathls", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for(int i = 0; i < items.size(); ++i)
    {
        bool isSelected = (i == selectedIndex);
    
        if(ImGui::Selectable((items[i] + "##" + std::to_string(i)).c_str(), isSelected))
        {
            selectedIndex = i;
        }
    
        if(isSelected)
            ImGui::SetItemDefaultFocus();
    }
    
    ImGui::EndChild();
}

void RenderImgPathsList(const std::vector<std::string>& items, int& selectedIndex)
{
    ImGui::BeginChild("##imgpathls", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for(int i = 0; i < items.size(); ++i)
    {
        bool isSelected = (i == selectedIndex);
    
        if(ImGui::Selectable((items[i] + "##" + std::to_string(i)).c_str(), isSelected))
        {
            selectedIndex = i;
        }
    
        if(isSelected)
            ImGui::SetItemDefaultFocus();
    }
    
    ImGui::EndChild();
}

void ShowAudioDeviceList(const std::vector<NVi::AudioDevice>& audioDevices)
{
    std::vector<const char*> deviceNames;
    for(const auto& device : audioDevices)
    {
        deviceNames.push_back(device.name);
    }
    
    // Ensure current_audio_dev is valid
    if(deviceNames.empty())
    {
        current_audio_dev = 0; // Reset to 0 if there are no devices
        allow_audio_dev_ssave = false;
    }
    else if(current_audio_dev >= deviceNames.size())
    {
        allow_audio_dev_ssave = true;
        current_audio_dev = 0; // Reset to the first device if the index is out of bounds
    }
    
        // Display the combo box
    const char* currentDeviceName = (deviceNames.empty() ? "No devices available" : deviceNames[current_audio_dev]);
    
    if(ImGui::BeginCombo("Audio Devices *", currentDeviceName))
    {
        if(deviceNames.empty())
        {
            // Render a placeholder item when no devices are available
            ImGui::Selectable("No devices available", false);
    
            // Avoid spamming warnings
            if(!call_once)
            {
                call_once = true;
                NVi::warn("Gui", "No audio devices available\n");
            }
        }
        else
        {
            for(int i = 0; i < deviceNames.size(); i++)
            {
                // Check if this item is selected
                bool isSelected = (current_audio_dev == i);
    
                // Add the item to the combo box
                if(ImGui::Selectable(deviceNames[i], isSelected))
                {
                    // Update the current index if the user selects this item
                    current_audio_dev = i;
                }
    
                // Set the initial focus when opening the combo box
                if(isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
    }
}

void RenderImageList(std::vector<SDL_Texture*> img)
{
#ifdef NON_ANDROID
    ImGui::BeginChild("##imgs", ImVec2(300, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
#else
    ImGui::BeginChild("##imgs", ImVec2(500, 430), true, ImGuiWindowFlags_HorizontalScrollbar);
#endif
    for (size_t i = 0; i < img.size(); ++i)
    {
        int w = (int)image_size, h = (int)image_size;
        // If you have per-image size, use SDL_GetTextureSize(img[i], &w, &h);
        
        ImGui::PushID((int)i);
        
        // Set the height of the selectable to fit the image
        ImVec2 selectable_size(0, image_size);
        
        // Make the whole row selectable
        bool is_selected = (selected_image == (int)i);
        if (ImGui::Selectable("##selectable", is_selected, 0, selectable_size))
        {
            selected_image = (int)i;
        }
        
        // Allow drawing over the selectable
        ImGui::SetItemAllowOverlap();
        
        // Get the rectangle of the selectable
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        
        // Vertically center the image in the selectable
        float image_y = min.y + (selectable_size.y - h) * 0.5f;
        ImGui::GetWindowDrawList()->AddImage((ImTextureID)img[i], ImVec2(min.x, image_y), ImVec2(min.x + w, image_y + h));
        
        // Vertically center the text in the selectable
        float text_height = ImGui::GetFontSize();
        float text_y = min.y + (selectable_size.y - text_height) * 0.5f;
        ImGui::SetCursorScreenPos(ImVec2(min.x + w + padding, text_y));
        img_filename = FilenameOnly(all_image_files[i]);
        ImGui::Text("%s", img_filename.c_str());
        
        // Item spacing
        if (i + 1 < img.size())
            ImGui::Dummy(ImVec2(0, 37.0f));
        
        ImGui::PopID();
    }
    ImGui::EndChild();
    
    ImGui::SameLine();
    // Render preview image in the main window, using ImGui::Image for correct placement
    if (selected_image >= 0 && selected_image < img.size())
    {
        ImGui::BeginGroup();
        ImGui::Text("Preview:");
        ImGui::Image((ImTextureID)img[selected_image], ImVec2(390, 390));
        ImGui::EndGroup();
    }
}

unsigned int ImVec4ToUInt(const ImVec4& color) 
{
    unsigned int r = static_cast<unsigned int>(color.x * 255.0f);
    unsigned int g = static_cast<unsigned int>(color.y * 255.0f);
    unsigned int b = static_cast<unsigned int>(color.z * 255.0f);
    return (r << 16) | (g << 8) | b;
}

ImVec4 UIntToImVec4(unsigned int rgb)
{
    float alpha = 1.0f;
    float r = ((rgb >> 16) & 0xFF) / 255.0f;
    float g = ((rgb >> 8)  & 0xFF) / 255.0f;
    float b = (rgb         & 0xFF) / 255.0f;
    return ImVec4(r, g, b, alpha);
}

void NVGui::Run(SDL_Renderer *r)
{
    NVGui nvg;
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    ImGuiIO& io = ImGui::GetIO();
    
    bool seek_back_fn = false;
    
    
    ImVec2 displaySize = io.DisplaySize;
    const float longClickThreshold = 0.5f;
    
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //{
    //    ImGui::ShowDemoWindow(&show_demo_window);
    //}
    
    // Reserved for future use
    //ImGui::Text("%.1f FPS", nvg.io.Framerate);
    

    ImVec2 windowPos = ImVec2((displaySize.x - displaySize.x + 15) * 0.5f,(displaySize.y - displaySize.y + 15) * 0.5f);
    ImGui::SetNextWindowSize(ImVec2(displaySize.x - 15, displaySize.y - 15));    // No size constraints
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::Begin("InvisibleWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGui::Columns(3, "ResizableColumns", false); // 2 columns, resizable
    float sb_col_w = ImGui::GetColumnWidth();
    float sb_btn_h = ImGui::GetContentRegionAvail().y;
    if(ImGui::InvisibleButton("<<" , ImVec2(sb_col_w, sb_btn_h)))
        seek_playback(-SEEK_AMOUNT);
    
    if(ImGui::IsItemActive())
    {
        float holdDuration = ImGui::GetIO().MouseDownDuration[0]; // [0] is for the left mouse button
    
        // Handling long click / tap event
        if(holdDuration > longClickThreshold)
        {
            main_gui_window = true;
        }
    }
    
    ImGui::NextColumn();

    float ps_col_w = ImGui::GetColumnWidth();
    float ps_btn_h = ImGui::GetContentRegionAvail().y;
    if(ImGui::InvisibleButton(is_paused ? "|>" : "||" , ImVec2(ps_col_w, ps_btn_h)))
        toggle_pause();
        
    if(ImGui::IsItemActive())
    {
        float holdDuration = ImGui::GetIO().MouseDownDuration[0]; // [0] is for the left mouse button
    
        // Handling long click / tap event to open up the settings window
        if(holdDuration > longClickThreshold)
        {
            main_gui_window = true;
        }
    }    
    
    ImGui::NextColumn();
    
    float sf_col_w = ImGui::GetColumnWidth();
    float sf_btn_h = ImGui::GetContentRegionAvail().y;
    if (ImGui::InvisibleButton(">>" , ImVec2(sf_col_w, sf_btn_h)))
        seek_playback(SEEK_AMOUNT);
        
    if(ImGui::IsItemActive())
    {
        float holdDuration = ImGui::GetIO().MouseDownDuration[0]; // [0] is for the left mouse button
    
        // Handling long click / tap event to open up the settings window
        if(holdDuration > longClickThreshold)
        {
            main_gui_window = true;
        }
    }
    
    ImGui::Columns(1); // Reset to single column
    
    
    
    // Show the main GUI window
    if(main_gui_window)
    {
        //selIndex = live_conf.midi_index;
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f)); // Pivot 0.5 = center
#ifdef NON_ANDROID
        ImGui::SetNextWindowSizeConstraints(ImVec2(700, 380), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("NVi PFA", &main_gui_window);
#else   // Setting up a different ui layout for mobile users
        ImGui::SetNextWindowSize(ImVec2(964.0f, 600.0f));
        ImGui::Begin("NVi PFA", &main_gui_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
#endif

        velocity_filter = live_conf.vel_filter;
        min_velocity = live_conf.vel_min;
        max_velocity = live_conf.vel_max;

        
        if(ImGui::Button("Quit"))
            NVi::Quit();
            
        if(ImGui::BeginItemTooltip())
        {
            ImGui::Text("Quit NVi PFA");
            ImGui::EndTooltip();
        }

        if(ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_None))
        {
            if(ImGui::BeginTabItem("Play MIDI Files"))
            {
                ImGui::SetNextItemWidth(200);
                ImGui::InputTextWithHint("##EHE", "Search midis", midi_search, IM_ARRAYSIZE(midi_search));
                
                midi_search_text = midi_search;
                
                ImGui::SameLine();
                
                if(ImGui::Button("Refresh List"))
                {
                    NVi::CreateMidiList(); // It simply overwrites to the present midi list
                }
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Synchronize the midi file list with the new files created");
                    ImGui::EndTooltip();
                }
                
                ImGui::SameLine();
                
                if(!is_playback_started && !playback_ended)
                {
                    ImGui::BeginDisabled();
                
                    if(ImGui::Button("Load Selected"))
                    {
                        // Check if the list is not empty and selIndex is valid before loading
                        if (!live_midi_list.empty() && selIndex >= 0 && selIndex < live_midi_list.size())
                        {
                            NVi::CloseMIDI();
                            startLoadMidiThread(live_midi_list[selIndex]);
                            live_conf.last_midi_path = live_midi_list[selIndex];
                            live_conf.midi_index = selIndex;
                            NVConf::WriteConfig(live_conf); // Also save the last selected midi file bc why not
                        }
                    }
                    ImGui::EndDisabled();
                }
                else
                {
                    if(ImGui::Button("Load Selected"))
                    {
                        // Check if the list is not empty and selIndex is valid before loading
                        if(!live_midi_list.empty() && selIndex >= 0 && selIndex < live_midi_list.size())
                        {
                            NVi::CloseMIDI();
                            //loadMidiFile(live_midi_list[selIndex]);
                            startLoadMidiThread(live_midi_list[selIndex]);
                            live_conf.last_midi_path = live_midi_list[selIndex];
                            live_conf.midi_index = selIndex;
                            NVConf::WriteConfig(live_conf); // Also save the last selected midi file bc why not
                        }
                    }
                }
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Load and play the selected MIDI file");
                    ImGui::EndTooltip();
                }
                
                ImGui::SameLine();
                
                if(ImGui::Button("Close"))
                    NVi::CloseMIDI();
                
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Close and reset midi playback");
                    ImGui::EndTooltip();
                }
                
                ImGui::SameLine();
                
                if(ImGui::Button("File info"))
                {
                    current_file_info = NVFileUtils::GetFileInfo(live_midi_list[selIndex]);
                    file_info_window = true;
                }
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Show file information");
                    ImGui::EndTooltip();
                }
                
                RenderMidiList(live_midi_list, selIndex, midi_search_text);
                ImGui::EndTabItem();
            }
            
            if(ImGui::BeginTabItem("Soundfonts"))
            {
                ImGui::SetNextItemWidth(300);
                ImGui::InputTextWithHint("##XD", "Search soundfonts", sf_search, IM_ARRAYSIZE(sf_search));
                
                sf_search_text = sf_search;
                
                ImGui::SameLine();
                
                if(ImGui::Button("Refresh List"))
                {
                    NVi::RefreshSFList();
                }
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Synchronize the soundfont file list with the new files created");
                    ImGui::EndTooltip();
                }
                
                ImGui::SameLine();
                
                if(ImGui::Button("File info"))
                {
                    current_file_info = NVFileUtils::GetFileInfo(live_midi_list[selIndex]);
                    file_info_window = true;
                }
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Show file information");
                    ImGui::EndTooltip();
                }
                
                RenderSoundfontList(live_soundfont_list, sf_search_text);
                    
                ImGui::EndTabItem();
            }
            
            if(ImGui::BeginTabItem("Settings"))
            {
                if(ImGui::Button("Save"))
                {
                    NVConf::WriteConfig(live_conf);
                }
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Save settings to configuration file");
                    ImGui::EndTooltip();
                }
                
                ImGui::SameLine();
                
                ImGui::Text("Settings marked with * require restart of NV PFA\n\n");
                
                if(ImGui::BeginTabBar("sub-tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("General"))
                    {
                        if(ImGui::CollapsingHeader("Custom media paths"))
                        {
                            ImGui::Checkbox("Include default paths", &use_default_media_paths);
                            live_conf.use_default_paths = use_default_media_paths;
                            if (ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Use the preincluded media paths to scan for midi and soundfont files\nDefault path: /sdcard/Download/");
                                ImGui::EndTooltip();
                            }
                            ImGui::Text("Add directories to scan for midis");
                            ImGui::InputTextWithHint("##XD", "New entry", midi_path_entry, IM_ARRAYSIZE(midi_path_entry));
                            
                            ImGui::SameLine();
                            
                            if(ImGui::Button("+"))
                            {
                                if(strlen(midi_path_entry) > 0)
                                {
                                    live_conf.extra_midi_paths.push_back(midi_path_entry); // Add the input text to the list
                                    midi_path_entry[0] = '\0';
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Add new midi path entry");
                                ImGui::EndTooltip();
                            }
                            
                            ImGui::SameLine();
                        
                            if(ImGui::Button("-"))
                            {
                                if(selected_midi_path_entry >= 0 && selected_midi_path_entry < live_conf.extra_midi_paths.size())
                                {
                                    live_conf.extra_midi_paths.erase(live_conf.extra_midi_paths.begin() + selected_midi_path_entry);
                                    selected_midi_path_entry = -1;
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Remove midi path entry");
                                ImGui::EndTooltip();
                            }
                        
                            RenderSoundfontsPathsList(live_conf.extra_midi_paths, selected_midi_path_entry);
                            
                            ImGui::Text("Add directories to scan for soundfonts");
                            ImGui::InputTextWithHint("##idk", "New entry", soundfons_path_entry, IM_ARRAYSIZE(soundfons_path_entry));
                            ImGui::SameLine();
                            if(ImGui::Button("+##sf"))
                            {
                                if(strlen(soundfons_path_entry) > 0)
                                {
                                    live_conf.extra_sf_paths.push_back(soundfons_path_entry);
                                    soundfons_path_entry[0] = '\0';
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Add new soundfonts path entry");
                                ImGui::EndTooltip();
                            }
                            
                            ImGui::SameLine();
                            
                            if(ImGui::Button("-##sf1"))
                            {
                                if(selected_soundfont_path_etry >= 0 && selected_soundfont_path_etry < live_conf.extra_sf_paths.size())
                                {
                                    live_conf.extra_sf_paths.erase(live_conf.extra_sf_paths.begin() + selected_soundfont_path_etry);
                                    selected_soundfont_path_etry = -1;
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Remove soundfonts path entry");
                                ImGui::EndTooltip();
                            }
                        
                            RenderMidiPathsList(live_conf.extra_sf_paths, selected_soundfont_path_etry);
                            
                            
                            ImGui::Text("Add directories to scan for images");
                            ImGui::InputTextWithHint("##images", "New entry", soundfons_path_entry, IM_ARRAYSIZE(soundfons_path_entry));
                            ImGui::SameLine();
                            if(ImGui::Button("+##img"))
                            {
                                if(strlen(soundfons_path_entry) > 0)
                                {
                                    live_conf.extra_img_paths.push_back(soundfons_path_entry);
                                    soundfons_path_entry[0] = '\0';
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Add new image path entry");
                                ImGui::EndTooltip();
                            }
                            
                            ImGui::SameLine();
                            
                            if(ImGui::Button("-##img1"))
                            {
                                if(selected_img_path_entry >= 0 && selected_img_path_entry < live_conf.extra_sf_paths.size())
                                {
                                    live_conf.extra_img_paths.erase(live_conf.extra_img_paths.begin() + selected_img_path_entry);
                                    selected_img_path_entry = -1;
                                }
                            }
                            if(ImGui::BeginItemTooltip())
                            {
                                ImGui::Text("Remove image path entry");
                                ImGui::EndTooltip();
                            }
                            
                            RenderImgPathsList(live_conf.extra_img_paths, selected_img_path_entry);
                        }
                        ImGui::EndTabItem();
                    }
                    
                    if(ImGui::BeginTabItem("Visual"))
                    {
                        ImGui::Checkbox("Overlap Remover *", &overlap_remover);
                        if(ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Removes duplicated note data reducing the visualization lag");
                            ImGui::EndTooltip();
                        }
                        ImGui::SliderInt("Note Speed", &live_note_speed, 100, 20000);
                        ImGui::Text("Background Color");
                        live_conf.OR = overlap_remover;
                        clear_color = ImVec4(live_conf.bg_R / 255.0f, live_conf.bg_G / 255.0f, live_conf.bg_B / 255.0f, live_conf.bg_A / 255.0f);

                        ImGui::ColorEdit3("##H", (float*)&clear_color);

                        if(ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Change the background color of the main scene");
                            ImGui::EndTooltip();
                        }
                        liveColor = NVi::Frgba2Irgba(clear_color);
                        
                        ImGui::Text("");
                        ImGui::Text("Custom Channel Colors *");
                        
                        if(ImGui::Button("Reset"))
                        {
                            //NVi::info("Gui", "Reset action\n");
                            for(int i = 0; i < 16; i++)
                            {
                                ui_chcolors[i] = UIntToImVec4(Col[i]);
                                live_conf.channel_colors[i] = Col[i];
                            }
                        }
                        if(ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Reset the color order");
                            ImGui::EndTooltip();
                        }
                        
                        for(int i = 0; i < 16; i++)
                        {
                            if(!is_defaultconfig)
                            {
                                ui_chcolors[i] = UIntToImVec4(live_conf.channel_colors[i]);
                            }
                            
                            temp_widget_id = "##Ch" + std::to_string(i);

                            ImGui::ColorEdit3(temp_widget_id.c_str(), (float*)&ui_chcolors[i], ImGuiColorEditFlags_NoInputs);

                            ImGui::SameLine();
                            
                            live_conf.channel_colors[i] = ImVec4ToUInt(ui_chcolors[i]);
                        }
                        ImGui::Text("\n");
                        
                        ImGui::Checkbox("Loop colors *", &loop_colors);
                        live_conf.loop_colors = loop_colors;
                        
                        ImGui::Checkbox("Use Background image", &use_bg_image);
                        ImGui::SameLine();
                        if(!use_bg_image)
                        {
                            ImGui::BeginDisabled();
                            if(ImGui::Button("Reload Background Image"))
                            {
                                CvWin->bg_img = IMG_LoadTexture(CvWin->Ren, all_image_files[selected_image].c_str());
                            }
                            ImGui::EndDisabled();
                        }
                        else
                        {
                            if(ImGui::Button("Reload Background Image"))
                            {
                                is_image_loaded = true;
                                CvWin->bg_img = IMG_LoadTexture(CvWin->Ren, all_image_files[selected_image].c_str());
                            }
                        }
                        if(ImGui::BeginItemTooltip())
                        {
                            ImGui::Text("Load the new selected image");
                            ImGui::EndTooltip();
                        }
                        live_conf.use_bg_img = use_bg_image;
                        is_image_loaded = use_bg_image;
                        RenderImageList(image_textures);
                        live_conf.bg_img = all_image_files[selected_image];
                    
                        ImGui::EndTabItem();
                    }
                    
                    if(ImGui::BeginTabItem("Audio"))
                    {
                        // Save device ID only if allowed by the list
                        // Risky!!
                        //if(allow_audio_dev_ssave)
                        //{
                            live_conf.audio_device_index = current_audio_dev;
                        //}
                        ShowAudioDeviceList(availableAudioDevices);
                        ImGui::Text("\n");
                        ImGui::Text("Voice Count");
                        // Store the previous value to detect changes
		                static int prev_voice_count = live_conf.bass_voice_count;
		
		                // Input widget for voice count
		                if(ImGui::InputInt("##LOL", &live_conf.bass_voice_count))
						{
			                // Ensure value is within reasonable limits
			                if (live_conf.bass_voice_count < 1) live_conf.bass_voice_count = 1;
			                if (live_conf.bass_voice_count > 5000) live_conf.bass_voice_count = 5000;
			
			                // Apply the change in real-time if the value has changed
			                if(prev_voice_count != live_conf.bass_voice_count)
						    {
								updateBassVoiceCount(live_conf.bass_voice_count);
								prev_voice_count = live_conf.bass_voice_count;
			                }
                        }
                        
                        ImGui::Text("Effects");
                        if(ImGui::CollapsingHeader("Velocity Filter *"))
                        {
                            ImGui::Checkbox("Enabled", &velocity_filter);
                            live_conf.vel_filter = velocity_filter;
                            
                            ImGui::BeginDisabled(!live_conf.vel_filter); // Disable widgets if isDisabled is true
                            {
                                ImGui::SliderInt("Min Velocity", &min_velocity, 0, 127);
                                ImGui::SliderInt("Max velocity", &max_velocity, 0, 127);
                            }
                            ImGui::EndDisabled();
                            
                            live_conf.vel_min = min_velocity;
                            live_conf.vel_max = max_velocity;
                            
                        }
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                    ImGui::Text("\n");
		        }
		
		        if(ImGui::BeginItemTooltip())
		        {
					ImGui::Text("Set how many notes can be played on specific instruments (changes apply immediately)");
					ImGui::EndTooltip();
		        }
                
                // Keeping the background color updated
                live_conf.bg_R = liveColor.r;
                live_conf.bg_G = liveColor.g;
                live_conf.bg_B = liveColor.b;
                live_conf.bg_A = liveColor.a;
                live_conf.note_speed = live_note_speed;

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("About"))
            {
                ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
                ImGui::Text("A clone of the original Piano From Above for mobile based on Qishipai's midi processing library.");
                ImGui::Text("Authors:");
                ImGui::Text("NVirsual: Qishipai");
                ImGui::Text("Piano From Above imitation: Tweak1600");
                ImGui::Text("Improved by:");
                ImGui::Text("0xsys");
                ImGui::Text("Hexagon-Midis\n\n");
                ImGui::Text("Icon Made by Zeal");
                ImGui::Text("Powered by: SDL3, Imgui, bass and bass plugins");
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    } // Main window
    
    if(file_info_window)
    {
        ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f)); // Pivot 0.5 = center
#ifdef NON_ANDROID
        ImGui::SetNextWindowSizeConstraints(ImVec2(500, 380), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("File Information", &file_info_window);
#else   // Setting up a different ui layout for mobile users
        ImGui::SetNextWindowSize(ImVec2(900.0f, 600.0f));
        ImGui::Begin("File Information", &file_info_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
#endif

        file_info_fields.str("");
        file_info_fields.clear();
     
        file_info_fields << "- - - - Midi File - - - -\n\n";
        file_info_fields << "File Name:         " << current_file_info.file_name << "\n";
        file_info_fields << "Last Modified:   " << current_file_info.last_mod << "\n";
        file_info_fields << "Size:                    " << current_file_info.size << "\n";
        
        // AAHHHH Stupid soundfont file information
        file_info_fields << "\n\n- - - - Loaded SoundFonts - - - -\n\n";
        
        for(int i = 0; i < sf_file_info_text_arr.size(); i++)
        {
            file_info_fields << sf_file_info_text_arr[i];
            file_info_fields << "\n";
        }
        
        strncpy(file_info_text, file_info_fields.str().c_str(), sizeof(file_info_text) - 1);
        file_info_text[sizeof(file_info_text) - 1] = '\0';
        
        
        ImVec2 avail = ImGui::GetContentRegionAvail();
        
        ImGui::InputTextMultiline("##source", file_info_text, IM_ARRAYSIZE(file_info_text), avail, ImGuiInputTextFlags_ReadOnly);
    
        ImGui::End();  
    }
    
    ImGui::End();
    
    // Rendering
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), r);
}