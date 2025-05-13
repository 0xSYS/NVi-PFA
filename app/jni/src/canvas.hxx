
#pragma once


#include <complex>
#include <unordered_map>
#include <SDL3/SDL.h>
#include "Nlist.hxx"

extern const unsigned char KeyMap[128];


// Required by the track color distributor
struct PairHash
{
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // Combine the two hashes
    }
};


static unsigned int Col[]= 
{
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

inline unsigned int note_color;

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
	const SDL_DisplayMode *mod;
    int   TH, WinW, WinH;
    bool  KeyPress[128];
    unsigned int KeyColor[128];

    Canvas(); 
    ~Canvas();

    void canvas_clear();

    void DrawKeyBoard();
    unsigned int GenerateRandomColor();
    void DrawNote(NVi::u16_t k, const NVnote &n, int pps);
    void ClearTrackChannelColors();

    void Note(int k, int yb, int ye, unsigned int c);

private:

    std::unordered_map<std::pair<int, int>, unsigned int, PairHash> trackChannelColorMap;

    SDL_Texture *Bk0, *Bk1, *Wk, *note;
    SDL_Surface *colors;

    int BkeyW, BkeyH, WkeyW, WkeyH;
    int TW, TX[11], KeyX[128];

    int scale(int x) const;

    RGBA_pix getColor(color C) const;
};


inline Canvas     *CvWin;