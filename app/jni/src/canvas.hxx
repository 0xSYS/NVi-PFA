
#pragma once


#include <complex>
#include <SDL3/SDL.h>
#include "Nlist.hxx"

extern const unsigned char KeyMap[128];

static constexpr unsigned int Col[]= 
{
    /*
    0xFFFFCC33,
    0xFFE433FF,
    0xFF99FF33,
    0xFF4B33FF,
    0xFF33FFFF,
    0xFFFF3381,
    0xFF3366FF,
    0xFFFF7E33,
    0xFF33FF66,
    0xFFE7FF33,
    0xFF9933FF,
    0xFF4EFF33,
    0xFFFF33CC,
    0xFF33FFB1,
    0xFFFF3333,
    0xFF33B4FF
    
    
    0xFF3366FF,
    0xFFFF7E33,
    0xFF33FF66,
    0xFFFF3381,
    0xFF33FFFF,
    0xFFE433FF,
    0xFF99FF33,
    0xFF4B33FF,
    0xFFFFCC33,
    0xFF33B4FF,
    0xFFFF3333,
    0xFF33FFB1,
    0xFFFF33CC,
    0xFF4EFF33,
    0xFF9933FF,
    0xFFE7FF33
    */
    0x3366FF,
    0xFF7E33,
    0x33FF66,
    0xFF3381,
    0x33FFFF,
    0xE433FF,
    0x99FF33,
    0x4B33FF,
    0xFFCC33,
    0x33B4FF,
    0xFF3333,
    0x33FFB1,
    0xFF33CC,
    0x4EFF33,
    0x9933FF,
    0xE7FF33
};

class Canvas
{
public:

    using color = std::complex<float>;

    struct RGBA_pix
    {
        unsigned char r, g, b, a;
    };

    SDL_Window   *Win;
    SDL_Renderer *Ren;
    //SDL_Texture  *GuiLayer;
	const SDL_DisplayMode *mod;
    int   TH, WinW, WinH;
    bool  KeyPress[128];
    unsigned int KeyColor[128];

    Canvas(); 
    ~Canvas();

    void canvas_clear();

    void DrawKeyBoard();
    void DrawNote(NVi::u16_t k, const NVnote &n, int pps);

    void Note(int k, int yb, int ye, unsigned int c);

private:

    SDL_Texture *Bk0, *Bk1, *Wk, *note;
    SDL_Surface *colors;

    int BkeyW, BkeyH, WkeyW, WkeyH;
    int TW, TX[11], KeyX[128];

    int scale(int x) const;

    RGBA_pix getColor(color C) const;
};


inline Canvas     *CvWin;