#include <vector>
#include <string>

struct Cmd {
  std::string op;
  std::vector<int> a;
  std::string s;
};

using Channel = std::vector<Cmd>;

Channel Music_Gym_Ch1 = {
    {"tempo",{138},""},
    {"volume",{7,7},""},
    {"duty_cycle",{3},""},
    {"vibrato",{8,2,2},""},
    {"toggle_perfect_pitch",{},""},
    {"note_type",{12,11,5},""},
    {"octave",{3},""},
    {"note",{}, "G_6"},
    {"note",{}, "C_1"},
    {"note",{}, "G_1"},
    {"note",{}, "F_6"},
    {"octave",{2},""},
    {"note",{}, "A#_1"},
    {"octave",{3},""},
    {"note",{}, "F_1"},
    {"note",{}, "E_6"},
    {"octave",{2},""},
    {"note",{}, "A_1"},
    {"octave",{3},""},
    {"note",{}, "E_1"},
    {"note",{}, "F_4"},
    {"note",{}, "G_4"},

    {"label",{},"mainloop"},

    {"note",{}, "E_4"},
    {"note",{}, "F_2"},
    {"note",{}, "G_2"},
    {"note",{}, "F_2"},
    {"note",{}, "E_2"},
    {"note",{}, "D_2"},
    {"note",{}, "C_2"},

    {"note_type",{12,11,1},""},
    {"note",{}, "D_3"},

    {"note_type",{12,11,5},""},
    {"note",{}, "E_1"},
    {"note",{}, "F_4"},
    {"note",{}, "E_2"},
    {"note",{}, "D_2"},
    {"note",{}, "E_2"},
    {"note",{}, "F_2"},

    {"note",{}, "E_4"},
    {"note",{}, "F_2"},
    {"note",{}, "G_2"},
    {"note",{}, "F_2"},
    {"note",{}, "E_2"},
    {"note",{}, "D_2"},
    {"note",{}, "C_2"},
    {"note",{}, "D_2"},

    {"note",{}, "D_1"},
    {"note",{}, "E_1"},
    {"note",{}, "F_4"},
    {"note",{}, "E_2"},
    {"note",{}, "D_2"},
    {"note",{}, "E_2"},
    {"note",{}, "F_2"},

    {"octave",{4},""},
    {"note",{}, "C_4"},
    {"octave",{3},""},
    {"note",{}, "A#_2"},
    {"octave",{4},""},
    {"note",{}, "C_2"},
    {"octave",{3},""},
    {"note",{}, "A#_2"},
    {"note",{}, "A_2"},
    {"note",{}, "G_2"},
    {"note",{}, "F_2"},

    {"note_type",{12,11,1},""},
    {"note",{}, "A#_3"},

    {"note_type",{12,11,5},""},
    {"note",{}, "F_1"},
    {"note",{}, "F_4"},
    {"note",{}, "E_2"},
    {"note",{}, "D_2"},
    {"note",{}, "E_2"},
    {"note",{}, "F_2"},
    {"note",{}, "C_4"}
};

Channel Music_Gym_Ch2 = {
    {"duty_cycle",{3},""},
    {"vibrato",{10,2,5},""},
    {"note_type",{12,12,7},""},
    {"octave",{4},""},
    {"note",{}, "C_6"},
    {"octave",{3},""},
    {"note",{}, "G_1"},
    {"octave",{4},""},
    {"note",{}, "C_1"},
    {"octave",{3},""},
    {"note",{}, "A#_6"},
    {"note",{}, "F_1"},
    {"note",{}, "A#_1"},

    {"note_type",{12,10,0},""},
    {"note",{}, "A_12"},

    {"note_type",{12,12,7},""},
    {"note",{}, "B_4"},

    {"label",{},"mainloop"},

    {"octave",{4},""},
    {"note",{}, "C_12"},
    {"octave",{3},""},
    {"note",{}, "G_2"},
    {"octave",{4},""},
    {"note",{}, "C_2"},

    {"note_type",{12,12,2},""},
    {"note",{}, "D_3"},
    {"octave",{3},""},
    {"note",{}, "A#_1"},

    {"note_type",{12,12,7},""},
    {"note",{}, "A#_12"},

    {"octave",{4},""},
    {"note",{}, "C_12"}
};

Channel Music_Gym_Ch3 = {
    {"note_type",{12,1,1},""},
    {"rest",{16},""},
    {"rest",{10},""},
    {"octave",{4},""},
    {"note",{}, "G_2"},
    {"note",{}, "F_2"},
    {"note",{}, "D_2"},

    {"label",{},"mainloop"},

    {"call",{},"sub1"},
    {"call",{},"sub2"},
    {"call",{},"sub1"},
    {"call",{},"sub2"},
    {"call",{},"sub1"},
    {"call",{},"sub2"},
    {"call",{},"sub1"},
    {"call",{},"sub2"},
    {"call",{},"sub1"},
    {"call",{},"sub3"},
    {"call",{},"sub1"},
    {"call",{},"sub3"},
    {"call",{},"sub1"},
    {"call",{},"sub3"},
    {"call",{},"sub1"},

    {"note",{}, "F_2"},
    {"note",{}, "A#_2"},
    {"note",{}, "F_2"},
    {"note",{}, "A#_2"},
    {"note",{}, "F_2"},
    {"note",{}, "A#_2"},
    {"note",{}, "A_2"},
    {"note",{}, "F_2"}
};