#include <vector>
#include <string>
#include <cstdint>

enum class CmdType {
  Tempo,
  Volume,
  DutyCycle,
  Vibrato,
  TogglePerfectPitch,
  NoteType,
  Octave,
  Note,
  Rest,
  LoopBegin,
  LoopEnd
};

struct Event {
  CmdType type;

  // Generic parameters (interpretation depends on type)
  int a = 0;
  int b = 0;
  int c = 0;

  std::string note;   // for NOTE names (C#, D_, etc.)
};

using Channel = std::vector<Event>;

struct Song {
  Channel ch1;
  Channel ch2;
  Channel ch3;
};

// ---------------------------
// Music_MeetMaleTrainer
// ---------------------------

Song Music_MeetMaleTrainer = {

  // =========================
  // CH1
  // =========================
  {
      {CmdType::Tempo, 112},
      {CmdType::Volume, 7, 7},
      {CmdType::DutyCycle, 3},
      {CmdType::Vibrato, 20, 3, 3},
      {CmdType::TogglePerfectPitch},

      {CmdType::NoteType, 12, 11, 4},

      {CmdType::Octave, 3},
      {CmdType::Note, 0, 0, 0, "C#"},
      {CmdType::Note, 0, 0, 0, "D_"},
      {CmdType::Note, 0, 0, 0, "D#"},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Note, 0, 0, 0, "F_"},
      {CmdType::Rest, 16},

      {CmdType::LoopBegin},

      {CmdType::Octave, 3},
      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Note, 0, 0, 0, "A_"},
      {CmdType::Note, 0, 0, 0, "G#"},
      {CmdType::Note, 0, 0, 0, "F#"},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Note, 0, 0, 0, "D#"},
      {CmdType::Note, 0, 0, 0, "F#"},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Note, 0, 0, 0, "F_"},
      {CmdType::Note, 0, 0, 0, "F#"},
      {CmdType::Note, 0, 0, 0, "G_"},
      {CmdType::Octave, 4},
      {CmdType::Note, 0, 0, 0, "D_"},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Rest, 16},

      {CmdType::LoopEnd}
  },

  // =========================
  // CH2
  // =========================
  {
      {CmdType::DutyCycle, 3},
      {CmdType::Vibrato, 24, 2, 2},

      {CmdType::NoteType, 12, 12, 4},

      {CmdType::Octave, 4},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Note, 0, 0, 0, "D#"},
      {CmdType::Note, 0, 0, 0, "D_"},
      {CmdType::Note, 0, 0, 0, "C#"},
      {CmdType::Octave, 3},
      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Rest, 12},

      {CmdType::Rest, 2},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Rest, 3},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Rest, 9}
  },

  // =========================
  // CH3
  // =========================
  {
      {CmdType::NoteType, 12, 1, 0},

      {CmdType::Rest, 6},

      {CmdType::Octave, 4},
      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Rest, 1},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Rest, 1},
      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Rest, 3},

      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Rest, 1},
      {CmdType::Note, 0, 0, 0, "E_"},
      {CmdType::Rest, 1},
      {CmdType::Note, 0, 0, 0, "B_"},
      {CmdType::Rest, 3},

      {CmdType::Note, 0, 0, 0, "F#"},
      {CmdType::Rest, 1},
      {CmdType::Note, 0, 0, 0, "F_"},
      {CmdType::Rest, 1}
  }
};
