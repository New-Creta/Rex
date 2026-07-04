// Music_IntroBattle.cpp
// Pure structural C++ conversion (no ASM strings)

#include <vector>
#include <cstdint>

namespace Music_IntroBattle
{
  enum class EventType
  {
    Note,
    Rest,
    Octave,
    Tempo,
    Volume,
    DutyCycle,
    Vibrato,
    NoteType,
    Drum,
    LoopStart,
    LoopEnd,
    Call,
    Return
  };

  struct Event
  {
    EventType type;
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
  };

  using Track = std::vector<Event>;

  struct Song
  {
    Track ch1;
    Track ch2;
    Track ch3;
    Track ch4;
  };

  static const Song IntroBattle =
  {
    // ===================== CH1 =====================
    {
        {EventType::Tempo, 98},
        {EventType::Volume, 7, 7},
        {EventType::DutyCycle, 3},
        {EventType::Vibrato, 6, 3, 4},
        {EventType::NoteType, 12, 11, 1},
        {EventType::Rest, 8},

        {EventType::Octave, 2},
        {EventType::Note, 9},  // A_
        {EventType::Note, 9},

        {EventType::NoteType, 12, 11, 4},
        {EventType::Octave, 3},
        {EventType::Note, 5},  // D_
        {EventType::NoteType, 12, 11, 1},

        {EventType::Octave, 2},
        {EventType::Note, 9},
        {EventType::Note, 9},

        {EventType::NoteType, 12, 11, 4},
        {EventType::Octave, 3},
        {EventType::Note, 6},  // D#
        {EventType::NoteType, 12, 11, 1},

        {EventType::Octave, 2},
        {EventType::Note, 9},
        {EventType::Note, 9},

        {EventType::NoteType, 12, 11, 4},
        {EventType::Octave, 3},
        {EventType::Note, 5},

        {EventType::NoteType, 12, 10, 0},
        {EventType::Note, 10}, // A#
        {EventType::NoteType, 12, 11, 1},

        {EventType::Note, 9},
        {EventType::Note, 9},

        {EventType::NoteType, 12, 11, 4},
        {EventType::Octave, 3},
        {EventType::Note, 5},

        {EventType::NoteType, 12, 2, -1},
        {EventType::Note, 8}, // G_

        {EventType::NoteType, 12, 11, 0},
        {EventType::Note, 9},

        {EventType::Rest, 16},
        {EventType::NoteType, 12, 11, 1},
        {EventType::Octave, 4},
        {EventType::Note, 5},

        {EventType::Return}
    },

    // ===================== CH2 =====================
    {
        {EventType::DutyCycle, 3},
        {EventType::Vibrato, 8, 2, 5},
        {EventType::NoteType, 12, 12, 2},
        {EventType::Rest, 8},

        {EventType::Octave, 3},
        {EventType::Note, 5},
        {EventType::Note, 5},

        {EventType::NoteType, 12, 12, 5},
        {EventType::Note, 9},

        {EventType::Return}
    },

    // ===================== CH3 =====================
    {
        {EventType::NoteType, 12, 1, 0},
        {EventType::Rest, 8},

        {EventType::Octave, 4},
        {EventType::Note, 5},
        {EventType::Rest},
        {EventType::Note, 5},

        {EventType::Return}
    },

    // ===================== CH4 =====================
    {
        {EventType::Drum, 19, 1},
        {EventType::Drum, 18, 1},
        {EventType::Drum, 17, 4},
        {EventType::Drum, 17, 8},

        {EventType::Return}
    }
  };
}