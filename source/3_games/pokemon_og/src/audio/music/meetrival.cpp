// music_intro_battle.cpp
// Structured C++ representation of the assembly-style music data

#include <vector>
#include <cstdint>
#include <string>

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
  DrumSpeed,
  DrumNote,
  Loop,
  Call,
  Return,
  End
};

enum class NoteName {
  C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B
};

struct Event {
  CmdType type;

  // Generic parameters (simple but flexible)
  int a = 0;
  int b = 0;
  int c = 0;

  NoteName note;
};

struct Channel {
  std::string name;
  std::vector<Event> events;
};

// ---------------------- CHANNEL 1 ----------------------

Channel Music_IntroBattle_Ch1 = {
    "Ch1",
    {
        {CmdType::Tempo, 98},
        {CmdType::Volume, 7, 7},
        {CmdType::DutyCycle, 3},
        {CmdType::Vibrato, 6, 3, 4},
        {CmdType::TogglePerfectPitch},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Rest, 8},

        {CmdType::Octave, 2},

        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 11, 4},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 4, 0, NoteName::D},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 11, 4},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 4, 0, NoteName::Ds},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 11, 4},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 4, 0, NoteName::D},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 10, 0},
        {CmdType::Note, 0, 4, 0, NoteName::As},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 11, 4},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 4, 0, NoteName::D},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 2, 0, NoteName::A},
        {CmdType::Note, 0, 2, 0, NoteName::A},

        {CmdType::NoteType, 12, 2, -1},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 4, 0, NoteName::G},

        {CmdType::NoteType, 12, 11, 0},
        {CmdType::Note, 0, 8, 0, NoteName::A},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 8, 0, NoteName::A},

        {CmdType::NoteType, 12, 11, 7},
        {CmdType::Octave, 3},
        {CmdType::Note, 0, 8, 0, NoteName::F},

        {CmdType::NoteType, 12, 4, -7},
        {CmdType::Octave, 2},
        {CmdType::Note, 0, 8, 0, NoteName::F},

        {CmdType::NoteType, 12, 11, 1},
        {CmdType::Octave, 4},
        {CmdType::Note, 0, 16, 0, NoteName::D},

        {CmdType::End}
    }
};

// ---------------------- CHANNEL 2 ----------------------

Channel Music_IntroBattle_Ch2 = {
    "Ch2",
    {
        {CmdType::DutyCycle, 3},
        {CmdType::Vibrato, 8, 2, 5},

        {CmdType::NoteType, 12, 12, 2},
        {CmdType::Rest, 8},

        {CmdType::Octave, 3},
        {CmdType::Note, 0, 2, 0, NoteName::D},
        {CmdType::Note, 0, 2, 0, NoteName::D},

        {CmdType::NoteType, 12, 12, 5},
        {CmdType::Note, 0, 4, 0, NoteName::A},

        {CmdType::NoteType, 12, 12, 2},
        {CmdType::Note, 0, 2, 0, NoteName::D},
        {CmdType::Note, 0, 2, 0, NoteName::D},

        {CmdType::NoteType, 12, 12, 5},
        {CmdType::Note, 0, 4, 0, NoteName::As},

        {CmdType::End}
    }
};

// ---------------------- CHANNEL 3 ----------------------

Channel Music_IntroBattle_Ch3 = {
    "Ch3",
    {
        {CmdType::NoteType, 12, 1, 0},
        {CmdType::Rest, 8},

        {CmdType::Octave, 4},
        {CmdType::Note, 0, 1, 0, NoteName::D},
        {CmdType::Rest, 1},

        {CmdType::Note, 0, 4, 0, NoteName::A},
        {CmdType::Note, 0, 4, 0, NoteName::D},

        {CmdType::End}
    }
};

// ---------------------- CHANNEL 4 ----------------------

Channel Music_IntroBattle_Ch4 = {
    "Ch4",
    {
        {CmdType::DrumSpeed, 6},

        {CmdType::DrumNote, 19, 1},
        {CmdType::DrumNote, 18, 1},
        {CmdType::DrumNote, 17, 4},

        {CmdType::DrumNote, 17, 8},

        {CmdType::End}
    }
};