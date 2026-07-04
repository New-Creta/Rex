#pragma once
#include <cstdint>

namespace Music_Celadon
{
  enum class Cmd : uint8_t
  {
    Tempo,
    Volume,
    DutyCycle,
    TogglePerfectPitch,
    NoteType,
    Octave,
    Note,
    Rest,
    Loop
  };

  struct Command
  {
    Cmd type;
    int a = 0;
    int b = 0;
    int c = 0;
  };

  // -------------------------
  // Channel 1
  // -------------------------
  static const Command Ch1[] =
  {
      {Cmd::Tempo, 144},
      {Cmd::Volume, 7, 7},
      {Cmd::DutyCycle, 3},
      {Cmd::TogglePerfectPitch},
      {Cmd::NoteType, 12, 2, -7},
      {Cmd::Rest, 8},
      {Cmd::Octave, 3},
      {Cmd::Note, 'D', 8},

      // .mainloop
      {Cmd::DutyCycle, 2},
      {Cmd::NoteType, 12, 11, 2},

      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'B', 4},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, 'B', 4},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, 'G', 1},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, 'B', 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 2},

      {Cmd::Octave, 3},
      {Cmd::Note, 'A', 8},
      {Cmd::Note, "F#", 4},   // representation note
      {Cmd::Note, 'A', 4},
      {Cmd::Note, "F#", 2},
      {Cmd::Note, 'A', 6},
      {Cmd::Note, 'A', 1},
      {Cmd::Note, 'B', 1},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 1},

      {Cmd::Octave, 3},
      {Cmd::Note, 'B', 1},
      {Cmd::Note, 'A', 2},
      {Cmd::Note, 'B', 2},
      {Cmd::Note, 'G', 4},

      {Cmd::Octave, 4},
      {Cmd::Note, 'G', 4},

      {Cmd::Octave, 3},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'B', 4},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, 'B', 4},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, "F#", 1},
      {Cmd::Note, 'G', 1},
      {Cmd::Note, 'A', 1},
      {Cmd::Note, 'B', 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 2},

      {Cmd::Octave, 3},
      {Cmd::Note, 'A', 8},
      {Cmd::Note, 'A', 2},
      {Cmd::Note, 'B', 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 2},

      {Cmd::Octave, 3},
      {Cmd::Note, 'B', 2},
      {Cmd::Note, 'A', 2},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, "F#", 2},
      {Cmd::Note, 'G', 2},
      {Cmd::Note, "F#", 4},

      {Cmd::Note, 'D', 1},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, "F#", 1},
      {Cmd::Note, 'G', 1},
      {Cmd::Note, 'A', 8},

      {Cmd::NoteType, 12, 9, 4},

      {Cmd::Note, 'B', 8},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'D', 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'A', 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 2},

      {Cmd::Octave, 3},
      {Cmd::Note, 'B', 8},
      {Cmd::Note, 'A', 8},
      {Cmd::Note, "F#", 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'A', 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, "F#", 4},
      {Cmd::Note, 'A', 4},

      {Cmd::Note, 'B', 8},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'D', 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'A', 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'C', 2},

      {Cmd::Octave, 3},
      {Cmd::Note, 'B', 8},
      {Cmd::Note, 'A', 8},
      {Cmd::Note, "F#", 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, 'A', 4},
      {Cmd::Note, 'G', 4},
      {Cmd::Note, "F#", 4},
      {Cmd::Note, 'A', 4},

      {Cmd::Loop, 0}
  };

  // -------------------------
  // Channel 2
  // -------------------------
  static const Command Ch2[] =
  {
      {Cmd::DutyCycle, 3},
      {Cmd::NoteType, 12, 12, 2},

      {Cmd::Octave, 4},
      {Cmd::Note, 'D', 1},
      {Cmd::Note, "C#", 1},
      {Cmd::Note, 'D', 1},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, "F#", 1},
      {Cmd::Note, 'E', 1},
      {Cmd::Note, "F#", 1},
      {Cmd::Note, 'G', 1},

      {Cmd::NoteType, 12, 10, 0},
      {Cmd::Note, 'A', 8},

      {Cmd::Loop, 0}
  };

  // -------------------------
  // Channel 3
  // -------------------------
  static const Command Ch3[] =
  {
      {Cmd::NoteType, 12, 1, 3},
      {Cmd::Rest, 8},

      {Cmd::Loop, 0}
  };
}