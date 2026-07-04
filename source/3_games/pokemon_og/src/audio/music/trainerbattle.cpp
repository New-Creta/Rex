#include <vector>
#include <cstdint>

enum class CommandType {
  Note,
  Rest,
  Octave,
  Tempo,
  Volume,
  DutyCycle,
  Vibrato,
  NoteType,
  TogglePerfectPitch,
  Loop
};

struct Event {
  CommandType type;

  // generic parameters (only some used per type)
  int p1 = 0;
  int p2 = 0;
  int p3 = 0;
};

using Channel = std::vector<Event>;

Channel Music_TrainerBattle_Ch1 = {
    {CommandType::Tempo, 112},
    {CommandType::Volume, 7, 7},
    {CommandType::DutyCycle, 3},
    {CommandType::Vibrato, 6, 3, 4},
    {CommandType::TogglePerfectPitch},
    {CommandType::NoteType, 12, 11, 2},
    {CommandType::Rest, 8},
    {CommandType::Octave, 3},

    {CommandType::Note, 5, 1},  // F_
    {CommandType::Note, 4, 1},  // E_
    {CommandType::Note, 5, 1},
    {CommandType::Note, 4, 1},
    {CommandType::Note, 3, 1},  // D#
    {CommandType::Note, 4, 1},
    {CommandType::Note, 3, 1},
    {CommandType::Note, 3, 1},  // D_
    {CommandType::Note, 3, 1},
    {CommandType::Note, 3, 1},
    {CommandType::Note, 2, 1},  // C#
    {CommandType::Note, 3, 1},
    {CommandType::Note, 2, 1},
    {CommandType::Note, 2, 1},  // C_
    {CommandType::Note, 2, 1},
    {CommandType::Note, 2, 1},

    {CommandType::Octave, 2},
    {CommandType::Note, 1, 1},  // B_
    {CommandType::Octave, 3},
    {CommandType::Note, 2, 1},  // C_
    {CommandType::Octave, 2},
    {CommandType::Note, 1, 1},
    {CommandType::Note, 10, 1}, // A#
    {CommandType::Note, 1, 1},
    {CommandType::Note, 10, 1},
    {CommandType::Note, 10, 1},

    {CommandType::Octave, 3},
    {CommandType::Note, 3, 6},
    {CommandType::Note, 4, 6},
    {CommandType::Note, 5, 4},
    {CommandType::Note, 3, 2},
    {CommandType::Note, 4, 4},
    {CommandType::Note, 5, 6},
    {CommandType::Note, 2, 4},
    {CommandType::Note, 3, 6},
    {CommandType::Note, 4, 6},
    {CommandType::Note, 5, 4},
    {CommandType::Note, 3, 2},
    {CommandType::Note, 4, 4},
    {CommandType::Note, 5, 6},
    {CommandType::Note, 2, 2},
    {CommandType::Note, 2, 2},

    {CommandType::Loop, 0} // .mainloop placeholder
};

Channel Music_TrainerBattle_Ch2 = {
    {CommandType::DutyCycle, 3},
    {CommandType::Vibrato, 10, 2, 5},
    {CommandType::NoteType, 12, 12, 2},

    {CommandType::Octave, 4},
    {CommandType::Note, 9, 1},
    {CommandType::Note, 8, 1},
    {CommandType::Note, 7, 1},
    {CommandType::Note, 6, 1},
    {CommandType::Note, 9, 1},
    {CommandType::Note, 5, 1},
    {CommandType::Note, 6, 1},
    {CommandType::Note, 5, 1},

    {CommandType::Loop, 0}
};

Channel Music_TrainerBattle_Ch3 = {
    {CommandType::Vibrato, 0, 2, 0},
    {CommandType::NoteType, 12, 1, 4},

    {CommandType::Octave, 3},
    {CommandType::Note, 1, 1},
    {CommandType::Note, 10, 1},
    {CommandType::Note, 9, 1},
    {CommandType::Note, 8, 1},
    {CommandType::Note, 9, 1},
    {CommandType::Note, 8, 1},
    {CommandType::Note, 7, 1},
    {CommandType::Note, 6, 1},

    {CommandType::Loop, 0}
};