#include <vector>
#include <cstdint>

enum class EventType {
  Note,
  Rest,
  Octave,
  Tempo,
  Volume,
  DutyCycle,
  Vibrato,
  NoteType,
  PitchSlide,
  Call,
  Return,
  LoopStart,
  LoopEnd
};

struct Event {
  EventType type;
  int a = 0;   // pitch / param 1
  int b = 0;   // duration / param 2
  int c = 0;   // optional param 3
};

// ---------- CHANNEL 1 ----------
const std::vector<Event> Music_TitleScreen_Ch1 = {
    {EventType::Tempo, 144},
    {EventType::Volume, 7, 7},
    {EventType::Vibrato, 9, 3, 4},
    {EventType::DutyCycle, 3},

    {EventType::NoteType, 12, 12, 1},
    {EventType::Octave, 2},
    {EventType::Note, 'E', 1},
    {EventType::Note, 'G', 1},
    {EventType::Note, 'B', 1},
    {EventType::Octave, 3},
    {EventType::Note, 'D', 1},
    {EventType::Octave, 2},
    {EventType::Note, 'G', 4},
    {EventType::Note, 'G', 6},
    {EventType::Note, 'G', 1},
    {EventType::Note, 'G', 1},
    {EventType::Note, 'G', 4},
    {EventType::Note, 'G', 4},
    {EventType::Note, 'G', 4},

    {EventType::NoteType, 8, 12, 1},
    {EventType::Note, 'A', 2},
    {EventType::Note, 'A', 2},
    {EventType::Note, 'A', 2},
    {EventType::Note, 'A', 2},
    {EventType::Note, 'A', 2},
    {EventType::Note, 'F', 2}, // F#

    // main loop marker (logical only)
    {EventType::LoopStart},

    // .sub1
    {EventType::Call, 1},
    // .sub2
    {EventType::Call, 2},
    // .sub1 again
    {EventType::Call, 1},

    {EventType::Octave, 3},
    {EventType::Note, 'C', 8},

    {EventType::NoteType, 8, 12, 6},
    {EventType::Note, 'E', 4},
    {EventType::Note, 'E', 4},
    {EventType::Note, 'C', 4},

    {EventType::NoteType, 12, 12, 6},
    {EventType::Octave, 2},
    {EventType::Note, 'B', 8},

    {EventType::NoteType, 8, 14, 7},
    {EventType::Octave, 3},
    {EventType::Note, 'F', 4},
    {EventType::Note, 'E', 4},
    {EventType::Note, 'C', 4},

    {EventType::NoteType, 12, 14, 7},
    {EventType::Note, 'D', 10},

    {EventType::NoteType, 12, 12, 6},
    {EventType::Octave, 2},
    {EventType::Note, 'B', 2},
    {EventType::Octave, 3},
    {EventType::Note, 'C', 2},
    {EventType::Note, 'D', 2},

    {EventType::LoopEnd}
};

// ---------- SUBROUTINES ----------
const std::vector<Event> TitleScreen_Sub1 = {
    {EventType::NoteType, 12, 12, 6},
    {EventType::Octave, 3},
    {EventType::Note, 'D', 6},
    {EventType::Octave, 2},
    {EventType::Note, 'B', 2},
    {EventType::Octave, 3},
    {EventType::Note, 'D', 8},
    {EventType::Return}
};

const std::vector<Event> TitleScreen_Sub2 = {
    {EventType::Note, 'C', 6},
    {EventType::Note, 'F', 6},
    {EventType::Note, 'C', 4},
    {EventType::Note, 'D', 8},

    {EventType::NoteType, 12, 14, 7},
    {EventType::Note, 'F', 6},
    {EventType::Note, 'E', 1},
    {EventType::Note, 'D', 1},
    {EventType::Note, 'D', 8},

    {EventType::NoteType, 8, 12, 6},
    {EventType::Note, 'C', 4},
    {EventType::Octave, 2},
    {EventType::Note, 'B', 4},
    {EventType::Octave, 3},
    {EventType::Note, 'C', 4},

    {EventType::Return}
};

const std::vector<Event> TitleScreen_Sub3 = {
    {EventType::NoteType, 12, 12, 1},
    {EventType::Note, 'D', 1},
    {EventType::Rest, 1},
    {EventType::Octave, 2},

    // repeated drum-like pattern of D
    {EventType::Note, 'D', 1},
    {EventType::Note, 'D', 1},
    {EventType::Note, 'D', 1},
    {EventType::Rest, 1},

    {EventType::Return}
};

// ---------- CHANNEL 2 ----------
const std::vector<Event> Music_TitleScreen_Ch2 = {
    {EventType::Vibrato, 16, 4, 6},
    {EventType::DutyCycle, 1},

    {EventType::NoteType, 12, 14, 1},
    {EventType::Octave, 2},
    {EventType::Note, 'G', 1},
    {EventType::Note, 'B', 1},
    {EventType::Octave, 3},
    {EventType::Note, 'D', 1},
    {EventType::Note, 'F', 1},
    {EventType::Note, 'G', 4},

    {EventType::LoopStart},

    {EventType::Call, 1},
    {EventType::Call, 2},
    {EventType::Call, 1},

    {EventType::Note, 'A', 4},
    {EventType::Note, 'F', 4},

    {EventType::LoopEnd}
};

// ---------- CHANNEL 3 ----------
const std::vector<Event> Music_TitleScreen_Ch3 = {
    {EventType::NoteType, 12, 1, 0},
    {EventType::Octave, 3},

    {EventType::Note, 'G', 1},
    {EventType::Rest, 1},
    {EventType::Note, 'D', 1},
    {EventType::Rest, 1},

    {EventType::LoopStart},

    {EventType::Call, 1},
    {EventType::Call, 2},

    {EventType::LoopEnd}
};

// ---------- CHANNEL 4 (DRUMS) ----------
struct DrumEvent {
  int speed;
  int drum;
  int duration;
};

const std::vector<DrumEvent> Music_TitleScreen_Ch4 = {
    {6, 3, 1},
    {6, 3, 1},
    {6, 4, 1},
    {6, 4, 1},

    {12, 2, 1},
    {12, 2, 1},
    {12, 2, 1}
};