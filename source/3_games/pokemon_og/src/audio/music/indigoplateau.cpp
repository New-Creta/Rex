#include <vector>
#include <cstdint>

enum class CmdType {
  Note,
  Rest,
  Octave,
  Tempo,
  Volume,
  DutyCycle,
  Vibrato,
  NoteType,
  DrumNote,
  Call,
  Return,
  LoopStart,
  LoopEnd
};

struct Event {
  CmdType type;

  // Generic payload
  int a = 0;
  int b = 0;
  int c = 0;
};

// -----------------------------
// Channel 1
// -----------------------------
namespace IndigoPlateau_Ch1 {

  static const std::vector<Event> sub1 = {
      {CmdType::NoteType, 12, 11, 2},
      {CmdType::Octave, 2},

      {CmdType::Note, /*A_*/ 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4}
  };

  static const std::vector<Event> mainloop = {
      {CmdType::Call, 0}, // sub1 placeholder

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Octave, 3},
      {CmdType::Note, 2, 4}, // D_

      {CmdType::Call, 0},

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Octave, 3},
      {CmdType::Note, 3, 4}, // D#

      {CmdType::Call, 0},

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Octave, 3},
      {CmdType::Note, 2, 4}, // D_

      {CmdType::Call, 0},

      {CmdType::NoteType, 12, 10, 0},
      {CmdType::Octave, 2},
      {CmdType::Note, 3, 4}, // A#

      {CmdType::Call, 0},

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Octave, 3},
      {CmdType::Note, 2, 4},

      {CmdType::NoteType, 12, 13, 4},
      {CmdType::Note, 9, 4}, // A_

      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},
      {CmdType::Note, 9, 4},

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Note, 3, 4},

      {CmdType::NoteType, 12, 13, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},
      {CmdType::Note, 3, 4},

      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Note, 5, 2},
      {CmdType::NoteType, 12, 11, 4},
      {CmdType::Note, 6, 2},

      {CmdType::NoteType, 12, 11, 0},
      {CmdType::Note, 9, 8},
      {CmdType::Octave, 2},
      {CmdType::Note, 9, 8},

      {CmdType::NoteType, 12, 11, 7},
      {CmdType::Octave, 3},
      {CmdType::Note, 5, 8},

      {CmdType::NoteType, 12, 4, -6},
      {CmdType::Octave, 2},
      {CmdType::Note, 3, 8}
  };

}

// -----------------------------
// Channel 2
// -----------------------------
namespace IndigoPlateau_Ch2 {

  static const std::vector<Event> sub1 = {
      {CmdType::NoteType, 12, 12, 2},
      {CmdType::Octave, 3},

      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4},
      {CmdType::Note, 2, 4}
  };

}

// -----------------------------
// Channel 3
// -----------------------------
namespace IndigoPlateau_Ch3 {

  static const std::vector<Event> sub1 = {
      {CmdType::Octave, 4},
      {CmdType::Note, 2, 2},
      {CmdType::Rest, 2},
      {CmdType::Note, 2, 2},
      {CmdType::Rest, 2},
      {CmdType::Note, 2, 2},
      {CmdType::Rest, 2},
      {CmdType::Note, 2, 2},
      {CmdType::Rest, 2}
  };

}

// -----------------------------
// Channel 4 (Drums)
// -----------------------------
namespace IndigoPlateau_Ch4 {

  static const std::vector<Event> sub1 = {
      {CmdType::DrumNote, 17, 4},
      {CmdType::DrumNote, 18, 4},
      {CmdType::DrumNote, 19, 4},
      {CmdType::DrumNote, 18, 4},
      {CmdType::DrumNote, 17, 4},
      {CmdType::DrumNote, 18, 4},
      {CmdType::DrumNote, 19, 4}
  };

  static const std::vector<Event> sub2 = sub1;
  static const std::vector<Event> sub3 = sub1;

  static const std::vector<Event> mainloop = {
      {CmdType::Call, 2},
      {CmdType::Call, 1},
      {CmdType::Call, 2},
      {CmdType::Call, 3}
  };

}