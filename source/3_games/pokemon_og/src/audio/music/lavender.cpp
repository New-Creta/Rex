// LavenderTown.cpp
// Structured C++ conversion of RGBDS-style music macros

#include <vector>
#include <string>
#include <cstdint>

enum class Cmd {
  Tempo,
  Volume,
  DutyCycle,
  TogglePerfectPitch,
  Vibrato,
  NoteType,
  Octave,
  Note,
  Rest,
  DrumSpeed,
  DrumNote,
  LoopStart,
  LoopEnd
};

struct Event {
  Cmd cmd;

  int a = 0;
  int b = 0;
  int c = 0;

  std::string s;   // note or label
};

struct Track {
  std::string name;
  std::vector<Event> events;
};

// ---------------- helpers ----------------

Event Tempo(int v) { return { Cmd::Tempo, v }; }
Event Volume(int l, int r) { return { Cmd::Volume, l, r }; }
Event Duty(int v) { return { Cmd::DutyCycle, v }; }
Event TogglePerfectPitch() { return { Cmd::TogglePerfectPitch }; }
Event Vibrato(int a, int b, int c) { return { Cmd::Vibrato,a,b,c }; }
Event NoteType(int a, int b, int c) { return { Cmd::NoteType,a,b,c }; }
Event Oct(int v) { return { Cmd::Octave,v }; }

Event Note(std::string n, int len) {
  return { Cmd::Note,len,0,0,n };
}

Event Rest(int len) {
  return { Cmd::Rest,len };
}

Event DrumSpeed(int v) { return { Cmd::DrumSpeed,v }; }
Event Drum(int id, int len) { return { Cmd::DrumNote,id,len }; }

Event LoopStart(std::string label) {
  return { Cmd::LoopStart,0,0,0,label };
}
Event LoopEnd(std::string label) {
  return { Cmd::LoopEnd,0,0,0,label };
}

// ---------------- LAVENDER TOWN ----------------

Track Music_Lavender_Ch1 = {
    "Ch1",
    {
        Tempo(152),
        Volume(7,7),
        Duty(1),
        TogglePerfectPitch(),
        Vibrato(0,8,8),
        NoteType(12,8,7),

        Rest(16), Rest(16), Rest(16), Rest(16),
        NoteType(12,10,7),

        LoopStart(".mainloop"),

        Oct(3),

        Note("G_",8), Note("G_",8),
        Note("E_",8), Note("E_",8),
        Note("G_",4), Note("F#",4),
        Note("E_",4), Note("B_",4),
        Note("C#",8), Note("C#",8),
        Note("G_",8), Note("G_",8),
        Note("F#",8), Note("F#",8),
        Note("B_",4), Note("G_",4),
        Note("F#",4), Note("B_",4),

        Oct(4),
        Note("C_",8), Note("C_",8),

        Oct(3),
        Note("G_",8), Note("G_",8),
        Note("E_",8), Note("E_",8),
        Note("G_",4), Note("F#",4),
        Note("E_",4), Note("B_",4),
        Note("C#",8), Note("C#",8),
        Note("G_",8), Note("G_",8),
        Note("F#",8), Note("F#",8),
        Note("B_",4), Note("G_",4),
        Note("F#",4), Note("B_",4),

        Note("C_",8), Note("C_",8),

        Rest(16), Rest(16), Rest(16), Rest(16),

        LoopEnd(".mainloop")
    }
};

Track Music_Lavender_Ch2 = {
    "Ch2",
    {
        Vibrato(0,3,4),
        Duty(3),
        NoteType(12,9,1),

        LoopStart(".mainloop"),

        Oct(5),
        Note("C_",4),
        Note("G_",4),
        Note("B_",4),
        Note("F#",4),

        LoopEnd(".mainloop")
    }
};

Track Music_Lavender_Ch3 = {
    "Ch3",
    {
        Vibrato(4,1,1),
        NoteType(12,3,5),

        Rest(16), Rest(16), Rest(16), Rest(16),

        NoteType(12,2,5),

        LoopStart(".mainloop"),

        Oct(4),
        Note("E_",16), Note("D_",16), Note("C_",16),
        Note("E_",4),  Note("C_",4),

        Oct(3),
        Note("B_",4),

        Oct(4),
        Note("E_",4),

        Note("E_",16), Note("D_",16), Note("C_",16),
        Note("E_",4),  Note("C_",4),

        Oct(3),
        Note("B_",4),

        Oct(4),
        Note("E_",4),

        Note("E_",16), Note("D_",16), Note("C_",16),
        Note("E_",4),  Note("C_",4),

        Oct(3),
        Note("B_",4),

        Oct(4),
        Note("E_",4),

        // high motif
        NoteType(12,3,5),
        Oct(6),
        Note("B_",4), Note("G_",4), Note("F#",4), Note("B_",4),

        NoteType(12,2,5),
        Oct(6),
        Note("B_",4), Note("G_",4), Note("F#",4), Note("B_",4),

        Oct(7),
        Note("B_",4), Note("G_",4), Note("F#",4), Note("B_",4),

        Oct(4),
        Note("E_",4), Note("G_",4), Note("F#",4), Note("B_",4),

        LoopEnd(".mainloop")
    }
};

Track Music_Lavender_Ch4 = {
    "Ch4",
    {
        DrumSpeed(12),

        Rest(16), Rest(16), Rest(16), Rest(16),

        LoopStart(".mainloop"),

        Drum(7,8),
        Drum(7,8),

        LoopEnd(".mainloop")
    }
};