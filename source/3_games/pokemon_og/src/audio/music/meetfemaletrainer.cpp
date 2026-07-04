// MeetFemaleTrainer.cpp
// Structured IR conversion (loop + call aware)

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
  LoopEnd,
  LoopRepeat,

  Call,
  Return
};

struct Event {
  Cmd cmd;

  int a = 0;
  int b = 0;
  int c = 0;

  std::string s; // note or label
};

// ---------------- helper constructors ----------------

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

Event LoopStart(std::string l) {
  return { Cmd::LoopStart,0,0,0,l };
}

Event LoopEnd(std::string l) {
  return { Cmd::LoopEnd,0,0,0,l };
}

Event LoopRepeat(int count, std::string label) {
  Event e{ Cmd::LoopRepeat };
  e.a = count;
  e.s = label;
  return e;
}

Event Call(std::string sub) {
  return { Cmd::Call,0,0,0,sub };
}

Event Ret() {
  return { Cmd::Return };
}

// ---------------- SONG ----------------

struct Track {
  std::string name;
  std::vector<Event> events;
};

// ---------------- CH1 ----------------

Track Music_MeetFemaleTrainer_Ch1 = {
    "Ch1",
    {
        Tempo(124),
        Volume(7,7),
        Duty(1),
        Toggle(),
        NoteType(12,11,2),

        Oct(3),
        Note("G#",6),

        Oct(4),
        Note("E_",2),
        Note("D#",2),
        Note("C#",2),
        Note("C_",2),

        NoteType(12,8,1),

        LoopStart(".mainloop"),

        LoopStart(".loop1"),
        Oct(3),
        Note("E_",4),
        LoopRepeat(12, ".loop1"),

        Note("E_",4),
        Oct(2),
        Note("B_",4),
        Note("B_",4),

        Oct(3),
        Note("E_",4),

        LoopEnd(".mainloop")
    }
};

// ---------------- CH2 ----------------

Track Music_MeetFemaleTrainer_Ch2 = {
    "Ch2",
    {
        Duty(2),

        NoteType(12,12,2),
        Oct(3),
        Note("B_",2),

        NoteType(12,12,7),
        Oct(4),
        Note("B_",12),

        LoopStart(".mainloop"),

        NoteType(12,12,2),

        Oct(3),
        Note("B_",4),
        Oct(4), Note("D#",4), Note("E_",4), Note("D#",4),

        Note("C#",2), Note("C_",2),
        Oct(3), Note("B_",2),
        Note("A_",2),
        Note("G#",2),
        Note("A_",2),
        Note("A#",2),

        Oct(4),
        Note("C#",2),

        Oct(3),
        Note("B_",4),

        Oct(4),
        Note("C#",4),

        Oct(3),
        Note("B_",4),
        Note("A_",4),

        Note("G#",2),
        Note("F#",2),
        Note("E_",2),
        Note("D#",2),
        Note("E_",2),
        Note("F#",2),
        Note("G#",2),
        Note("A_",2),

        LoopRepeat(0, ".mainloop")
    }
};

// ---------------- CH3 ----------------

Track Music_MeetFemaleTrainer_Ch3 = {
    "Ch3",
    {
        NoteType(12,1,0),

        Rest(8),

        Oct(5),
        Note("C#",1),
        Rest(1),

        Oct(4),
        Note("B_",1),
        Rest(1),
        Note("A_",1),
        Rest(1),

        LoopStart(".mainloop"),

        Call(".sub1"),

        Note("G#",1), Rest(3),
        Note("E_",1), Rest(3),
        Note("G#",1), Rest(3),
        Note("E_",1), Rest(3),

        Call(".sub1"),

        Note("G#",1), Rest(3),
        Note("E_",1), Rest(3),
        Note("G#",1), Rest(3),
        Note("B_",1), Rest(3),

        LoopRepeat(0, ".mainloop"),

        // -------- subroutine --------
        Call(".sub1"),
        Ret()
    }
};

// ---------------- SUBROUTINE (logical only) ----------------

std::vector<Event> Sub1 = {
    Note("G#",1), Rest(3),
    Note("E_",1), Rest(3),
    Note("G#",1), Rest(3),
    Note("E_",1), Rest(1),
    Note("F#",1), Rest(1),
    Ret()
};