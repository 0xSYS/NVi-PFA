
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <iostream>
#include <cmath>
#include <algorithm>


//#include <SDL_image.h>


#include "extern/imgui/imgui.h"
#include "extern/imgui_sdl3/imgui_impl_sdl3.h"
#include "extern/imgui_sdl3/imgui_impl_sdlrenderer3.h"


#include "common.hxx"
#include "canvas.hxx"
#include "Gui.hxx"
//#include "Utils.hxx"





int _KeyWidth[128];
const float SharpRatio = 0.64f;
typedef unsigned long DWORD;
float fDeflate;
// Define the vertex structure
SDL_Vertex vert[4];
// unsigned short r=c&0xFF;
// unsigned short g=(c&0xFF00)>>8;
// unsigned short b=(c&0xFF0000)>>16;
// Implementing the DrawRect Function
void DrawRect(SDL_Renderer* renderer, float x, float y, float cx, float cy, DWORD c1, DWORD c2, DWORD c3, DWORD c4)
{
    // Converts color from DWORD to SDL_Color
    vert[0].position.x = x;
    vert[0].position.y = y;
    vert[0].color.r = (c1&0xFF)/255.0f;
    vert[0].color.g = ((c1&0xFF00)>>8)/255.0f;
    vert[0].color.b = ((c1&0xFF0000)>>16)/255.0f;
    vert[0].color.a = 1.0;

    // left
    vert[1].position.x = x+cx;
    vert[1].position.y = y;
    vert[1].color.r = (c2&0xFF)/255.0f;
    vert[1].color.g = ((c2&0xFF00)>>8)/255.0f;
    vert[1].color.b = ((c2&0xFF0000)>>16)/255.0f;
    vert[1].color.a = 1.0;

    // right
    vert[2].position.x = x+cx;
    vert[2].position.y = y+cy;
    vert[2].color.r = (c3&0xFF)/255.0f;
    vert[2].color.g = ((c3&0xFF00)>>8)/255.0f;
    vert[2].color.b = ((c3&0xFF0000)>>16)/255.0f;
    vert[2].color.a = 1.0;
    vert[3].position.x = x;
    vert[3].position.y = y+cy;
    vert[3].color.r = (c4&0xFF)/255.0f;
    vert[3].color.g = ((c4&0xFF00)>>8)/255.0f;
    vert[3].color.b = ((c4&0xFF0000)>>16)/255.0f;
    vert[3].color.a = 1.0;
    int indices[] = {0, 1, 2, 2, 3, 0};
    // Call SDL_RenderGeometry to draw the quadrilateral.
    SDL_RenderGeometry(renderer, NULL, vert, 4, indices, 6);
}

void DrawSkew(SDL_Renderer* renderer, float x1, float y1, float x2, float y2,float x3, float y3,float x4, float y4, DWORD c1, DWORD c2, DWORD c3, DWORD c4)
{
    // Converts color from DWORD to SDL_Color
    vert[0].position.x = x1;
    vert[0].position.y = y1;
    vert[0].color.r = (c1&0xFF)/255.0f;
    vert[0].color.g = ((c1&0xFF00)>>8)/255.0f;
    vert[0].color.b = ((c1&0xFF0000)>>16)/255.0f;
    vert[0].color.a = 1.0;

    // left
    vert[1].position.x = x2;
    vert[1].position.y = y2;
    vert[1].color.r = (c2&0xFF)/255.0f;
    vert[1].color.g = ((c2&0xFF00)>>8)/255.0f;
    vert[1].color.b = ((c2&0xFF0000)>>16)/255.0f;
    vert[1].color.a = 1.0;

    // right
    vert[2].position.x = x3;
    vert[2].position.y = y3;
    vert[2].color.r = (c3&0xFF)/255.0f;
    vert[2].color.g = ((c3&0xFF00)>>8)/255.0f;
    vert[2].color.b = ((c3&0xFF0000)>>16)/255.0f;
    vert[2].color.a = 1.0;
    vert[3].position.x = x4;
    vert[3].position.y = y4;
    vert[3].color.r = (c4&0xFF)/255.0f;
    vert[3].color.g = ((c4&0xFF00)>>8)/255.0f;
    vert[3].color.b = ((c4&0xFF0000)>>16)/255.0f;
    vert[3].color.a = 1.0;
    int indices[] = {0, 1, 2, 2, 3, 0};
    // Call SDL_RenderGeometry to draw the quadrilateral.
    SDL_RenderGeometry(renderer, NULL, vert, 4, indices, 6);
}



