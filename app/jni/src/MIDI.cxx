#include <cstdio>



#include "MIDI.hxx"
using namespace NVi;


bool NVmidiFile::mid_open(const char *name)
{
    // Midi file opening
    FILE* fp;
    fp=fopen(name, "rb");
    u32_t size = 0;
    u32_t tmp = 0;
    if (fp == nullptr)
    {
        error("MIDI", "'%s' Failed to open midi file !\n", name);
        info ("MIDI", "Please check midi file path.\n");
        return false;
    }

    if (fread(&tmp, 4, 1, fp) != 1)
    {
        error("MIDI", "'%s' : MIDI File is corrupt !\n", name);
        return (fclose(fp), false);
    }

    if (tmp != "MThd"_u64be)
    {
        error("MIDI", "'%s:' Incompatible MIDI File type !\n", name);
        return (fclose(fp), false);
    }

    fread(&size  , 4, 1, fp); revU32(size);
    fread(&type  , 2, 1, fp); revU16(type);
    fread(&tracks, 2, 1, fp); revU16(tracks);
    fread(&ppnq  , 2, 1, fp); revU16(ppnq);
#ifdef __linux__
    fseek(fp, SEEK_SET, size + 8);
#endif

#if defined(_WIN32) || defined(_WIN64)
    fseek(fp, size + 8, SEEK_SET);
#endif

    trk_over = new bool     [tracks];
    trk_data = new nv_byte* [tracks];
    trk_ptr  = new nv_byte* [tracks];
    grp_code = new nv_byte  [tracks];
    
    info("MIDI", "Total trk count: %d\n", tracks);
    
    total_track_count = tracks;

    for (u16_t trk = 0; trk < tracks; ++trk)
    {
        fread(&tmp , 4, 1, fp);
        fread(&size, 4, 1, fp);

        if (tmp != "MTrk"_u64be)
        {
            tracks = trk, mid_close();
            error("MIDI", "Track corrupted !\n");
            info("MIDI", "@Track%hd\n", trk);
            return (fclose(fp), false);
        }
        else
            info("MIDI", "Loaded Track: %hd\n", trk);

        tmp = 0; revU32(size);
        trk_data[trk] = new nv_byte [size];
        fread(trk_data[trk], size,  1, fp);
    }

    return (rewind_all(), fclose(fp), true);
}

void NVmidiFile::rewind_all()
{
    for (u16_t trk = 0; trk < tracks; ++trk)
    {
        trk_over[trk] = false;
        grp_code[trk] = 0x0Fu;
        trk_ptr [trk] = trk_data[trk];
    }
}

void NVmidiFile::mid_close()
{
    for (u16_t trk = 0; trk < tracks; ++trk)
    {
        delete[] trk_data[trk];
    }

    delete[]  trk_data; 
    delete[]   trk_ptr;
    trk_data = nullptr; 
    trk_ptr  = nullptr;
    delete[]  trk_over; 
    delete[]  grp_code;
    trk_over = nullptr; 
    grp_code = nullptr;
}

static inline u32_t getVLi_U32(nv_byte **p)
{
    u32_t VLi32 = **p & 0x7Fu;

    while (*(*p)++ & 0x80u)
    {
        VLi32 = VLi32 << 7 | (**p & 0x7Fu);
    }

    return VLi32;
}

bool NVmidiEvent::get(u16_t track, NVmidiFile &midi)
{
    if (midi.trk_over[track])
    {
        return false;
    }
    
    //NVi::info("MIDI", "Track: %hd\n", midi.trk_over[track]);

    nv_byte code, **p = midi.trk_ptr + track;

    tick = getVLi_U32(p);

    if (**p & 0x80u)
    {
        code = midi.grp_code[track] = *(*p)++;
    }
    else
    {
        code = midi.grp_code[track];
    }

    chan = code & 0x0Fu;

    switch (type = (NV_METYPE)(code & 0xF0u))
    {
    case (NV_METYPE::NOFF):
    case (NV_METYPE::NOON):
    case (NV_METYPE::NOAT):
    case (NV_METYPE::CTRO): num   = *(*p)++;
    case (NV_METYPE::PROG):
    case (NV_METYPE::CHAT): value = *(*p)++;

        break;

    case (NV_METYPE::PITH):

            value = *(*p)++; // Obtaining lower octal
            value |= (*(*p)++) << 7; // Obtaining large combined bytes
        break;

    case (NV_METYPE::SYSC):

        if (code == 0xFFu)
        {
            if ((num = *(*p)++) == 0x2Fu)
            {
                midi.trk_over[track] = true;
            }

            type = NV_METYPE::META;
        }
        else
        {
            num = code & 0x0Fu;
        }

        datasz = getVLi_U32(p); data = *p;
        *p = *p + datasz; chan = (nv_byte)-1;
        break;

    default:

        warn("MIDI", "Unknown event type on track%hd !\n", track);
        info("MIDI", "@%08x\n", *p - midi.trk_data[track]);
        return false;
    }

    return true;
}
