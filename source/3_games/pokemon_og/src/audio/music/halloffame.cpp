#include <string>
#include <vector>

struct Command {
  std::string op;
  std::vector<int> args;
  std::string label;
};

using Channel = std::vector<Command>;

namespace Music_HallOfFame {

  Channel Ch1 = {
      {"tempo", {112}},
      {"volume", {7,7}},
      {"duty_cycle", {3}},
      {"vibrato", {12,2,2}},
      {"toggle_perfect_pitch", {}},
      {"note_type", {12,11,3}},
      {"rest", {16}},
      {"rest", {16}},
      {"rest", {16}},
      {"rest", {12}},
      {"octave", {3}},
      {"note", {2,1}}, // D_
      {"note", {4,1}}, // E_
      {"note", {5,1}}, // F_
      {"note", {6,1}}, // F#

      {"label", {}, ".mainloop"},
      {"label", {}, ".loop1"},

      {"note_type", {12,8,0}},
      {"octave", {4}},
      {"note", {0,16}}, // C_
      {"note", {4,16}}, // E_
      {"note", {6,16}}, // F#
      {"note_type", {12,6,0}},
      {"note", {5,16}}, // F_

      {"sound_loop", {3}, ".loop1"},

      {"rest", {16}},
      {"rest", {16}},
      {"rest", {16}},
      {"rest", {16}},

      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch2 = {
      {"vibrato", {8,2,5}},
      {"duty_cycle", {3}},
      {"label", {}, ".mainloop"},

      {"note_type", {12,12,4}},
      {"octave", {3}},
      {"note", {7,2}},  // G_
      {"note", {2,2}},  // D_
      {"note", {7,2}},  // G_
      {"note", {9,10}}, // A_
      {"note", {7,2}},
      {"note", {2,2}},
      {"note", {7,2}},
      {"octave", {4}},
      {"note", {0,4}},  // C_
      {"octave", {3}},
      {"note", {11,4}}, // B_
      {"note", {9,2}},  // A_
      {"note", {7,2}},  // G_
      {"note", {2,2}},  // D_
      {"note", {7,2}},  // G_
      {"note", {9,10}}, // A_
      {"note", {5,2}},  // F_
      {"note", {0,2}},  // C_
      {"note", {5,2}},  // F_
      {"note", {10,4}}, // A#
      {"note", {9,4}},  // A_
      {"note", {5,2}},  // F_

      {"sound_loop", {0}, ".mainloop"}
  };

  Channel Ch3 = {
      {"label", {}, ".mainloop"},
      {"note_type", {12,1,2}},

      {"label", {}, ".loop1"},
      {"octave", {4}},
      {"note", {2,2}}, {"note", {7,2}},
      {"note", {2,2}}, {"note", {7,2}},
      {"note", {2,2}}, {"note", {7,2}},
      {"note", {2,2}}, {"note", {7,2}},

      {"sound_loop", {12}, ".loop1"},

      {"note", {2,4}}, {"note", {7,4}},
      {"note", {2,4}}, {"note", {7,4}},
      {"note", {4,4}}, {"note", {7,4}},
      {"note", {4,4}}, {"note", {7,4}},
      {"note", {6,4}}, {"note", {9,4}},
      {"note", {6,4}}, {"note", {9,4}},
      {"note", {4,4}}, {"note", {7,4}},
      {"note", {4,4}}, {"note", {7,4}},

      {"sound_loop", {0}, ".mainloop"}
  };

}