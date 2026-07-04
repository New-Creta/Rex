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

std::vector<Cmd> Music_Dungeon3_Ch1() {
  return {
      {TEMPO,112},{VOLUME,7,7},{DUTY_CYCLE,3},{TOGGLE_PERFECT_PITCH},
      {VIBRATO,8,1,4},

      {NOTE_TYPE,12,12,3},

      {SOUND_LOOP,3}, {SOUND_CALL,0,0,0,".sub1"},
      {OCTAVE,2},{NOTE,0,0,2,"B_"},
      {OCTAVE,3},{NOTE,0,0,2,"D#"},
      {NOTE,0,0,2,"G_"},
      {OCTAVE,2},{NOTE,0,0,2,"B_"},
      {OCTAVE,3},{NOTE,0,0,2,"D#"},
      {NOTE,0,0,2,"G_"},
      {NOTE,0,0,2,"D#"},
      {NOTE,0,0,2,"E_"},
  };
}

std::vector<Cmd> Music_Dungeon3_Ch2() {
  return {
      {VIBRATO,11,1,5},
      {DUTY_CYCLE,3},
      {NOTE_TYPE,12,13,3},

      {OCTAVE,4},{NOTE,0,0,6,"D#"},
      {NOTE,0,0,6,"C#"},
      {OCTAVE,3},{NOTE,0,0,2,"B_"},
      {OCTAVE,4},{NOTE,0,0,2,"C#"},
      {NOTE,0,0,2,"D#"},
      {NOTE,0,0,2,"G_"},
  };
}

std::vector<Cmd> Music_Dungeon3_Ch3() {
  return {
      {NOTE_TYPE,12,1,2},
      {REST,16},{REST,16},{REST,16},{REST,16},
      {REST,16},{REST,16},{REST,16},{REST,16},
  };
}

std::vector<Cmd> Music_Dungeon3_Ch4() {
  return {
      {0},{REST,16},{REST,16},{REST,16}
  };
}


// ================= FINAL BATTLE =================

std::vector<Cmd> Music_FinalBattle_Ch1() {
  return {
      {TEMPO,112},
      {VOLUME,7,7},
      {DUTY_CYCLE,3},
      {VIBRATO,6,3,4},
      {TOGGLE_PERFECT_PITCH},

      {NOTE_TYPE,12,11,2},
      {OCTAVE,3},

      {NOTE,0,0,1,"F#"},
      {NOTE,0,0,1,"F_"},
      {NOTE,0,0,1,"F#"},
      {NOTE,0,0,1,"G_"},
      {NOTE,0,0,1,"F#"},
      {NOTE,0,0,1,"G_"},
      {NOTE,0,0,1,"G#"},
      {NOTE,0,0,1,"G_"},
  };
}

std::vector<Cmd> Music_FinalBattle_Ch2() {
  return {
      {DUTY_CYCLE,3},
      {VIBRATO,8,2,5},

      {NOTE_TYPE,12,12,2},
      {OCTAVE,5},

      {NOTE,0,0,1,"C_"},
      {OCTAVE,4},{NOTE,0,0,1,"G#"},
      {NOTE,0,0,1,"B_"},
      {NOTE,0,0,1,"F#"},
  };
}

std::vector<Cmd> Music_FinalBattle_Ch3() {
  return {
      {NOTE_TYPE,12,1,1},
      {OCTAVE,4},
      {NOTE,0,0,4,"E_"},
      {OCTAVE,5},{NOTE,0,0,4,"E_"},
  };
}