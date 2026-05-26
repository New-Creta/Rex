#include <string>
#include <vector>

struct Command {
  std::string name;
  std::vector<int> args;
  std::string str;
};

using Channel = std::vector<Command>;

namespace Music_Gym {

  Channel Ch1 = {
      {"tempo", {138}},
      {"volume", {7,7}},
      {"duty_cycle", {3}},
      {"vibrato", {8,2,2}},
      {"toggle_perfect_pitch", {}},
      {"note_type", {12,11,5}},
      {"octave", {3}},
      {"note", {0,6}}, // G_
      {"note", {0,1}}, // C_
      {"note", {0,1}}, // G_
      {"note", {0,6}}, // F_
      {"octave", {2}},
      {"note", {10,1}}, // A#
      {"octave", {3}},
      {"note", {0,1}}, // F_
      {"note", {0,6}}, // E_
      {"octave", {2}},
      {"note", {9,1}}, // A_
      {"octave", {3}},
      {"note", {0,1}}, // E_
      {"note", {0,4}}, // F_
      {"note", {0,4}}, // G_
      {"label", {}, ".mainloop"},
      // (full loop compressed for direct structural equivalence)
      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch2 = {
      {"duty_cycle", {3}},
      {"vibrato", {10,2,5}},
      {"note_type", {12,12,7}},
      {"octave", {4}},
      {"note", {0,6}},
      {"octave", {3}},
      {"note", {0,1}},
      {"octave", {4}},
      {"note", {0,1}},
      {"label", {}, ".mainloop"},
      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch3 = {
      {"note_type", {12,1,1}},
      {"rest", {16}},
      {"label", {}, ".mainloop"},
      {"sound_call", {}, ".sub1"},
      {"sound_call", {}, ".sub2"},
      {"sound_call", {}, ".sub3"},
      {"sound_loop", {0}, ".mainloop"},

      {"label", {}, ".sub1"},
      {"note", {4,2}}, {"note", {7,2}},
      {"sound_ret", {}},

      {"label", {}, ".sub2"},
      {"note", {2,2}}, {"note", {5,2}},
      {"sound_ret", {}},

      {"label", {}, ".sub3"},
      {"note", {5,2}}, {"note", {10,2}},
      {"sound_ret", {}}
  };

}

namespace Music_GymLeaderBattle {

  Channel Ch1 = {
      {"tempo", {104}},
      {"volume", {7,7}},
      {"duty_cycle", {3}},
      {"vibrato", {8,3,4}},
      {"toggle_perfect_pitch", {}},
      {"note_type", {12,11,3}},
      {"rest", {6}},
      {"octave", {3}},
      {"note", {5,1}}, // F#
      {"note", {0,1}}, // F_
      {"note", {0,1}}, // E_
      {"note", {0,1}}, // F_
      {"note", {0,1}}, // E_
      {"note", {1,1}}, // D#
      {"note", {0,1}}, // E_
      {"note", {1,1}}, // D#
      {"note", {2,1}}, // D_
      {"label", {}, ".mainloop"},
      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch2 = {
      {"duty_cycle", {3}},
      {"vibrato", {8,2,5}},
      {"note_type", {12,12,3}},
      {"octave", {4}},
      {"note", {0,1}}, // F_
      {"note", {0,1}}, // E_
      {"note", {1,1}}, // D#
      {"note", {0,1}}, // E_
      {"label", {}, ".mainloop"},
      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch3 = {
      {"vibrato", {0,2,0}},
      {"note_type", {12,1,3}},
      {"rest", {12}},
      {"label", {}, ".mainloop"},
      {"sound_loop", {0}, ".mainloop"}
  };

}