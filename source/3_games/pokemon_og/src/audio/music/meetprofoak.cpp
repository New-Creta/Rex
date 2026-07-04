#include <vector>
#include <string>
#include <cstdint>

enum class Cmd {
  Tempo,
  Volume,
  DutyCycle,
  Vibrato,
  TogglePerfectPitch,
  DutyCyclePattern,
  NoteType,
  Octave,
  Note,
  Rest,
  LoopStart,
  LoopEnd,
  Call,
  Ret
};

struct Event {
  Cmd cmd;

  int a = 0;
  int b = 0;
  int c = 0;

  std::string note; // used for note names like F#, C_, etc.

  Event(Cmd c) : cmd(c) {}
  Event(Cmd c, int a) : cmd(c), a(a) {}
  Event(Cmd c, int a, int b) : cmd(c), a(a), b(b) {}
  Event(Cmd c, int a, int b, int c2) : cmd(c), a(a), b(b), c(c2) {}
  Event(Cmd c, const std::string& n) : cmd(c), note(n) {}
};

using Channel = std::vector<Event>;

struct Song {
  Channel ch1;
  Channel ch2;
  Channel ch3;
};

// =======================================================
// Music_MeetProfOak (FULL CONVERSION)
// =======================================================

Song Music_MeetProfOak = {

  // =========================
  // CH1
  // =========================
  {
      {Cmd::Tempo, 112},
      {Cmd::Volume, 7, 7},
      {Cmd::DutyCycle, 3},
      {Cmd::TogglePerfectPitch},
      {Cmd::NoteType, 12, 11, 4},

      {Cmd::Octave, 3},
      {Cmd::Note, "F#"},
      {Cmd::Note, "B_"},
      {Cmd::Octave, 4},
      {Cmd::Note, "D#"},
      {Cmd::Note, "E_"},
      {Cmd::Note, "F#", 12},

      {Cmd::NoteType, 12, 10, 2},
      {Cmd::Octave, 3},

      {Cmd::LoopStart},

      {Cmd::Note, "F#", 6},
      {Cmd::Note, "E_", 4},
      {Cmd::Note, "A_", 2},
      {Cmd::Note, "A_", 4},

      {Cmd::Note, "F#", 6},
      {Cmd::Note, "E_", 4},
      {Cmd::Note, "A_", 2},
      {Cmd::Note, "A_", 4},

      {Cmd::Note, "F#", 6},
      {Cmd::Note, "E_", 4},
      {Cmd::Note, "G#", 2},
      {Cmd::Note, "G#", 4},

      {Cmd::Note, "F#", 6},
      {Cmd::Note, "E_", 4},
      {Cmd::Note, "G#", 2},
      {Cmd::Note, "G#", 4},

      {Cmd::Note, "E_", 6},
      {Cmd::Note, "D#", 4},
      {Cmd::Note, "F#", 2},
      {Cmd::Note, "F#", 4},

      {Cmd::Note, "E_", 6},
      {Cmd::Note, "D#", 4},
      {Cmd::Note, "F#", 2},
      {Cmd::Note, "F#", 4},

      {Cmd::LoopEnd}
  },

  // =========================
  // CH2
  // =========================
  {
      {Cmd::Vibrato, 8, 1, 1},
      {Cmd::DutyCycle, 2},
      {Cmd::NoteType, 12, 12, 4},

      {Cmd::Octave, 3},
      {Cmd::Note, "B_", 1},
      {Cmd::Octave, 4},
      {Cmd::Note, "D#"},
      {Cmd::Note, "F#"},
      {Cmd::Note, "A#"},
      {Cmd::Note, "B_", 12},

      {Cmd::NoteType, 12, 11, 2},
      {Cmd::Octave, 3},

      {Cmd::LoopStart},

      {Cmd::Note, "B_", 2},
      {Cmd::Note, "C#", 2},
      {Cmd::Note, "D#", 2},
      {Cmd::Note, "E_", 4},

      {Cmd::Note, "D#", 2},
      {Cmd::Note, "C#", 4},

      {Cmd::Note, "B_", 2},
      {Cmd::Note, "A_", 2},
      {Cmd::Note, "G#", 2},
      {Cmd::Note, "A_", 4},

      {Cmd::Note, "B_", 2},
      {Cmd::Note, "B_", 4},

      {Cmd::LoopEnd}
  },

  // =========================
  // CH3 (LONG PATTERN TRACK)
  // =========================
  {
      {Cmd::NoteType, 12, 1, 2},

      {Cmd::Rest, 10},

      {Cmd::LoopStart},

      {Cmd::Octave, 4},
      {Cmd::Note, "A_", 1}, {Cmd::Rest, 1},
      {Cmd::Octave, 5},
      {Cmd::Note, "C#", 1}, {Cmd::Rest, 1},

      {Cmd::LoopStart}, // nested pattern blocks (flattened representation)

      {Cmd::Octave, 4},
      {Cmd::Note, "G#", 1}, {Cmd::Rest, 1},
      {Cmd::Note, "B_", 1}, {Cmd::Rest, 1},

      {Cmd::LoopEnd},
      {Cmd::LoopEnd}
  }
};