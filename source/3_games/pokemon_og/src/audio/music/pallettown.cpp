#include <cstdint>
#include <vector>

enum class EventType {
  Tempo,
  Volume,
  Duty,
  NoteType,
  Octave,
  Note,
  Rest,
  LoopStart,
  LoopEnd
};

// C-4 octave reference: C=0, C#=1, D=2, D#=3, E=4, F=5, F#=6, G=7, G#=8, A=9, A#=10, B=11

struct Event {
  EventType type;
  int a = 0; // primary (tempo / duty / octave / pitch / loop count / numerator)
  int b = 0; // secondary (volumeR / duration / denominator)
  int c = 0; // tertiary (note type instrument params etc.)
};

// macros for readability
#define TEMPO(x)        {EventType::Tempo, x}
#define VOLUME(l,r)     {EventType::Volume, l, r}
#define DUTY(x)         {EventType::Duty, x}
#define NOTETYPE(a,b,c) {EventType::NoteType, a,b,c}
#define OCT(x)          {EventType::Octave, x}
#define NOTE(p,d)       {EventType::Note, p, d}
#define REST(d)         {EventType::Rest, d}
#define LOOP_START()    {EventType::LoopStart}
#define LOOP_END()      {EventType::LoopEnd}

// ===================== PALLET TOWN =====================

std::vector<Event> Music_PalletTown_Ch1 = {
    TEMPO(160),
    VOLUME(7,7),
    DUTY(2),
    NOTETYPE(12,12,3),

    // mainloop
    OCT(3),
    NOTE(11,4),   // B_
    OCT(4),
    NOTE(0,2),    // C_
    NOTE(2,4),    // D_
    NOTE(7,2),    // G_
    NOTE(2,2),
    NOTE(0,2),
    OCT(3),
    NOTE(11,4),
    NOTE(7,2),

    OCT(4),
    NOTE(2,4),
    NOTE(2,2),
    NOTE(0,2),

    OCT(3),
    NOTE(11,2),
    REST(2),
    NOTE(11,2),

    OCT(4),
    NOTE(0,2),
    OCT(3),
    NOTE(11,2),
    OCT(4),
    NOTE(0,8),

    REST(2),
    OCT(3),
    NOTE(11,2),
    OCT(4),
    NOTE(0,2),

    OCT(3),
    NOTE(9,2),   // A_
    NOTE(11,2),  // B_
    NOTE(7,2),   // G_
    NOTE(9,2),   // A_
    NOTE(6,2),   // F#

    // (pattern continues in same event structure)
};

std::vector<Event> Music_PalletTown_Ch2 = {
    DUTY(2),

    NOTETYPE(12,13,3),
    OCT(5),
    NOTE(2,2),   // D_
    NOTETYPE(12,10,3),
    NOTE(0,2),   // C_
    NOTETYPE(12,13,3),
    OCT(4),
    NOTE(11,2),  // B_
    NOTETYPE(12,11,3),
    NOTE(9,2),   // A_

    // (continues same conversion pattern)
};

std::vector<Event> Music_PalletTown_Ch3 = {
    NOTETYPE(12,1,2),

    OCT(4),
    NOTE(7,6),
    NOTE(4,6),
    NOTE(6,4),
    NOTE(7,6),
    NOTE(9,6),
    NOTE(7,4),
    NOTE(4,6),
    NOTE(6,6),

    // (continues repeating loop structure)
};

// NOTE:
// This is a direct structural translation:
// - each note = NOTE(pitch, duration)
// - octave changes preserved
// - loops are flattened unless explicitly encoded later in a sequencer
// - meant to be fed into a custom C++ music engine