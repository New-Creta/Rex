// MeetEvilTrainer.cpp

#include <vector>
#include <string>

enum class Cmd {
  Tempo,
  Volume,
  DutyCycle,
  TogglePerfectPitch,
  NoteType,
  Octave,
  Note,
  Rest,
  LoopStart,
  LoopEnd
};

struct Event {
  Cmd cmd;

  int a = 0;
  int b = 0;
  int c = 0;

  std::string s;
};

struct Track {
  std::string name;
  std::vector<Event> events;
};

// ---------------- helpers ----------------

Event Tempo(int v) { return { Cmd::Tempo, v }; }
Event Volume(int l, int r) { return { Cmd::Volume,l,r }; }
Event Duty(int v) { return { Cmd::DutyCycle,v }; }
Event Toggle() { return { Cmd::TogglePerfectPitch }; }
Event NoteType(int a, int b, int c) { return { Cmd::NoteType,a,b,c }; }
Event Oct(int v) { return { Cmd::Octave,v }; }

Event Note(std::string n, int len) {
  return { Cmd::Note,len,0,0,n };
}

Event Rest(int len) {
  return { Cmd::Rest,len };
}

Event LoopStart(std::string label) {
  return { Cmd::LoopStart,0,0,0,label };
}

Event LoopEnd(std::string label) {
  return { Cmd::LoopEnd,0,0,0,label };
}

// ---------------- SONG ----------------

Track Music_MeetEvilTrainer_Ch1 = {
    "Ch1",
    {
        Tempo(124),
        Volume(7,7),
        Duty(2),
        Toggle(),
        NoteType(12,11,1),

        Rest(4),
        Oct(3),

        Note("D_",2),
        Note("C#",2),
        NoteType(12,4,-7),
        Note("D_",4),

        LoopStart(".mainloop"),

        NoteType(12,10,1),
        Note("D_",4),
        Note("D_",4),
        Note("D_",4),

        NoteType(12,7,0),
        Note("D_",4),

        LoopEnd(".mainloop")
    }
};

Track Music_MeetEvilTrainer_Ch2 = {
    "Ch2",
    {
        Duty(1),
        NoteType(12,11,6),

        Oct(3),
        Note("B_",2),
        Note("A#",2),
        Note("B_",8),

        LoopStart(".mainloop"),

        NoteType(12,12,2),

        Oct(4),
        Note("D#",2),
        Note("D_",2),
        Note("C#",2),
        Note("C_",2),

        Oct(3),
        Note("B_",4), Note("B_",4), Note("B_",4), Note("B_",4), Note("B_",4),

        NoteType(12,4,-7),
        Note("A#",4),

        NoteType(12,12,2),
        Note("G_",2),
        Note("G#",2),
        Note("A_",2),
        Note("A#",2),

        Note("B_",4), Note("B_",4), Note("B_",4), Note("B_",4), Note("B_",4),

        NoteType(12,3,-7),
        Note("A#",4),

        NoteType(12,12,2),

        LoopEnd(".mainloop")
    }
};

Track Music_MeetEvilTrainer_Ch3 = {
    "Ch3",
    {
        NoteType(12,1,0),

        Rest(8),

        Oct(4),
        Note("F#",1),
        Rest(1),
        Note("F_",1),
        Rest(1),

        LoopStart(".mainloop"),

        Note("F#",1), Rest(3),
        Note("F#",1), Rest(3),
        Note("F#",1), Rest(3),

        Note("A#",4),

        LoopEnd(".mainloop")
    }
};