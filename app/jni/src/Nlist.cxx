#include "Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"







template<typename T> struct rP    { using t = T; };

template<typename T> struct rP<T*>{ using t = T; };

NVnote::NVnote(double T, const NVseq_event &E):
    track(E.track), Tstart(T), Tend(114514191981.0),
    chn(E.chan), key(E.num), vel(E.value){ }

bool NVnoteList::start_parse(const char *name)
{
    if (!M.mid_open(name))
    {
        return false;
    }

    if (M.type == 2)
    {
        NVi::error("Nlist", "Invalid midi file format !\n");
        NVi::info ("Nlist", "format 0 / format 1 are compatible.\n");
        return (M.mid_close(), false);
    }

    S.seq_start(M);
    abstick = 0; Tread = 0.0; dT = 0.5 / M.ppnq;
    keys = new rP<decltype(keys)>::t [M.tracks];
    return true;
}

void NVnoteList::destroy_all()
{
    delete[] keys; keys =  nullptr;
    M.mid_close(); S.seq_destroy();
}

void NVnoteList::list_seek(double T)
{
    if (T < Tread)
    {
        abstick = 0; Tread = 0.0; dT = 0.5 / M.ppnq;
        M.rewind_all(); S.seq_reset(M);
    }

    for (int i = 0; i < 128; ++i)
    {
        L[i].clear();

        for (NVi::u16_t t = 0; t < M.tracks; ++t)
        {
            while (!keys[t][i].empty())
            {
                keys[t][i].pop();
            }
        }
    }

    while (S.event().track < M.tracks)
    {
        const NVseq_event  &E  =  S.event();
        Tread += dT * (E.abstick - abstick);
        abstick = E.abstick;

        if (Tread >= T){ break; }

        if (E.type == NV_METYPE::META && E.num == 0x51u)
        {
            NVi::u32_t   speed    =   E.data[0];
            speed = speed << 8 | E.data[1];
            speed = speed << 8 | E.data[2];
            dT = 0.000001 * speed / M.ppnq;
        }

        S.seq_next(M);
    }
}

void NVnoteList::update_to(double T)
{
    //std::cout << "Update\n";
    while (S.event().track < M.tracks)
    {
        const NVseq_event  &E  =  S.event();
        Tread += dT * (E.abstick - abstick);
        abstick = E.abstick;

        if (Tread >= T){ break; }

        switch (E.type)
        {
        case (NV_METYPE::META):

            if (E.num == 0x51u)
            {
                NVi::u32_t   speed    =   E.data[0];
                speed = speed << 8 | E.data[1];
                speed = speed << 8 | E.data[2];
                dT = 0.000001 * speed / M.ppnq;
            }

            break;

        case (NV_METYPE::NOON):

            if (E.value > 0)
            {
                L[E.num].emplace_back(Tread, E);
                auto nt = L[E.num].end();
                keys[E.track][E.num].push(--nt);
                break;
            }

        case (NV_METYPE::NOFF):

            if (!keys[E.track][E.num].empty())
            {
                keys[E.track][E.num].top()->Tend = Tread;
                keys[E.track][E.num].pop();
            }

        default: break;
        }

        S.seq_next(M);
    }
}

void NVnoteList::OR()        // A presumably useful overlap remover
{
    NVi::info("Nlist", "Applying OR\n");
    for (int i = 0; i < 128; ++i)
    {
        std::list<NVnote>::iterator p = L[i].end();
        double T0 = 114514191981.0;
        double T1 = 114514191981.0;

        while (p-- != L[i].begin())
        {
            if (T0 < p->Tend && p->Tend < T1)
            {
                p->Tend = T0, T0 = p->Tstart;

                if (p->Tend - p->Tstart < 1e-7)
                {
                    p = L[i].erase(p);
                }
            }
            else
            {
                T0 = p->Tstart, T1 = p->Tend;
            }
        }
    }
}

void NVnoteList::remove_to(double T)
{
    for (int i = 0; i < 128; ++i)
    {
        std::list<NVnote>::iterator p = L[i].begin();

        while (p != L[i].end() && p->Tstart < T)
        {
            p->Tend < T? (p = L[i].erase(p)) : ++p;
        }
    }
}
