// <NVirsual> MIDI.hxx 2021-11-18 by 云中龙++

#pragma once

#include "Utils.hxx"

enum class NV_METYPE  /* === MIDI Event types === */
{
    NOFF = (NVi::nv_byte)0x80, // Noteoff
    NOON = (NVi::nv_byte)0x90, // Noteon
    NOAT = (NVi::nv_byte)0xA0, // Polifonic Key Pressure
    CTRO = (NVi::nv_byte)0xB0, // ControlChange
    PROG = (NVi::nv_byte)0xC0, // ProgramChange
    CHAT = (NVi::nv_byte)0xD0, // Channel
    PITH = (NVi::nv_byte)0xE0, // Pitchbend
    SYSC = (NVi::nv_byte)0xF0, // System Exclusive
    META = (NVi::nv_byte)0xFF, // Meta-Event
};

struct NVmidiFile   /* ===== MIDI file type ===== */
{
    /* Type, number of tracks, resolution */
    NVi::u16_t type, tracks, ppnq;

    bool         *trk_over;   // End of track
    NVi::nv_byte **trk_data;  // Orbital data
    NVi::nv_byte **trk_ptr;   // Readout position
    NVi::nv_byte *grp_code;   // Event group

    /* Opening MIDI File */
    bool mid_open(const char *name);

    void rewind_all();  // Reset track pointers

    void mid_close();   // Close midi file
};

struct NVmidiEvent  /* =====  MIDI Event Class ===== */
{
    NV_METYPE    type;
    NVi::u32_t   tick;
    NVi::nv_byte chan, num;
    NVi::u16_t   value;
    NVi::size_t  datasz;
    const NVi::nv_byte *data;

    /* Get events from a specified track of a specified file */
    bool get(NVi::u16_t track, NVmidiFile &midi);
};
