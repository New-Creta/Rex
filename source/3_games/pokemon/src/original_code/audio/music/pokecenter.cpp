#include <vector>
#include <string>

enum EventType {
  NOTE,
  REST,
  CALL,
  RET,
  END
};

struct Event {
  EventType type;
  std::string note;   // "C#", "F#", etc.
  int length = 0;

  // for call/subroutine logic
  std::string target;
};

struct Channel {
  std::string name;
  int tempo = 0;
  int volumeL = 0;
  int volumeR = 0;

  std::vector<Event> mainLoop;
  std::vector<Event> sub1;
  std::vector<Event> sub2;
  std::vector<Event> sub3;
};

// ================= CHANNEL 1 =================
Channel Music_Pokecenter_Ch1 = {
    "Ch1",
    144, 7, 7,
    {
    // mainLoop (flattened control flow preserved)
    {NOTE, "F#", 2},
    {NOTE, "F_", 2},
    {NOTE, "F#", 2},
    {NOTE, "D_", 4},
    {NOTE, "C#", 2},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "G_", 2},
    {NOTE, "F#", 2},
    {NOTE, "E_", 2},
    {NOTE, "F#", 2},
    {NOTE, "G_", 2},
    {NOTE, "A_", 2},
    {NOTE, "A_", 2},
    {NOTE, "E_", 2},
    {NOTE, "A_", 2},
    {NOTE, "C#", 4},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "G_", 2},
    {NOTE, "F#", 2},
    {NOTE, "A_", 2},
    {NOTE, "B_", 2},
    {NOTE, "C#", 2},
    {NOTE, "D_", 2},
    {NOTE, "C#", 2},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "F#", 2},
    {NOTE, "F_", 2},
    {NOTE, "F#", 2},
    {NOTE, "D_", 4},
    {NOTE, "C#", 2},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "B_", 2},
    {NOTE, "A_", 2},
    {NOTE, "G_", 2},
    {NOTE, "F#", 2},
    {NOTE, "E_", 2},
    {NOTE, "F#", 2},
    {NOTE, "G_", 2},
    {NOTE, "A_", 2},
    {END, "", 0}
}
};

// ================= CHANNEL 2 =================
Channel Music_Pokecenter_Ch2 = {
    "Ch2",
    144, 7, 7,
    {
        {CALL, "", 0, "sub1"},
        {CALL, "", 0, "sub2"},
        {NOTE, "F#", 8},
        {NOTE, "A_", 8},
        {NOTE, "G_", 2},
        {NOTE, "A_", 2},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {NOTE, "E_", 8},
        {NOTE, "C#", 8},
        {NOTE, "E_", 8},
        {NOTE, "F#", 2},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {NOTE, "E_", 2},
        {NOTE, "D_", 8},
        {NOTE, "F#", 8},
        {NOTE, "A_", 8},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {NOTE, "G_", 2},
        {NOTE, "A_", 2},
        {NOTE, "B_", 8},
        {NOTE, "A_", 4},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {NOTE, "G_", 8},
        {NOTE, "F#", 2},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {NOTE, "E_", 2},
        {NOTE, "D_", 8},
        {END, "", 0}
    }
};

// ================= CHANNEL 3 =================
Channel Music_Pokecenter_Ch3 = {
    "Ch3",
    144, 7, 7,
    {
        {NOTE, "D_", 2},
        {NOTE, "F#", 2},
        {NOTE, "D_", 2},
        {NOTE, "F#", 2},
        {NOTE, "D_", 2},
        {NOTE, "F#", 2},
        {NOTE, "G_", 2},
        {NOTE, "F#", 2},
        {CALL, "", 0, "sub1"},
        {CALL, "", 0, "sub2"},
        {NOTE, "F#", 2},
        {NOTE, "A_", 2},
        {NOTE, "G_", 2},
        {NOTE, "A_", 2},
        {NOTE, "G_", 2},
        {NOTE, "A_", 2},
        {NOTE, "G_", 2},
        {NOTE, "A_", 2},
        {END, "", 0}
    }
};