const unsigned char KeyMap[128] =
{
    0,   2,   4,   5,   7,   9,   11,  12,  14,  16,  17,  19,  21,  23,  24,  26,
    28,  29,  31,  33,  35,  36,  38,  40,  41,  43,  45,  47,  48,  50,  52,  53,
    55,  57,  59,  60,  62,  64,  65,  67,  69,  71,  72,  74,  76,  77,  79,  81,
    83,  84,  86,  88,  89,  91,  93,  95,  96,  98,  100, 101, 103, 105, 107, 108,
    110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 1,   3,   6,   8,   10,
    13,  15,  18,  20,  22,  25,  27,  30,  32,  34,  37,  39,  42,  44,  46,  49,
    51,  54,  56,  58,  61,  63,  66,  68,  70,  73,  75,  78,  80,  82,  85,  87,
    90,  92,  94,  97,  99,  102, 104, 106, 109, 111, 114, 116, 118, 121, 123, 126,
};

static const short GenKeyX[] =
{
    0, 12, 18, 33, 36, 54, 66, 72, 85, 90, 105, 108
};

static int call_index = 0;

Canvas::Canvas()
{
    call_index++; // Increment the index each time this method gets called
    
    // Avoiding to initialize SDL and imgui 2 times bc apparently this class method gets called 2 times
    // Very weird
    if(call_index == 2)
    {
        SDL_Init(SDL_INIT_VIDEO); //IMG_Init(IMG_INIT_PNG);
        //Win = SDL_CreateWindow("NVplayer++", parsed_config.window_w, parsed_config.window_h, 0);
        //Win = SDL_CreateWindow("NVplayer++", 1900, 900, 0);
#ifndef NON_ANDROID
        Win = SDL_CreateWindow("NVplayer++", 1920, 1080, 0);
#else
        Win = SDL_CreateWindow("NVplayer++", 1900, 900, 0);
#endif
        if(Win == nullptr)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "Failed to create window" , nullptr);
        }
        
        
        Ren = SDL_CreateRenderer(Win, "opengles2");
        if(Ren == nullptr)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", "Failed to create render context" , nullptr);
        }
        
	    SDL_SetRenderVSync(Ren, 1);
	    SDL_SetWindowPosition(Win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	
	    SDL_DisplayID sid = SDL_GetDisplayForWindow(Win);
	    mod = SDL_GetDesktopDisplayMode(sid);
	    SDL_SetRenderDrawBlendMode(Ren, SDL_BLENDMODE_BLEND);
	    //R, G, B, A
        // SDL_SetRenderDrawColor(Ren, parsed_config.bg_R, parsed_config.bg_G, parsed_config.bg_B, 255); // Background color
        SDL_SetRenderDrawColorFloat(Ren, parsed_config.bg_R, parsed_config.bg_G, parsed_config.bg_B, parsed_config.bg_A);
        SDL_GetWindowSize(Win, &WinW, &WinH);
        NVGui::Setup(Win, Ren);
    }

	for (int i = 0; i != 128; ++i)
	{
		KeyX[i] = (i / 12 * 126 + GenKeyX[i % 12]) * WinW / 1350;
	}


    for (int i = 0; i != 127; ++i)
    {
    	int val;
    	switch (i % 12)
    	{
        	case 1:
        	case 3:
        	case 6:
        	case 8:
        	case 10:
        		val = WinW * 9 / 1350;
        		break;
        	case 4:
        	case 11:
        		val = KeyX[i + 1] - KeyX[i];
        		break;
        	default:
        		val = KeyX[i + 2] - KeyX[i];
        		break;
    	}
    	_KeyWidth[i] = val;
    }
	_KeyWidth[127] = WinW - KeyX[127];

    TW    = scale(684), TH    = scale(610);
    BkeyW = scale(60 ), WkeyW = scale(94 );
    BkeyH = scale(386), WkeyH = scale(608);
	fDeflate = WkeyW * 0.15f / 2.0f;
	fDeflate = floor( fDeflate + 0.5f );
    fDeflate = std::max( std::min( fDeflate, 3.0f ), 1.0f );
}

