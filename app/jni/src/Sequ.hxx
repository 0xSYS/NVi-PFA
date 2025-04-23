// <NVirsual> Sequ.hxx 2021-11-18 by 云中龙++

#pragma once


#include "MIDI.hxx"

struct NVseq_event: /* Sequencer MIDI event class */
    public NVmidiEvent
{
    NVi::u16_t track;   // Orbital number

    NVi::u32_t abstick; // Absolute Tick
};

class NVsequencer /* == sequencer class (computing) == */
{
public:

    /* Initialize this serializer */
    void seq_start(NVmidiFile &midi);

    /* Reset this Sequencer */
    void seq_reset(NVmidiFile &midi);

    /* Get the next MIDI event */
    void seq_next(NVmidiFile &midi);

    /* Current MIDI events */
    const NVseq_event& event() const;

    /* Close this Sequencer */
    void seq_destroy();

private:

    NVi::u32_t   L;    // Maximum number of nodes
    NVi::u16_t  *T;    // Choice Tree
    NVseq_event *E;    // event buffer

    /* Update the specified node of the choice tree */
    void update(int p, int a, int b);
};
