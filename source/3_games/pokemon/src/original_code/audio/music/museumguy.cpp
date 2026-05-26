// museum_guy_music.cpp

#include <vector>
#include <variant>
#include <string>

enum class Note {
  C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B
};

enum class CommandType {
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
  Call,
  LoopStart,
  LoopEnd,
  Return,
  End
};

struct NoteEvent {
  Note note;
  int length;
};

struct DrumEvent {
  int id;
  int length;
};

struct Command {
  CommandType type;

  int a = 0;
  int b = 0;
  int c = 0;

  Note note{};
};

using Event = std::variant<Command, NoteEvent, DrumEvent>;

struct Pattern {
  std::string name;
  std::vector<Event> events;
};

struct Channel {
  std::string name;
  std::vector<Event> events;
};

// ------------------------------------------------------
// SUBROUTINES (from .sub1 / .sub2)
// ------------------------------------------------------

Pattern Museum_Sub1 = {
    ".sub1",
    {
        NoteEvent{Note::E, 2},
        NoteEvent{Note::E, 6},
        NoteEvent{Note::F, 2},
        NoteEvent{Note::F, 6}
    }
};

Pattern Museum_Sub2 = {
    ".sub2",
    {
        NoteEvent{Note::E, 2},
        NoteEvent{Note::E, 6},
        NoteEvent{Note::D, 2},
        NoteEvent{Note::D, 6}
    }
};

// ------------------------------------------------------
// CH1
// ------------------------------------------------------

Channel Music_MuseumGuy_Ch1 = {
    "Ch1",
    {
        {Command{CommandType::Tempo, 128}},
        {Command{CommandType::Volume, 7, 7}},
        {Command{CommandType::DutyCycle, 2}},
        {Command{CommandType::TogglePerfectPitch}},

        {Command{CommandType::Octave, 3}},

        NoteEvent{Note::B, 2},
        NoteEvent{Note::A, 2},
        NoteEvent{Note::Gs, 2},
        NoteEvent{Note::A, 2},
        NoteEvent{Note::Gs, 2},
        NoteEvent{Note::Fs, 2},
        NoteEvent{Note::E, 2},
        NoteEvent{Note::Ds, 2},

        {Command{CommandType::Octave, 2}},
        NoteEvent{Note::B, 4},

        {Command{CommandType::Call, 0}}, // sub1
        {Command{CommandType::Call, 1}}, // sub2
        {Command{CommandType::Call, 0}}, // sub1

        NoteEvent{Note::E, 4},
        NoteEvent{Note::E, 4},
        NoteEvent{Note::E, 2},
        NoteEvent{Note::E, 6},

        {Command{CommandType::LoopStart}}
        // main loop repeats same pattern conceptually
    }
};

// ------------------------------------------------------
// CH2
// ------------------------------------------------------

Channel Music_MuseumGuy_Ch2 = {
    "Ch2",
    {
        {Command{CommandType::DutyCycle, 2}},

        {Command{CommandType::Octave, 4}},
        NoteEvent{Note::E, 2},
        NoteEvent{Note::D, 2},
        NoteEvent{Note::Cs, 2},
        NoteEvent{Note::D, 2},

        NoteEvent{Note::Cs, 2},
        {Command{CommandType::Octave, 3}},
        NoteEvent{Note::B, 2},
        NoteEvent{Note::A, 2},
        NoteEvent{Note::Gs, 2},

        NoteEvent{Note::E, 4},

        {Command{CommandType::LoopStart}}
    }
};

// ------------------------------------------------------
// CH3
// ------------------------------------------------------

Channel Music_MuseumGuy_Ch3 = {
    "Ch3",
    {
        {Command{CommandType::Octave, 4}},

        {Command{CommandType::Rest, 16}},

        {Command{CommandType::Call, 0}}, // sub1

        {Command{CommandType::LoopStart}}
    }
};

// ------------------------------------------------------
// CH4 (Drums)
// ------------------------------------------------------

Channel Music_MuseumGuy_Ch4 = {
    "Ch4",
    {
        {Command{CommandType::DrumSpeed, 12}},

        DrumEvent{17, 4},
        DrumEvent{17, 6},
        DrumEvent{17, 2},

        DrumEvent{19, 1},
        DrumEvent{18, 1},

        {Command{CommandType::LoopStart}}
    }
};