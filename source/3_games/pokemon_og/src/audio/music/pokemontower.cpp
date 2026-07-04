#include <vector>
#include <string>

enum EventType {
  NOTE,
  REST,
  SET_TYPE,
  CALL,
  LOOP_START,
  LOOP_END
};

struct Event {
  EventType type;
  std::string note;   // "C#", "G_", etc.
  int length = 0;

  int param1 = 0;
  int param2 = 0;
};

struct Channel {
  std::string name;
  int tempo = 0;
  int volumeL = 0;
  int volumeR = 0;
  int dutyCycle = 0;

  std::vector<Event> events;
};

// ================= CH 1 =================
Channel Music_PokemonTower_Ch1 = {
    "Ch1",
    152, 7, 7, 3,
    {
        {REST, "", 4},
        {NOTE, "B_", 12},

        {NOTE, "G_", 1}, {REST, "", 7},
        {NOTE, "G_", 1}, {REST, "", 7},
        {NOTE, "B_", 1}, {REST, "", 7},
        {NOTE, "B_", 1}, {REST, "", 3},

        {NOTE, "B_", 1}, {REST, "", 3},
        {NOTE, "B_", 1}, {REST, "", 7},
        {NOTE, "B_", 1}, {REST, "", 7},

        {NOTE, "F#", 1}, {REST, "", 7},
        {NOTE, "C_", 1},
        {NOTE, "B_", 1},
        {NOTE, "G_", 1}, {REST, "", 5},

        {NOTE, "E_", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 7},

        {NOTE, "F#", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 7},

        {NOTE, "G_", 1}, {REST, "", 7},
        {NOTE, "G_", 1}, {REST, "", 7},

        {NOTE, "F#", 1}, {REST, "", 7},
        {NOTE, "F#", 1}, {REST, "", 7},

        {NOTE, "G_", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 7},

        {NOTE, "D_", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 7},

        {NOTE, "G_", 1}, {REST, "", 7},
        {NOTE, "G_", 1}, {REST, "", 7},

        {NOTE, "F#", 1}, {REST, "", 7},

        {NOTE, "B_", 1}, {REST, "", 7},
        {NOTE, "B_", 1}, {REST, "", 7},

        {NOTE, "C_", 1}, {REST, "", 7},
        {NOTE, "C_", 1}, {REST, "", 7},

        {NOTE, "C#", 1}, {REST, "", 7},
        {NOTE, "C#", 1}, {REST, "", 7},

        {NOTE, "C_", 1}, {REST, "", 7},
        {NOTE, "C_", 1}, {REST, "", 3},

        {NOTE, "C_", 1}, {REST, "", 3},
        {NOTE, "D_", 1}, {REST, "", 7},

        {NOTE, "D_", 1}, {REST, "", 7},

        {NOTE, "A_", 1}, {REST, "", 7},
        {NOTE, "A_", 1}, {REST, "", 7},

        {NOTE, "B_", 8},
        {NOTE, "B_", 8},
        {NOTE, "C_", 8},
        {NOTE, "C_", 8},
        {NOTE, "C#", 8},
        {NOTE, "C#", 8},

        {NOTE, "D_", 16},
        {REST, "", 16}, {REST, "", 16}, {REST, "", 16}, {REST, "", 16},

        {NOTE, "B_", 4},
        {NOTE, "E_", 4},
        {NOTE, "D_", 4},
        {NOTE, "C_", 4},

        {END, "", 0}
    }
};

// ================= CH 2 =================
Channel Music_PokemonTower_Ch2 = {
    "Ch2",
    152, 7, 7, 3,
    {
        {NOTE, "C_", 12},
        {NOTE, "E_", 4},

        {NOTE, "C_", 8},
        {NOTE, "B_", 4},

        {NOTE, "G_", 1}, {NOTE, "F#", 1}, {NOTE, "E_", 1}, {NOTE, "D#", 1},

        {NOTE, "G_", 8},
        {NOTE, "C_", 8},

        {NOTE, "B_", 4},
        {NOTE, "G_", 4},

        {NOTE, "E_", 4},
        {NOTE, "G_", 4},

        {NOTE, "C_", 8},

        {NOTE, "C_", 8},

        {NOTE, "G_", 1}, {NOTE, "F#", 1}, {NOTE, "E_", 1}, {REST, "", 1},

        {NOTE, "G_", 4},
        {NOTE, "B_", 4},
        {NOTE, "G_", 4},
        {NOTE, "B_", 4},

        {NOTE, "C_", 4},
        {NOTE, "B_", 4},

        {NOTE, "C_", 16},
        {NOTE, "E_", 8},

        {END, "", 0}
    }
};

// ================= CH 3 =================
Channel Music_PokemonTower_Ch3 = {
    "Ch3",
    152, 7, 7, 0,
    {
        {REST, "", 8},
        {NOTE, "G_", 8},

        {NOTE, "E_", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 7},

        {NOTE, "E_", 1}, {REST, "", 7},
        {NOTE, "E_", 1}, {REST, "", 3},

        {NOTE, "E_", 1}, {NOTE, "D#", 1}, {NOTE, "F#", 1}, {NOTE, "D#", 1},

        {NOTE, "E_", 1}, {REST, "", 7},
        {NOTE, "G_", 1}, {REST, "", 7},

        {NOTE, "E_", 1}, {REST, "", 7},
        {NOTE, "B_", 1}, {REST, "", 7},

        {NOTE, "E_", 1}, {NOTE, "D#", 1},

        {END, "", 0}
    }
};