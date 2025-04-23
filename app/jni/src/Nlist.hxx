// <NVirsual> Nlist.hxx 2021-11-18 by 云中龙++

#pragma once


#include "Sequ.hxx"

#include <stack>
#include <list>

struct NVnote    /* ===== Note class rendering ===== */
{
    NVi::u16_t   track;
    double       Tstart, Tend;
    NVi::nv_byte chn, key, vel;

    NVnote(double T, const NVseq_event &E);
};

class NVnoteList  /* ===== Note queue class ===== */
{
public:

    NVmidiFile M;      // MIDI File
    double Tread;      // Current read position
    std::list<NVnote> L[128]; // Note List

    /* MIDI Parsing */
    bool start_parse(const char *name);

    /* Close component */
    void destroy_all();

    /* Locate to T seconds and clear the list */
    void list_seek(double T);

    /* Put the notes before the Tth second into the list */
    void update_to(double T);

    void OR();  // ppl in the Black MIDI Community knows what that means lol

    /* Remove notes in the list up to T seconds ago */
    void remove_to(double T);

private:

    NVsequencer S;       // Event sequencer
    double     dT;       // Speed
    NVi::u32_t abstick;  // Current read position in ticks
    std::stack<std::list<NVnote>::iterator> (*keys)[128];
};
