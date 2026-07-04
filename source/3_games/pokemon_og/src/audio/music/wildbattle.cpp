std::vector<Note> Music_WildBattle_Ch1 = {
    {Tempo, 104},
    {Volume, {7, 7}},
    {DutyCycle, 3},
    {Vibrato, {6, 3, 4}},
    {TogglePerfectPitch},
    {NoteType, {12, 11, 3}},
    {Octave, 4},
    {Note, {"C_", 1}},
    {Octave, 3},
    {Note, {"B_", 1}},
    {Note, {"A#", 1}},
    {Note, {"A_", 1}},
    {Note, {"A#", 1}},
    {Note, {"A_", 1}},
    {Note, {"G#", 1}},
    {Note, {"G_", 1}},
    {Note, {"G#", 1}},
    {Note, {"G_", 1}},
    {Note, {"F#", 1}},
    {Note, {"F_", 1}},
    {Note, {"F#", 1}},
    {Note, {"F_", 1}},
    {Note, {"E_", 1}},
    {Note, {"D#", 1}},
    {Note, {"E_", 1}},
    {Note, {"D#", 1}},
    {Note, {"D_", 1}},
    {Note, {"C#", 1}},
    {Note, {"D_", 1}},
    {Note, {"C#", 1}},
    {Note, {"C_", 1}},
    {Octave, 2},
    {Note, {"B_", 1}},
    {Octave, 3},
    {Note, {"C_", 1}},
    {Octave, 2},
    {Note, {"B_", 1}},
    {Note, {"A#", 1}},
    {Note, {"A_", 1}},
    {Note, {"A#", 1}},
    {Note, {"B_", 1}},
    {Octave, 3},
    {Note, {"C_", 1}},
    {Note, {"C#", 1}},

    {NoteType, {12, 11, 1}},
    {Note, {"G_", 6}},
    {Note, {"E_", 6}},
    {Note, {"D#", 12}},
    {Note, {"C#", 14}},
    {Note, {"E_", 6}},
    {Note, {"D#", 10}},

    {NoteType, {12, 4, -7}},
    {Note, {"C#", 10}},

    {NoteType, {12, 11, 1}},
    {Note, {"G_", 6}},
    {Note, {"E_", 6}},
    {Note, {"D#", 12}},
    {Note, {"C#", 14}},
    {Note, {"E_", 6}},
    {Note, {"D#", 10}},
    {Note, {"C#", 10}},

    {Label, "mainloop"},

    {NoteType, {12, 11, 3}},
    {Note, {"C#", 1}},
    {Note, {"D_", 1}},
    {Note, {"C#", 1}},
    {Note, {"C_", 1}},
    {Note, {"C#", 1}},
    {Note, {"D_", 1}},
    {Note, {"C#", 1}},
    {Note, {"C_", 1}},
    {Note, {"C#", 1}},
    {Note, {"D_", 1}},
    {Note, {"D#", 1}},
    {Note, {"D_", 1}},
    {Note, {"C#", 1}},
    {Note, {"C_", 1}},
    {Octave, 2},
    {Note, {"B_", 1}},
    {Octave, 3},
    {Note, {"C_", 1}},

    // ...
    // Remaining notes continue in the exact same format
    // ...

    {SoundLoop, {0, "mainloop"}}
};

std::vector<Note> Music_WildBattle_Ch2 = {
    {DutyCycle, 3},
    {Vibrato, {8, 2, 5}},
    {NoteType, {12, 12, 3}},
    {Octave, 4},

    {Note, {"G_", 1}},
    {Note, {"F#", 1}},
    {Note, {"F_", 1}},
    {Octave, 5},
    {Note, {"G_", 1}},

    // ...
    // Remaining channel data
    // ...

    {SoundLoop, {0, "mainloop"}}
};

std::vector<Note> Music_WildBattle_Ch3 = {
    {Vibrato, {0, 2, 0}},
    {NoteType, {12, 1, 1}},
    {Octave, 4},

    {Note, {"C#", 1}},
    {Rest, 1},
    {Note, {"C#", 1}},
    {Note, {"C_", 1}},

    // ...
    // Remaining channel data
    // ...

    {SoundLoop, {0, "mainloop"}}
};