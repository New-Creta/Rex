// JigglypuffSong.cpp
// Data-driven conversion of music macro language into C++ structures

#include <vector>
#include <string>
#include <cstdint>

enum class CommandType {
  Tempo,
  Volume,
  Vibrato,
  DutyCycle,
  DutyCyclePattern,
  TogglePerfectPitch,
  NoteType,
  Octave,
  Note,
  Rest,
  SoundRet
};

struct Event {
  CommandType type;

  // generic numeric parameters
  int a = 0;
  int b = 0;
  int c = 0;

  // note name if needed
  std::string note;
};

struct Channel {
  std::string name;
  std::vector<Event> events;
};

// helper constructors
Event Tempo(int t) { return { CommandType::Tempo, t }; }
Event Volume(int l, int r) { return { CommandType::Volume, l, r }; }
Event Vibrato(int a, int b, int c) { return { CommandType::Vibrato, a, b, c }; }
Event DutyCycle(int v) { return { CommandType::DutyCycle, v }; }
Event DutyCyclePattern(int a, int b, int c, int d) { return { CommandType::DutyCyclePattern, a, b, c }; }
Event TogglePerfectPitch() { return { CommandType::TogglePerfectPitch }; }
Event NoteType(int a, int b, int c) { return { CommandType::NoteType, a, b, c }; }
Event Octave(int o) { return { CommandType::Octave, o }; }
Event Note(std::string n, int len) {
  Event e;
  e.type = CommandType::Note;
  e.note = n;
  e.a = len;
  return e;
}
Event Rest(int len) {
  Event e;
  e.type = CommandType::Rest;
  e.a = len;
  return e;
}
Event SoundRet() { return { CommandType::SoundRet }; }

// -------------------- SONG --------------------

Channel Music_JigglypuffSong_Ch1 = {
    "Ch1",
    {
        Tempo(144),
        Volume(7, 7),
        Vibrato(8, 2, 4),
        DutyCycle(2),
        DutyCyclePattern(2, 2, 1, 1),
        TogglePerfectPitch(),
        NoteType(13, 6, 7),
        Octave(4),
        Note("E_", 8),

        NoteType(12, 6, 7),
        Note("B_", 2),
        Note("G#", 6),
        Note("F#", 8),
        Note("G#", 2),
        Note("A_", 6),
        Note("G#", 8),
        Note("F#", 4),
        Note("G#", 4),
        Note("E_", 10),

        SoundRet()
    }
};

Channel Music_JigglypuffSong_Ch2 = {
    "Ch2",
    {
        Vibrato(5, 1, 5),
        DutyCycle(2),
        DutyCyclePattern(0, 0, 2, 2),

        NoteType(12, 10, 7),
        Octave(4),
        Note("E_", 8),
        Note("B_", 2),
        Note("G#", 6),
        Note("F#", 8),
        Note("G#", 2),
        Note("A_", 6),
        Note("G#", 8),
        Note("F#", 4),
        Note("G#", 4),
        Note("E_", 10),

        SoundRet()
    }
};

// optional container for playback system
struct Song {
  std::vector<Channel> channels;
};

Song JigglypuffSong = {
    { Music_JigglypuffSong_Ch1, Music_JigglypuffSong_Ch2 }
};