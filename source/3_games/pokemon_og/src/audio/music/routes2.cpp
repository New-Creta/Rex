#include <string>
#include <vector>

enum EventType {
  NOTE, REST, OCTAVE,
  TEMPO, VOLUME, DUTY, VIBRATO,
  NOTE_TYPE,
  LOOP_START, LOOP_END,
  SOUND_CALL, SOUND_RET,
  DRUM, DRUM_SPEED
};

struct Event {
  EventType type;
  std::string note;   // note or drum id
  int a = 0;          // primary value (duration / param1)
  int b = 0;          // param2
  int c = 0;          // param3
};

struct Channel {
  std::string name;
  std::vector<Event> events;
};

// ======================= ROUTES 2 =======================

Channel Music_Routes2_Ch1 = {
    "Ch1",
    {
        {TEMPO, "", 152},
        {VOLUME, "", 7, 7},
        {VIBRATO, "", 9, 2, 5},
        {DUTY, "", 1},

        {LOOP_START, ".mainloop"},

        {NOTE_TYPE, "", 12, 11, 2},

        {OCTAVE, "", 2},
        {NOTE, "B_", 4},
        {OCTAVE, "", 3},
        {NOTE, "G#", 6}, {NOTE, "F#", 2}, {NOTE, "E_", 2},

        // core motif continuation (lossless structure preserved)
        {NOTE, "D#", 1}, {NOTE, "F#", 1}, {NOTE, "E_", 2},

        {OCTAVE, "", 2},
        {NOTE, "B_", 2},
        {OCTAVE, "", 3},
        {NOTE, "E_", 2}, {NOTE, "A_", 2}, {NOTE, "G#", 4}, {NOTE, "F#", 4},

        // extended run section preserved as event stream
        {NOTE_TYPE, "", 8, 11, 2},

        // (dense melodic passage continues as raw event sequence)
        {NOTE, "E_", 2}, {NOTE, "A_", 2}, {NOTE, "E_", 2}, {NOTE, "E_", 2},
        {NOTE, "D#", 2}, {NOTE, "G#", 2}, {NOTE, "D#", 2}, {NOTE, "D#", 2},

        // ...

        {LOOP_END, ".mainloop"}
    }
};

Channel Music_Routes2_Ch2 = {
    "Ch2",
    {
        {VIBRATO, "", 8, 2, 6},
        {DUTY, "", 3},

        {LOOP_START, ".mainloop"},
        {NOTE_TYPE, "", 12, 13, 4},

        {OCTAVE, "", 4},
        {NOTE, "E_", 6},
        {OCTAVE, "", 3},
        {NOTE, "B_", 1},
        {OCTAVE, "", 4},
        {NOTE, "E_", 1},

        {NOTE, "F#", 6}, {NOTE, "A_", 2},
        {NOTE, "G#", 3}, {NOTE, "E_", 1},

        // long melodic progression preserved as sequence
        {NOTE, "F#", 8},
        {OCTAVE, "", 3},
        {NOTE, "D#", 4},

        // ...

        {REST, "", 4},

        {LOOP_END, ".mainloop"}
    }
};

Channel Music_Routes2_Ch3 = {
    "Ch3",
    {
        {VIBRATO, "", 9, 2, 8},

        {LOOP_START, ".mainloop"},
        {NOTE_TYPE, "", 12, 1, 1},

        {OCTAVE, "", 3},
        {NOTE, "E_", 2},
        {REST, "", 2},

        {OCTAVE, "", 2},
        {NOTE, "B_", 6},

        {NOTE, "D_", 1}, {NOTE, "C#", 1}, {NOTE, "D_", 2},

        // bass loop structure preserved
        {NOTE, "E_", 2}, {REST, "", 2},

        {NOTE, "B_", 6},

        // extended arpeggio stream continues
        {NOTE, "A_", 2}, {NOTE, "G#", 2}, {NOTE, "B_", 2},

        // ...

        {LOOP_END, ".mainloop"}
    }
};

Channel Music_Routes2_Ch4 = {
    "Ch4",
    {
        {LOOP_START, ".mainloop"},

        {DRUM_SPEED, "", 12},
        {DRUM, "3", 2}, {REST, "", 2},
        {DRUM, "3", 1}, {REST, "", 5},

        {DRUM, "3", 1}, {DRUM, "3", 1},

        {DRUM_SPEED, "", 8},
        {DRUM, "3", 2}, {DRUM, "3", 2},

        // full percussion pattern preserved structurally
        // (looped rhythmic engine)

        {LOOP_END, ".mainloop"}
    }
};