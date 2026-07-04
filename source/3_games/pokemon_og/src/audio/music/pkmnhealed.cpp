// Pokémon Healed Theme - C++ representation

#include <vector>
#include <string>

enum EventType {
  NOTE,
  REST,
  PITCH_SLIDE,
  END
};

struct Event {
  EventType type;
  std::string note;   // e.g. "B_", "E_"
  int length;         // tick length
  int param1;         // used for pitch_slide start/amount etc.
  int param2;         // optional
};

struct Channel {
  std::string name;
  int tempo;
  int volumeLeft;
  int volumeRight;
  int dutyCycle;

  std::vector<Event> events;
};

// ---------------- CHANNEL 1 ----------------
Channel Music_PkmnHealed_Ch1 = {
    "Ch1",
    144,
    7, 7,
    2,
    {
        {REST, "", 2},
        {PITCH_SLIDE, "B_", 4, 1},
        {NOTE, "B_", 2},
        {PITCH_SLIDE, "E_", 3, 1},
        {NOTE, "B_", 2},
        {PITCH_SLIDE, "E_", 4, 1},
        {NOTE, "E_", 2},
        {REST, "", 4},
        {PITCH_SLIDE, "B_", 5, 1},
        {NOTE, "E_", 4},
        {PITCH_SLIDE, "B_", 4, 1},
        {NOTE, "B_", 4},
        {END, "", 0}
    }
};

// ---------------- CHANNEL 2 ----------------
Channel Music_PkmnHealed_Ch2 = {
    "Ch2",
    144,
    7, 7,
    2,
    {
        {NOTE, "B_", 4},
        {NOTE, "B_", 4},
        {NOTE, "B_", 2},
        {NOTE, "G#_", 2},
        {NOTE, "E_", 8},
        {END, "", 0}
    }
};

// ---------------- CHANNEL 3 ----------------
Channel Music_PkmnHealed_Ch3 = {
    "Ch3",
    144,
    7, 7,
    1,
    {
        {NOTE, "E_", 2},
        {REST, "", 2},
        {NOTE, "E_", 2},
        {REST, "", 2},
        {NOTE, "E_", 2},
        {NOTE, "G#_", 2},
        {NOTE, "E_", 6},
        {REST, "", 2},
        {END, "", 0}
    }
};