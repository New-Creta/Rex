#include <vector>
#include <string>

enum CmdType {
  TEMPO,
  VOLUME,
  DUTY_CYCLE,
  VIBRATO,
  TOGGLE_PERFECT_PITCH,
  NOTE_TYPE,
  NOTE,
  REST,
  OCTAVE,
  SOUND_CALL,
  SOUND_LOOP,
  SOUND_RET
};

struct Cmd {
  CmdType type;
  int a = 0;
  int b = 0;
  int c = 0;
  std::string s;
};

// ================= GAME CORNER =================

std::vector<Cmd> Music_GameCorner_Ch1() {
  return {
      {TEMPO,120},
      {VOLUME,7,7},
      {DUTY_CYCLE,3},
      {VIBRATO,12,3,4},
      {TOGGLE_PERFECT_PITCH},
      {NOTE_TYPE,12,11,5},

      {OCTAVE,3},
      {NOTE,0,0,6,"A_"},

      {NOTE_TYPE,12,11,1},
      {NOTE,0,0,2,"G#"},
      {NOTE,0,0,2,"F#"},
      {NOTE,0,0,2,"E_"},
      {NOTE,0,0,2,"D#"},
      {NOTE,0,0,2,"C#"},
      {NOTE,0,0,16,"D#"},
  };
}

std::vector<Cmd> Music_GameCorner_Ch2() {
  return {
      {DUTY_CYCLE,2},
      {VIBRATO,10,2,3},

      {NOTE_TYPE,12,12,6},
      {OCTAVE,4},
      {NOTE,0,0,6,"C#"},

      {NOTE_TYPE,12,12,1},
      {OCTAVE,3},
      {NOTE,0,0,2,"B_"},
      {NOTE,0,0,2,"A_"},
      {NOTE,0,0,2,"G#"},
      {NOTE,0,0,2,"F#"},
      {NOTE,0,0,2,"E_"},
      {NOTE,0,0,2,"F#"},
  };
}

std::vector<Cmd> Music_GameCorner_Ch3() {
  return {
      {NOTE_TYPE,12,1,3},
      {OCTAVE,5},{NOTE,0,0,1,"C#"},
      {REST,5},
      {OCTAVE,4},{NOTE,0,0,1,"F#"},
      {REST,1},{NOTE,0,0,1,"G#"},
      {REST,1},{NOTE,0,0,1,"A_"},
      {REST,1},{NOTE,0,0,1,"A#"},
      {REST,1},{NOTE,0,0,1,"A#"},
      {REST,1},{NOTE,0,0,1,"B_"},
      {REST,9},
  };
}