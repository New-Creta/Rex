#pragma once
#include <cstdint>

// C++ transcription of RGBDS-style music macros for Game Boy audio engine.
// This is a direct structural translation (not an audio-synth implementation).

namespace Music_BikeRiding
{
  enum class CommandType : uint8_t
  {
    Tempo,
    Volume,
    DutyCycle,
    Vibrato,
    NoteType,
    Octave,
    Note,
    Rest,
    TogglePerfectPitch,
    Loop,
    Call,
    Return
  };

  struct Command
  {
    CommandType type;
    int a = 0;
    int b = 0;
    int c = 0;
  };

  // -------------------------
  // Channel 1
  // -------------------------
  static const Command Ch1[] =
  {
      {CommandType::Tempo, 144},
      {CommandType::Volume, 7, 7},
      {CommandType::DutyCycle, 3},
      {CommandType::Vibrato, 8, 1, 4},
      {CommandType::NoteType, 12, 11, 5},
      {CommandType::Octave, 3},
      {CommandType::Note, 'G', 2},

      // .mainloop
      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 4},
      {CommandType::Note, 'D', 4},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'C', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'G', 2},
      {CommandType::Note, 'G', 2},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'F', 4},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'D', 2},
      {CommandType::Note, 'F', 4},
      {CommandType::Note, 'D', 4},

      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 2},

      {CommandType::Octave, 4},
      {CommandType::Note, 'F', 4},
      {CommandType::Note, 'D', 4},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'G', 2},
      {CommandType::Note, 'C', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'C', 2},
      {CommandType::Note, 'D', 2},
      {CommandType::Note, 'E', 2},

      {CommandType::NoteType, 12, 11, 6},
      {CommandType::Note, 'F', 10},

      {CommandType::NoteType, 12, 10, 6},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'G', 10},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'D', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'F', 6},

      {CommandType::TogglePerfectPitch},

      {CommandType::NoteType, 12, 11, 3},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'D', 2},
      {CommandType::Note, 'D', 1},
      {CommandType::Note, 'E', 1},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'E', 1},
      {CommandType::Note, 'F', 1},

      {CommandType::TogglePerfectPitch},

      {CommandType::NoteType, 12, 11, 5},
      {CommandType::Note, 'G', 6},
      {CommandType::Note, 'G', 6},
      {CommandType::Note, 'A', 2},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'G', 6},

      {CommandType::NoteType, 12, 11, 4},
      {CommandType::Note, 'G', 2},
      {CommandType::Note, 'F', 4},

      {CommandType::NoteType, 12, 10, 4},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'D', 2},

      {CommandType::NoteType, 12, 9, 3},
      {CommandType::Octave, 3},
      {CommandType::Note, 'A', 2},
      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 4},

      {CommandType::Note, 'C', 2},
      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 2},
      {CommandType::Note, 'A', 1},
      {CommandType::Note, 'B', 1},
      {CommandType::Note, 'A', 2},
      {CommandType::Note, 'B', 2},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 2},
      {CommandType::Note, 'C', 4},
      {CommandType::Note, 'C', 2},

      {CommandType::Octave, 3},
      {CommandType::Note, 'A', 2},
      {CommandType::Note, 'B', 2},
      {CommandType::Note, 'B', 2},
      {CommandType::Note, 'A', 2},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 4},
      {CommandType::Octave, 3},
      {CommandType::Note, 'A', 2},
      {CommandType::Note, 'B', 1},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 1},

      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 2},

      {CommandType::Octave, 4},
      {CommandType::Note, 'D', 4},

      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 2},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 4},

      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 1},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 1},
      {CommandType::Note, 'D', 1},

      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 1},

      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 4},

      {CommandType::NoteType, 12, 3, -5},
      {CommandType::Note, 'C', 4},

      {CommandType::NoteType, 12, 11, 4},
      {CommandType::Note, 'F', 6},
      {CommandType::Note, 'G', 4},
      {CommandType::Note, 'F', 1},
      {CommandType::Note, 'G', 1},
      {CommandType::Note, 'F', 4},
      {CommandType::Note, 'E', 6},
      {CommandType::Note, 'F', 2},
      {CommandType::Note, 'E', 2},
      {CommandType::Note, 'D', 1},
      {CommandType::Note, 'E', 1},
      {CommandType::Note, 'D', 2},
      {CommandType::Note, 'C', 2},

      {CommandType::NoteType, 12, 11, 5},
      {CommandType::Octave, 3},
      {CommandType::Note, 'A', 4},
      {CommandType::Octave, 4},
      {CommandType::Note, 'D', 4},
      {CommandType::Octave, 3},
      {CommandType::Note, 'B', 4},
      {CommandType::Octave, 4},
      {CommandType::Note, 'E', 4},
      {CommandType::Note, 'C', 4},
      {CommandType::Note, 'F', 4},
      {CommandType::Note, 'D', 4},
      {CommandType::Note, 'F', '#'}, // placeholder for F#

      {CommandType::Vibrato, 10, 2, 6},
      {CommandType::NoteType, 12, 8, 0},
      {CommandType::Note, 'G', 16},
      {CommandType::Note, 'G', 4},
      {CommandType::NoteType, 12, 8, 7},
      {CommandType::Note, 'G', 12},

      {CommandType::NoteType, 12, 11, 5},
      {CommandType::Vibrato, 8, 1, 4},

      {CommandType::Loop, 0}
  };

  // -------------------------
  // Channel 2
  // -------------------------
  static const Command Ch2[] =
  {
      {CommandType::DutyCycle, 2},
      {CommandType::Vibrato, 6, 1, 5},
      {CommandType::NoteType, 12, 12, 3},
      {CommandType::Octave, 4},
      {CommandType::Note, 'C', 2},

      {CommandType::Loop, 0}
  };

  // -------------------------
  // Channel 3 (arpeggio-like)
  // -------------------------
  static const Command Ch3[] =
  {
      {CommandType::NoteType, 12, 1, 3},
      {CommandType::Rest, 2},

      {CommandType::Loop, 0}
  };

  // -------------------------
  // Channel 4 (drums)
  // -------------------------
  static const Command Ch4[] =
  {
      {CommandType::Loop, 0}
  };
}