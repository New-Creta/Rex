#include <vector>
#include <string>

enum EventType {
  NOTE,
  REST,
  OCTAVE,
  TEMPO,
  VOLUME,
  DUTY,
  VIBRATO,
  NOTE_TYPE,
  LOOP_START,
  LOOP_END,
  CALL,
  RET
};

struct MusicEvent {
  EventType type;
  std::string note;   // e.g. "C#", "D_", or empty for non-note events
  int value = 0;       // duration, octave, tempo, etc.
  int value2 = 0;      // optional second parameter (e.g. volume right)
  int value3 = 0;      // optional third parameter
};

struct MusicChannel {
  std::string name;
  std::vector<MusicEvent> events;
};

// -------------------- ROUTES 1 --------------------

MusicChannel Music_Routes1_Ch1 = {
    "Ch1",
    {
        {TEMPO, "", 152},
        {VOLUME, "", 7, 7},
        {VIBRATO, "", 4, 2, 3},
        {DUTY, "", 2},
        {NOTE_TYPE, "", 12, 10, 1},

        {LOOP_START, ".mainloop"},

        {REST, "", 4},

        {OCTAVE, "", 4},
        {NOTE, "D_", 2}, {NOTE, "D_", 6}, {NOTE, "D_", 2}, {NOTE, "D_", 6},
        {NOTE, "D_", 2}, {NOTE, "D_", 1},
        {NOTE, "C#", 1},
        {OCTAVE, "", 3},
        {NOTE, "B_", 1},
        {OCTAVE, "", 4},
        {NOTE, "C#", 1},
        {OCTAVE, "", 3},
        {NOTE, "A_", 2}, {NOTE, "A_", 2}, {NOTE, "A_", 6},

        // (pattern continues from original assembly...)

        {LOOP_END, ".mainloop"}
    }
};

MusicChannel Music_Routes1_Ch2 = {
    "Ch2",
    {
        {DUTY, "", 2},

        {LOOP_START, ".mainloop"},

        {NOTE_TYPE, "", 12, 13, 1},

        {NOTE, "D_", 1}, {NOTE, "E_", 1}, {NOTE, "F#", 2},
        {NOTE, "F#", 2}, {NOTE, "F#", 2},

        // (truncated pattern mapping continues...)

        {LOOP_END, ".mainloop"}
    }
};

MusicChannel Music_Routes1_Ch3 = {
    "Ch3",
    {
        {VIBRATO, "", 8, 2, 5},
        {NOTE_TYPE, "", 12, 1, 3},

        {REST, "", 2},

        {LOOP_START, ".mainloop"},

        {OCTAVE, "", 4},
        {NOTE, "D_", 4}, {NOTE, "C#", 4}, {NOTE, "B_", 4}, {NOTE, "A_", 4},

        // (melody pattern continues...)

        {LOOP_END, ".mainloop"}
    }
};

MusicChannel Music_Routes1_Ch4 = {
    "Ch4",
    {
        {LOOP_START, ".mainloop"},

        {REST, "", 4},

        {DRUM, "15", 2}, {REST, "", 2},
        {DRUM, "15", 2}, {REST, "", 2},

        // (full percussion loop continues...)

        {LOOP_END, ".mainloop"}
    }
};