Canvas::~Canvas()
{
    /*Hmmm this returns suntime error for some reason
    Error:
    void ImGui_ImplSDLRenderer3_Shutdown(): Assertion `bd != nullptr && "No renderer backend to shutdown, or already shutdown?"' failed.
    Well ig this is not a big problem XD
    */
    // Disabled bc I had enough of it lmaooo
    
    //ImGui_ImplSDLRenderer3_Shutdown();
    //ImGui_ImplSDL3_Shutdown();
    //ImGui::DestroyContext();
    
    SDL_DestroySurface(colors);
    SDL_DestroyTexture(Bk0);
    SDL_DestroyTexture(Bk1);
    SDL_DestroyTexture(Wk);
    SDL_DestroyRenderer(Ren);
    SDL_DestroyWindow(Win);
    SDL_Quit();
}

void Canvas::canvas_clear()
{

    SDL_RenderClear(Ren);

    for (int i = 0; i < 128; ++i)
    {
        KeyColor[i] = 0xFFFFFFFF, KeyPress[i] = false;
    }
}

void Canvas::Note(int k, int yb, int ye, unsigned int c)
{
    int x  = KeyX[KeyMap[k]]-1, w = (k >= 75? BkeyW : WkeyW)+1;
    int  h = yb - ye ;
	unsigned short r=c&0xFF;
    unsigned short g=(c&0xFF00)>>8;
	unsigned short b=(c&0xFF0000)>>16;
	unsigned short r1=r*0.6f;
    unsigned short g1=g*0.6f;
	unsigned short b1=b*0.6f;
	unsigned short r2=r*0.2f;
    unsigned short g2=g*0.2f;
	unsigned short b2=b*0.2f;

    // const RGBA_pix &p = getColor(c);
    // SDL_SetTextureColorMod(note, r, g, b);
    // SDL_SetRenderDrawColor(Ren, r, g, b, 255);
    // SDL_FRect R{ (float)x, (float)ye, (float)w+1, (float)h}; //SDL_RenderRect(Ren, &R);
    // SDL_RenderTexture(Ren,note, nullptr, &R);
    // SDL_SetRenderDrawColor(Ren, r/5, g/5, b/5, 255); // Set the border color to green
	DrawRect(Ren, x, ye, w, h, 0xFF000000|r2|g2<<8|b2<<16, 0xFF000000|r2|g2<<8|b2<<16, 0xFF000000|r2|g2<<8|b2<<16, 0xFF000000|r2|g2<<8|b2<<16);
	// SDL_FRect s{ (float)x, (float)ye+1, (float)w, (float)h-2}; //SDL_RenderRect(Ren, &R);
	// SDL_SetTextureColorMod(note, r, g, b);
    // SDL_SetRenderDrawColor(Ren, r, g, b, 255);
    // SDL_RenderTexture(Ren,note, nullptr, &s);
    if(h-2.0f*fDeflate>0)
		DrawRect(Ren, x+fDeflate, ye+fDeflate, w-2.0f*fDeflate, h-2.0f*fDeflate, c,0xFF000000|r1|g1<<8|b1<<16, 0xFF000000|r1|g1<<8|b1<<16,c);

}

