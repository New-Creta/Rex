// oaks_lab_music.cpp

#include <vector>
#include <variant>
#include <string>

enum class Note {
  C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B
};

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
  LoopStart,
  LoopEnd,
  End
};

struct Command {
  CmdType type;
  int a = 0, b = 0, c = 0;
};

struct NoteEvent {
  Note note;
  int length;
};

using Event = std::variant<Command, NoteEvent>;

struct Channel {
  std::string name;
  std::vector<Event> events;
};

// ------------------------------------------------------
// CH1 - Melody
// ------------------------------------------------------

Channel Music_OaksLab_Ch1 = {
    "Ch1",
    {
        Command{CmdType::Tempo, 140},
        Command{CmdType::Volume, 7, 7},
        Command{CmdType::DutyCycle, 3},
        Command{CmdType::Vibrato, 16, 1, 2},
        Command{CmdType::TogglePerfectPitch},

        Command{CmdType::Octave, 2},
        NoteEvent{Note::B, 1},

        Command{CmdType::Octave, 3},
        NoteEvent{Note::Cs, 1},
        NoteEvent{Note::D, 1},
        NoteEvent{Note::E, 1},
        NoteEvent{Note::Fs, 1},
        NoteEvent{Note::Gs, 1},
        NoteEvent{Note::A, 1},
        NoteEvent{Note::B, 1},

        // MAIN LOOP
        Command{CmdType::LoopStart},

        Command{CmdType::Octave, 4},
        NoteEvent{Note::Cs, 4},

        Command{CmdType::Octave, 3},
        NoteEvent{Note::B, 2},
        NoteEvent{Note::Cs, 2},
        NoteEvent{Note::A, 4},
        NoteEvent{Note::Gs, 2},
        NoteEvent{Note::A, 2},

        NoteEvent{Note::B, 1},
        NoteEvent{Note::A, 6},
        NoteEvent{Note::Gs, 1},
        NoteEvent{Note::B, 1},

        Command{CmdType::Octave, 4},
        NoteEvent{Note::Cs, 4},

        Command{CmdType::Octave, 3},
        NoteEvent{Note::A, 4},
        NoteEvent{Note::Gs, 2},
        NoteEvent{Note::A, 2},
        NoteEvent{Note::Fs, 4},
        NoteEvent{Note::E, 2},
        NoteEvent{Note::Fs, 2},

        NoteEvent{Note::Gs, 1},
        NoteEvent{Note::Fs, 8},
        NoteEvent{Note::E, 4},
        NoteEvent{Note::A, 6},

        Command{CmdType::Octave, 4},
        NoteEvent{Note::D, 8},
        NoteEvent{Note::Cs, 6},

        Command{CmdType::LoopStart},

        Command{CmdType::End}
    }
};

// ------------------------------------------------------
// CH2 - Harmony
// ------------------------------------------------------

Channel Music_OaksLab_Ch2 = {
    "Ch2",
    {
        Command{CmdType::DutyCycle, 3},
        Command{CmdType::Vibrato, 10, 2, 5},

        Command{CmdType::Octave, 3},
        NoteEvent{Note::Gs, 1},
        NoteEvent{Note::A, 1},
        NoteEvent{Note::B, 1},

        Command{CmdType::Octave, 4},
        NoteEvent{Note::Cs, 1},
        NoteEvent{Note::D, 1},
        NoteEvent{Note::E, 1},
        NoteEvent{Note::Fs, 1},
        NoteEvent{Note::Gs, 1},

        Command{CmdType::LoopStart},

        NoteEvent{Note::A, 6},
        NoteEvent{Note::Gs, 1},
        NoteEvent{Note::Fs, 1},
        NoteEvent{Note::E, 6},

        NoteEvent{Note::Ds, 1},
        NoteEvent{Note::E, 1},
        NoteEvent{Note::Fs, 1},

        NoteEvent{Note::E, 8},
        NoteEvent{Note::Fs, 6},

        Command{CmdType::End}
    }
};

// ------------------------------------------------------
// CH3 - Bass / Wave
// ------------------------------------------------------

Channel Music_OaksLab_Ch3 = {
    "Ch3",
    {
        Command{CmdType::Octave, 4},
        Command{CmdType::Rest, 2},

        Command{CmdType::LoopStart},

        NoteEvent{Note::Cs, 1},
        NoteEvent{Note::A, 1},
        NoteEvent{Note::E, 1},

        NoteEvent{Note::A, 1},
        NoteEvent{Note::Fs, 1},
        NoteEvent{Note::E, 1},

        NoteEvent{Note::Cs, 1},
        NoteEvent{Note::A, 1},

        Command{CmdType::LoopStart},

        Command{CmdType::End}
    }
};