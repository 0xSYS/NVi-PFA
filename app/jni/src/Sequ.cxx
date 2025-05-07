#include "Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"


void NVsequencer::seq_start(NVmidiFile &midi)
{
    E = new NVseq_event [midi.tracks + 1u];
    E[midi.tracks].abstick = 0xFFFFFFFFu;
    E[midi.tracks].track   = midi.tracks;

    for (L = 1; L < midi.tracks; L <<= 1){ }

    T = new NVi::u16_t [L << 1]; seq_reset(midi);
}

void NVsequencer::seq_reset(NVmidiFile &midi)
{
    for (NVi::u32_t i = midi.tracks; i < L; ++i)
    {
        T[i + L] = midi.tracks;
    }

    for (NVi::u16_t i = 0; i < midi.tracks; ++i)
    {
        if (E[i].get(i, midi))
        {
            E[i].track   = T[L + i] = i;
            E[i].abstick = E[i].tick;
        }
        else
        {
            T[L + i] = midi.tracks;
            NVi::warn("Sequ", "Empty track: %d", i);
        }
    }

    for (int i = L - 1; i > 0; --i)
    {
        update(i, T[i << 1], T[i << 1 | 1]);
    }
}

void NVsequencer::seq_next(NVmidiFile &midi)
{
    int a = T[1], p = L + a;

    if (midi.trk_over[a] || !E[a].get(a, midi))
    {
        T[p] = midi.tracks;
    }
    else
    {
        E[a].abstick += E[a].tick;
    }

    for ( ; (a = p >> 1); p = a)
    {
        update(a, T[p], T[p ^ 1]);
    }
}

const NVseq_event& NVsequencer::event() const
{
    return E[T[1]];
}

void NVsequencer::seq_destroy()
{
    delete[]  T; 
    delete[]  E;
    T = nullptr; 
    E = nullptr;
}

void NVsequencer::update(int p, int a, int b)
{
    T[p] = E[a].abstick < E[b].abstick? a : b;
}
