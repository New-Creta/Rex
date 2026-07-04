#include <vector>
#include <string>

using namespace std;

struct NoteEvent {
  string note;
  int octave;
  int duration;
  bool isRest;
};

struct DrumEvent {
  int drum;
  int duration;
};

class Music_Vermilion_Ch1 {
public:
  int tempo = 156;
  int volumeLeft = 7;
  int volumeRight = 7;

  vector<NoteEvent> sequence = {
      {"E_",3,4,false},
      {"C#",3,1,false},
      {"D_",3,1,false},
      {"E_",3,2,false},
      {"A_",3,4,false},
      {"B_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"F#",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},
      {"A_",3,4,false},
      {"F#",3,1,false},
      {"G#",3,1,false},
      {"A_",3,2,false},
      {"E_",3,4,false},
      {"C#",3,2,false},
      {"E_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"B_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},
      {"G#",3,2,false},
      {"C#",3,2,false},
      {"D_",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},

      {"E_",3,4,false},
      {"C#",3,1,false},
      {"D_",3,1,false},
      {"E_",3,2,false},
      {"A_",3,4,false},
      {"B_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"F#",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},
      {"A_",3,4,false},
      {"F#",3,1,false},
      {"G#",3,1,false},
      {"A_",3,2,false},
      {"E_",3,4,false},
      {"C#",3,1,false},
      {"D_",3,1,false},
      {"E_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"F#",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},
      {"G#",3,2,false},
      {"F#",3,4,false},
      {"E_",3,4,false},

      {"F#",3,2,false},
      {"G#",3,2,false},
      {"F#",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"B_",3,2,false},
      {"A_",3,2,false},

      {"C#",4,2,false},
      {"D_",4,2,false},
      {"C#",4,2,false},
      {"B_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,1,false},
      {"A_",3,1,false},
      {"B_",3,2,false},

      {"C#",4,2,false},
      {"E_",4,2,false},
      {"A_",3,2,false},
      {"D_",4,2,false},
      {"G#",3,2,false},
      {"C#",4,2,false},
      {"F#",3,2,false},
      {"B_",3,2,false},
      {"G#",3,2,false},
      {"A_",3,2,false},
      {"B_",3,2,false},
      {"A_",3,2,false},
      {"G#",3,2,false},
      {"F#",3,2,false},
      {"E_",3,2,false},
      {"F#",3,2,false},
      {"G#",3,2,false},
      {"B_",3,2,false},
  };
};

class Music_Vermilion_Ch2 {
public:
  vector<NoteEvent> sequence = {
      {"A_",3,8,false},
      {"D_",4,4,false},
      {"C#",4,4,false},
      {"B_",3,6,false},
      {"A_",3,1,false},
      {"B_",3,1,false},
      {"C#",4,8,false},

      {"A_",3,8,false},
      {"D_",4,4,false},
      {"C#",4,4,false},
      {"B_",3,6,false},
      {"C#",4,1,false},
      {"B_",3,1,false},
      {"A_",3,8,false},

      {"A_",3,8,false},
      {"D_",4,4,false},
      {"C#",4,4,false},
      {"B_",3,6,false},
      {"A_",3,1,false},
      {"B_",3,1,false},
      {"C#",4,8,false},

      {"A_",3,8,false},
      {"D_",4,4,false},
      {"C#",4,4,false},
      {"B_",3,6,false},
      {"C#",4,1,false},
      {"B_",3,1,false},
      {"A_",3,8,false},

      {"B_",3,4,false},
      {"C#",4,4,false},
      {"D_",4,4,false},
      {"E_",4,4,false},
      {"F#",4,8,false},
      {"B_",4,8,false},

      {"A_",4,4,false},
      {"G#",4,4,false},
      {"F#",4,4,false},
      {"E_",4,4,false},
      {"F#",4,8,false},
      {"E_",4,8,false},
  };
};

class Music_Vermilion_Ch3 {
public:
  vector<NoteEvent> sequence = {
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},

      {"G#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"G#",4,2,false},
      {"F#",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},

      {"G#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"G#",4,2,false},
      {"F#",4,2,false},{"G#",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},

      {"G#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},
      {"B_",4,2,false},{"E_",4,2,false},
      {"A_",4,2,false},{"E_",4,2,false},

      {"G#",4,2,false},{"E_",4,2,false},
      {"F#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},

      {"B_",4,2,false},{"A_",4,2,false},
      {"G#",4,2,false},{"F#",4,2,false},
      {"F#",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"B_",4,2,false},{"E_",4,2,false},
      {"B_",4,2,false},{"E_",4,2,false},
      {"B_",4,2,false},{"E_",4,2,false},

      {"A_",4,2,false},{"E_",4,2,false},
      {"G#",4,2,false},{"E_",4,2,false},
  };
};

class Music_Vermilion_Ch4 {
public:
  int drumSpeed = 12;

  vector<DrumEvent> sequence = {
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},

      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,1},{6,1},{6,1},{6,1},

      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
      {6,2},{6,1},{6,1},
  };
};