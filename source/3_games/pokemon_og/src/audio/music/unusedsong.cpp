#include <vector>
#include <string>

using namespace std;

struct NoteEvent {
  string note;
  int octave;
  int duration;
  bool isRest;
};

class Music_UnusedSong_Ch1 {
public:
  int tempo = 144;
  int volumeLeft = 7;
  int volumeRight = 7;

  vector<NoteEvent> sequence = {
      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"E_", 6, 1, false},
      {"REST", 0, 4, true},

      {"E_", 6, 2, false},
      {"REST", 0, 2, true},

      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"D#", 6, 1, false},
      {"REST", 0, 8, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"B_", 5, 1, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"B_", 5, 1, false},
      {"REST", 0, 2, true},

      {"D#", 6, 4, false},
      {"C#", 6, 4, false},

      {"D#", 6, 1, false},
      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},

      {"B_", 5, 2, false},
      {"REST", 0, 1, true},

      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"E_", 6, 1, false},
      {"REST", 0, 8, true},

      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 5, 1, false},
      {"D#", 6, 1, false},
      {"REST", 0, 4, true},

      {"B_", 5, 2, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"C#", 6, 1, false},
      {"B_", 5, 1, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 2, true},

      {"F#", 6, 4, false},
      {"E_", 6, 4, false},

      {"F#", 6, 1, false},
      {"G#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},

      {"D#", 6, 2, false},
      {"REST", 0, 3, true},

      {"A_", 5, 2, false},
      {"REST", 0, 2, true},
      {"B_", 5, 2, false},
      {"REST", 0, 2, true},

      {"C#", 6, 2, false},
      {"REST", 0, 2, true},
      {"E_", 6, 2, false},
      {"F#", 6, 8, false},
      {"REST", 0, 2, true},

      {"E_", 6, 2, false},
      {"D#", 6, 2, false},
      {"C#", 6, 2, false},
      {"REST", 0, 2, true},

      {"G#", 5, 2, false},
      {"REST", 0, 2, true},
      {"A_", 5, 2, false},
      {"REST", 0, 2, true},
      {"B_", 5, 2, false},
      {"REST", 0, 2, true},

      {"D#", 6, 2, false},
      {"E_", 6, 8, false},
      {"REST", 0, 2, true},

      {"D#", 6, 2, false},
      {"C#", 6, 2, false},
      {"D#", 6, 2, false},

      {"A_", 5, 2, false},
      {"G#", 5, 2, false},
      {"A_", 5, 2, false},

      {"A#", 5, 1, false},
      {"B_", 5, 1, false},
      {"REST", 0, 4, true},

      {"F#", 5, 2, false},
      {"REST", 0, 2, true},

      {"B_", 5, 2, false},
      {"A#", 5, 2, false},
      {"B_", 5, 2, false},

      {"C_", 6, 1, false},
      {"C#", 6, 1, false},
      {"REST", 0, 8, true},

      {"F#", 5, 2, false},
      {"F_", 5, 2, false},
      {"F#", 5, 2, false},
      {"G#", 5, 2, false},
      {"A_", 5, 2, false},

      {"E_", 6, 2, false},
      {"D#", 6, 2, false},
      {"C#", 6, 2, false},
      {"B_", 5, 2, false},

      {"REST", 0, 6, true},
      {"B_", 5, 2, false},
      {"REST", 0, 6, true},
  };
};

class Music_UnusedSong_Ch2 {
public:
  int tempo = 144;
  int volumeLeft = 7;
  int volumeRight = 7;

  vector<NoteEvent> sequence = {
      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"G#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},
      {"A_", 6, 1, false},
      {"G#", 6, 1, false},
      {"REST", 0, 8, true},

      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"G#", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 4, true},

      {"B_", 5, 2, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 2, true},

      {"F#", 6, 4, false},
      {"E_", 6, 4, false},

      {"F#", 6, 1, false},
      {"G#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},

      {"D#", 6, 2, false},
      {"REST", 0, 1, true},

      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"G#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},
      {"A_", 6, 1, false},
      {"G#", 6, 1, false},
      {"REST", 0, 4, true},

      {"E_", 6, 2, false},
      {"REST", 0, 2, true},

      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"F#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"REST", 0, 1, true},
      {"G#", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 8, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 2, true},

      {"C#", 6, 1, false},
      {"REST", 0, 1, true},
      {"D#", 6, 1, false},
      {"REST", 0, 1, true},
      {"E_", 6, 1, false},
      {"F#", 6, 1, false},
      {"REST", 0, 2, true},

      {"F#", 6, 4, false},
      {"E_", 6, 4, false},

      {"F#", 6, 1, false},
      {"G#", 6, 1, false},
      {"REST", 0, 1, true},
      {"B_", 6, 1, false},
      {"REST", 0, 1, true},

      {"F#", 6, 2, false},
      {"REST", 0, 1, true},

      {"C#", 6, 2, false},
      {"REST", 0, 2, true},
      {"D#", 6, 2, false},
      {"REST", 0, 2, true},
      {"E_", 6, 2, false},
      {"REST", 0, 2, true},
      {"G#", 6, 2, false},
      {"REST", 0, 2, true},

      {"D#", 6, 8, false},
      {"REST", 0, 2, true},

      {"E_", 6, 2, false},
      {"D#", 6, 2, false},
      {"C#", 6, 2, false},

      {"B_", 5, 2, false},
      {"REST", 0, 2, true},

      {"C#", 6, 2, false},
      {"REST", 0, 2, true},
      {"D#", 6, 2, false},
      {"REST", 0, 2, true},
      {"F#", 6, 2, false},
      {"REST", 0, 2, true},

      {"C#", 6, 8, false},
      {"REST", 0, 2, true},

      {"D#", 6, 2, false},
      {"C#", 6, 2, false},
      {"B_", 5, 2, false},

      {"A_", 5, 2, false},
      {"G#", 5, 2, false},
      {"A_", 5, 2, false},

      {"G#", 5, 1, false},
      {"F#", 5, 1, false},
      {"REST", 0, 8, true},

      {"B_", 5, 2, false},
      {"A#", 5, 2, false},
      {"B_", 5, 2, false},

      {"A_", 5, 1, false},
      {"G#", 5, 1, false},
      {"REST", 0, 4, true},

      {"G#", 5, 2, false},
      {"REST", 0, 2, true},

      {"F#", 5, 2, false},
      {"F_", 5, 2, false},
      {"F#", 5, 2, false},
      {"G#", 5, 2, false},
      {"A_", 5, 2, false},

      {"E_", 5, 2, false},
      {"C#", 5, 2, false},
      {"E_", 5, 2, false},
      {"F#", 5, 2, false},

      {"REST", 0, 6, true},
      {"D#", 5, 2, false},
      {"REST", 0, 6, true},
  };
};