void Canvas::DrawKeyBoard()
{
	float fTransitionPct = .02f;
    float fTransitionCY = std::max( 3.0f, std::floor( WinW * 82 / 1000 * fTransitionPct + 0.5f ) );
    float fRedPct = .05f;
    float fRedCY = floor( WinW * 82 / 1000 * fRedPct + 0.5f );
    float fSpacerCY = 2.0f;
    float fTopCY = floor( ( WinW * 82 / 1000 - fSpacerCY - fRedCY - fTransitionCY ) * 0.95f + 0.5f );
    float fNearCY = WinW * 82 / 1000 - fSpacerCY - fRedCY - fTransitionCY - fTopCY;
	float fKeyGap = std::max( 1.0f, std::floor( _KeyWidth[0] * 0.05f + 0.5f ) );
    float fKeyGap1 = fKeyGap - floor( fKeyGap / 2.0f + 0.5f );
	float fCurX = 0;
	float fSharpCY = fTopCY * 0.67f;
    float fCurY = fTransitionCY + fRedCY + fSpacerCY;

	DrawRect(Ren, 0, WinH - WinW * 82 / 1000, WinW, WinW * 82 / 1000, 0xFF000000,0xFF000000,0xFF000000,0xFF000000 );
    // DrawRect(Ren, 0, 0, WinW, fTransitionCY, 0xFF999999, 0xFF999999, 0xFF000000, 0xFF000000 );
    DrawRect(Ren, 0, WinH - WinW * 82 / 1000 + fTransitionCY, WinW, fRedCY, 0xFF06054C, 0xFF06054C, 0xFF0D0A98, 0xFF0D0A98 );
    DrawRect(Ren, 0, WinH - WinW * 82 / 1000 + fTransitionCY + fRedCY, WinW, fSpacerCY, 0xFF1C1C1C, 0xFF1C1C1C, 0xFF1C1C1C, 0xFF1C1C1C );
    for (int i = 0; i != 75; ++i)
    {
	    int j = KeyMap[i];
	    if (!KeyPress[j])//If the key is not pressed
        {
	        DrawRect(Ren, fCurX + fKeyGap1 , fCurY+WinH - WinW * 82 / 1000, _KeyWidth[j] - fKeyGap, fTopCY + fNearCY, 0xFFCCCCCC, 0xFFCCCCCC, 0xFFFFFFFF, 0xFFFFFFFF );
            DrawRect(Ren, fCurX + fKeyGap1 , fCurY + fTopCY+WinH - WinW * 82 / 1000, _KeyWidth[j] - fKeyGap, fNearCY, 0xFFCCCCCC, 0xFFCCCCCC, 0xFF999999, 0xFF999999 );
            DrawRect(Ren, fCurX + fKeyGap1, fCurY + fTopCY+WinH - WinW * 82 / 1000, _KeyWidth[j] - fKeyGap, 2.0f, 0xFF3D3D3D, 0xFF3D3D3D, 0xFF999999, 0xFF999999 );

            if ( j == 60)
            {
                float fMXGap = floor( _KeyWidth[j] * 0.25f + 0.5f );
                float fMCX = _KeyWidth[j] - fMXGap * 2.0f - fKeyGap;
                float fMY = std::max( fCurY + fTopCY - fMCX - 5.0f, fCurY + fSharpCY + 5.0f );
                DrawRect(Ren, fCurX + fKeyGap1 + fMXGap, fMY+WinH - WinW * 82 / 1000, fMCX, fCurY + fTopCY - 5.0f - fMY, 0xFFCCCCCC,0xFFCCCCCC,0xFFCCCCCC,0xFFCCCCCC );
            }
        }
        else
        {
	        unsigned int  c = KeyColor[j];
	        unsigned short r = (c&0xFF);
		    unsigned short g = ((c&0xFF00)>>8);
		    unsigned short b = ((c&0xFF0000)>>16);
		    unsigned short r1 = r*0.8f;
            unsigned short g1 = g*0.8f;
            unsigned short b1 = b*0.8f;
       	    unsigned short r2 = r*0.6f;
            unsigned short g2 = g*0.6f;
            unsigned short b2 = b*0.6f;
            // unsigned int l=0xFF000000|r1|g1<<8|b1<<16;
	        DrawRect(Ren, fCurX + fKeyGap1 , fCurY+WinH - WinW * 82 / 1000, _KeyWidth[j] - fKeyGap, fTopCY + fNearCY - 2.0f, 0xFF000000|r1|g1<<8|b1<<16,0xFF000000|r1|g1<<8|b1<<16, c, c );
	        DrawRect(Ren, fCurX + fKeyGap1 , fCurY + fTopCY + fNearCY - 2.0f+WinH - WinW * 82 / 1000, _KeyWidth[j] - fKeyGap, 2.0f, 0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16 );
	        if ( j == 60 )
            {
                float fMXGap = floor( _KeyWidth[j] * 0.25f + 0.5f );
                float fMCX = _KeyWidth[j] - fMXGap * 2.0f - fKeyGap;
                float fMY = std::max( fCurY + fTopCY + fNearCY - fMCX - 7.0f, fCurY + fSharpCY + 5.0f );
                // DrawRect(Ren, fCurX + fKeyGap1+WinH - WinW * 82 / 1000 + fMXGap, fMY, fMCX, fCurY + fTopCY + fNearCY - 7.0f - fMY, m_csKBWhite.iDarkRGB );
                DrawRect(Ren, fCurX + fKeyGap1 + fMXGap , fMY+WinH - WinW * 82 / 1000, fMCX, fCurY + fTopCY + fNearCY - 7.0f - fMY, 0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16,0xFF000000|r2|g2<<8|b2<<16);
            }
        }


	    //DrawGrayRect(_KeyX[j], 0, _KeyWidth[j], _KeyHeight,KeyColors[j],1.05);
	    //DrawRectangle(_KeyX[j], 0, _KeyWidth[j] + 1, _KeyHeight, 0xFF000000); // Drawing separator lines between keys
	    //DrawGrayRect(_KeyX[j]+1, 0, _KeyWidth[j]-1, bgr/3+1,KeyColors[j],2); // The darker color at the bottom of the white key after it is pressed
	    //if (!KeyPressed[j])//If the key is not pressed
	    //{
	    //	DrawRectangle(_KeyX[j], 0, _KeyWidth[j]+1, bgr, 0xFF000000);
	    //	Draw3Drect2(_KeyX[j]+1, 1, _KeyWidth[j]-1, bgr-2, 0xFFAFAFAF,1.3);
	    //}
		//Drawing shadows on the bottom of the keys
		DrawRect(Ren, floor( fCurX + fKeyGap1 + _KeyWidth[j] - fKeyGap + 0.5f ), fCurY+WinH - WinW * 82 / 1000, fKeyGap, fTopCY + fNearCY, 0xFF000000, 0xFF999999, 0xFF999999, 0xFF000000 );
		fCurX+=_KeyWidth[j];
    }
    float fSharpTop = SharpRatio * 0.7f;

    fCurY = fTransitionCY + fRedCY + fSpacerCY;
	for (int i=75; i != 128; ++i)
    {
	    int j = KeyMap[i];
	    float fNudgeX = 0.3;
        // MIDI::Note eNote = MIDI::NoteVal( i );
        // if ( eNote == MIDI::CS || eNote == MIDI::FS ) fNudgeX = -SharpRatio / 5.0f;
        // else if ( eNote == MIDI::AS || eNote == MIDI::DS ) fNudgeX = SharpRatio / 5.0f;
        fCurX = KeyX[j];
        const float cx = _KeyWidth[0] * SharpRatio;
        const float x = fCurX - _KeyWidth[0] * ( SharpRatio / 2.0f - fNudgeX );
        const float fSharpTopX1 = x + _KeyWidth[0] * ( SharpRatio - fSharpTop ) / 2.0f;
        const float fSharpTopX2 = fSharpTopX1 + _KeyWidth[0] * fSharpTop;
	    if (!KeyPress[j])//If the key is not pressed
        {
            DrawSkew(Ren, fSharpTopX1, fCurY + fSharpCY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY + fSharpCY - fNearCY+WinH - WinW * 82 / 1000, x + cx, fCurY + fSharpCY+WinH - WinW * 82 / 1000, x, fCurY + fSharpCY+WinH - WinW * 82 / 1000, 0xFF404040,0xFF404040, 0xFF000000, 0xFF000000 );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY + fSharpCY - fNearCY+WinH - WinW * 82 / 1000, x, fCurY + fSharpCY+WinH - WinW * 82 / 1000, x, fCurY+WinH - WinW * 82 / 1000, 0xFF404040,0xFF404040, 0xFF000000, 0xFF000000 );
            DrawSkew(Ren, fSharpTopX2, fCurY + fSharpCY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNearCY+WinH - WinW * 82 / 1000, x + cx, fCurY+WinH - WinW * 82 / 1000, x + cx, fCurY + fSharpCY+WinH - WinW * 82 / 1000, 0xFF404040,0xFF404040, 0xFF000000, 0xFF000000 );
            DrawRect(Ren, fSharpTopX1, fCurY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX2 - fSharpTopX1, fSharpCY, 0xFF000000,0xFF000000,0xFF000000,0xFF000000 );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNearCY+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f+WinH - WinW * 82 / 1000, 0xFF202020,0xFF202020,0xFF404040,0xFF404040 );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.65f+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.55f+WinH - WinW * 82 / 1000, 0xFF404040,0xFF404040, 0xFF000000, 0xFF000000);
        }
        else
        {
	        const float fNewNear = fNearCY * 0.25f;
		    unsigned int  c = KeyColor[j];
	        unsigned short r = (c&0xFF);
		    unsigned short g = ((c&0xFF00)>>8);
		    unsigned short b = ((c&0xFF0000)>>16);
		    unsigned short r1 = r*0.5f;
            unsigned short g1 = g*0.5f;
            unsigned short b1 = b*0.5f;
            DrawSkew(Ren, fSharpTopX1, fCurY + fSharpCY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY + fSharpCY - fNewNear+WinH - WinW * 82 / 1000, x + cx, fCurY + fSharpCY+WinH - WinW * 82 / 1000, x, fCurY + fSharpCY+WinH - WinW * 82 / 1000, c, c, 0xFF000000|r1|g1<<8|b1<<16, 0xFF000000|r1|g1<<8|b1<<16 );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY + fSharpCY - fNewNear+WinH - WinW * 82 / 1000, x, fCurY + fSharpCY+WinH - WinW * 82 / 1000, x, fCurY+WinH - WinW * 82 / 1000, c, c, 0xFF000000|r1|g1<<8|b1<<16, 0xFF000000|r1|g1<<8|b1<<16 );
            DrawSkew(Ren, fSharpTopX2, fCurY + fSharpCY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNewNear+WinH - WinW * 82 / 1000, x + cx, fCurY+WinH - WinW * 82 / 1000, x + cx, fCurY + fSharpCY+WinH - WinW * 82 / 1000, c, c, 0xFF000000|r1|g1<<8|b1<<16, 0xFF000000|r1|g1<<8|b1<<16 );
            DrawRect(Ren, fSharpTopX1, fCurY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX2 - fSharpTopX1, fSharpCY, 0xFF000000|r1|g1<<8|b1<<16,0xFF000000|r1|g1<<8|b1<<16,0xFF000000|r1|g1<<8|b1<<16,0xFF000000|r1|g1<<8|b1<<16 );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNewNear+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f+WinH - WinW * 82 / 1000, c, c, c, c );
            DrawSkew(Ren, fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f+WinH - WinW * 82 / 1000, fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.75f+WinH - WinW * 82 / 1000, fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.65f+WinH - WinW * 82 / 1000, c, c, 0xFF000000|r1|g1<<8|b1<<16, 0xFF000000|r1|g1<<8|b1<<16 );
        }
    }

    /*int y = WinH - TH + 1, i, y_;

    for (i = 0, y_ = y + WkeyH; i < 75; ++i)
    {
        int k = KeyMap[i], x_ = KeyX[k] + WkeyW;
        unsigned int  c = KeyColor[k];

        for (int _y = y; _y < y_; ++_y)
        {
            // const RGBA_pix &p = getColor(c);
			unsigned short r=c&0xFF;
    	    unsigned short g=(c&0xFF00)>>8;
            unsigned short b=(c&0xFF0000)>>16;
            SDL_SetRenderDrawColor(Ren, r, g, b, 255);
            SDL_RenderLine(Ren, KeyX[k], _y, x_, _y);
        }
    }

    for (i = 0; i < 11; ++i)
    {
        SDL_FRect Box{(float)TX[i], (float)y - 1, (float)TW, (float)TH};
        SDL_RenderTexture(Ren, Wk, nullptr, &Box);
    }

    for (i = 75, y_ = y + BkeyH; i < 128; ++i)
    {
        int k = KeyMap[i], x_ = KeyX[k] + BkeyW;
        unsigned int c = KeyColor[k];

        for (int _y = y; _y < y_; ++_y)
        {

            unsigned short r=c&0xFF;
    	    unsigned short g=(c&0xFF00)>>8;
            unsigned short b=(c&0xFF0000)>>16;
            SDL_SetRenderDrawColor(Ren, r, g, b, 255);
            SDL_RenderLine(Ren, KeyX[k], _y, x_, _y);
        }
    }

    for (i = 75; i < 128; ++i)
    {
        int k = KeyMap[i];
        SDL_FRect Box{ (float)KeyX[k], (float)y - 1, (float)BkeyW + 2, (float)BkeyH + 3};
        SDL_RenderTexture(Ren, KeyPress[k]? Bk1 : Bk0, nullptr, &Box);
    }*/
}

void Canvas::DrawNote(NVi::u16_t k, const NVnote &n, int pps)
{
    unsigned int c = Col[(n.track % 16 + n.chn) % 16];
    int key = KeyMap[k];
    
    int y_0 = std::clamp((int)floor(_WinH - (n.Tstart - Tplay) * pps + 0.5f), 0, _WinH);
    int y_1 = (n.Tend < Tplay + Tscr) ? std::clamp((int)floor(_WinH - (n.Tend - Tplay) * pps + 0.5f), 0, _WinH) : 0;
    
        if (n.Tstart <= Tplay && Tplay < n.Tend)
        {
            CvWin->KeyPress[key] = true;
            CvWin->KeyColor[key] = c;
        }
    
        CvWin->Note(k, y_0, y_1, c);
}

int Canvas::scale(int x) const
{
    // Fixed a small note glitch LMfaoerGdfgg
    return (x * WinW + 4700) / 7330;
    //return (x * WinW + 3665) / 7330;
}

Canvas::RGBA_pix Canvas::getColor(color C) const
{
    int x = C.real() + 1024.5f, y = C.imag() + 1024.5f;
    return *((RGBA_pix*)colors->pixels + (x + y * 2048));
}
