namespace Music_Dungeon2 {

  struct M {
    static void tempo(int) {}
    static void volume(int, int) {}
    static void duty_cycle(int) {}
    static void vibrato(int, int, int) {}
    static void note_type(int, int, int) {}
    static void octave(int) {}
    static void note(const char*, int = 0) {}
    static void rest(int) {}
  };

  using Music = M;

  // ---------------- CH1 ----------------
  void Music_Dungeon2_Ch1() {
    Music::tempo(144);
    Music::volume(7, 7);
    Music::duty_cycle(3);
    Music::toggle_perfect_pitch();
    Music::vibrato(10, 1, 4);

  mainloop:
    while (true) {

      for (int rep = 0; rep < 2; rep++) {
        Music::note_type(12, 11, 2);

        Music::octave(4);
        for (int i = 0; i < 4; i++) Music::note("E_", 4);
        for (int i = 0; i < 4; i++) Music::note("A#", 4);
        for (int i = 0; i < 4; i++) Music::note("E_", 4);

        Music::octave(5);
        for (int i = 0; i < 4; i++) Music::note("C#", 4);

        Music::octave(3);
        for (int i = 0; i < 4; i++) Music::note("E_", 4);
        for (int i = 0; i < 4; i++) Music::note("A#", 4);

        Music::octave(2);
        Music::note("G_", 2); Music::note("A#", 4);
        Music::note("G_", 2);

        Music::octave(3);
        Music::note("C#", 4);

        Music::octave(2);
        Music::note("G_", 2); Music::note("A#", 2);
        Music::note("B_", 2); Music::note("G_", 2);

        Music::octave(3);
        Music::note("C#", 4);

        Music::octave(2);
        Music::note("G_", 2); Music::note("A_", 4); Music::note("F#", 2);
      }

      Music::note_type(12, 1, -7);
      Music::octave(3);
      Music::note("E_", 16);
      Music::note("C_", 16);
      Music::note("D_", 16);
      Music::octave(2);
      Music::note("A#", 16);

      for (int i = 0; i < 9; i++) Music::rest(16);
    }
  }

  // ---------------- CH2 ----------------
  void Music_Dungeon2_Ch2() {
    Music::vibrato(11, 1, 5);

  mainloop:
    while (true) {

      for (int r = 0; r < 2; r++) {
        Music::duty_cycle(3);
        Music::note_type(12, 12, 2);

        Music::octave(3);
        for (int i = 0; i < 4; i++) Music::note("E_", 4);
        for (int i = 0; i < 4; i++) Music::note("C_", 4);
        for (int i = 0; i < 4; i++) Music::note("E_", 4);
        for (int i = 0; i < 4; i++) Music::note("C_", 4);
        for (int i = 0; i < 4; i++) Music::note("B_", 4);

        Music::octave(4);
        for (int i = 0; i < 4; i++) Music::note("F#", 4);
        for (int i = 0; i < 4; i++) Music::note("D_", 4);

        Music::note("G_", 4); Music::note("G_", 4); Music::note("G_", 4); Music::note("F#", 4);
      }

      Music::octave(3);
      Music::note("E_", 2); Music::note("G_", 2); Music::note("E_", 2); Music::note("D#", 2);
      Music::note("E_", 2); Music::note("E_", 2);

      Music::octave(5);
      Music::note("E_", 2); Music::rest(2);
      Music::note("D#", 2); Music::rest(2);
      Music::note("D_", 2); Music::rest(2);
      Music::note("C#", 2); Music::note("C_", 2);

      Music::octave(4);
      Music::note("E_", 2); Music::note("G_", 2);

      Music::octave(3);
      Music::note("A#", 2); Music::note("C#", 2); Music::note("A#", 2);
      Music::note("A_", 2); Music::note("A#", 2); Music::note("G_", 2);

      Music::octave(5);
      Music::note("G_", 2); Music::rest(2);
      Music::note("F#", 2); Music::rest(2);
      Music::note("F_", 2); Music::rest(2);
      Music::note("E_", 2); Music::note("D#", 2);
      Music::note("D_", 2); Music::note("C#", 2);

      for (int i = 0; i < 4; i++) Music::rest(16);

      Music::note_type(12, 12, 7);
      Music::duty_cycle(1);
      Music::octave(4);
      Music::note("E_", 16);
      Music::note("D_", 16);
      Music::note("C_", 16);
      Music::note("D_", 16);
    }
  }

  // ---------------- CH3 ----------------
  void Music_Dungeon2_Ch3() {
    Music::note_type(12, 1, 3);
    Music::vibrato(8, 2, 6);

  mainloop:
    while (true) {

      for (int i = 0; i < 16; i++) {
        // sub2 pattern
        Music::octave(4);
        Music::note("E_", 2); Music::rest(4);
        Music::octave(3);
        Music::note("E_", 1); Music::rest(3);
        Music::note("E_", 1); Music::rest(1);
        Music::octave(4);
        Music::note("F#", 4);
      }

      Music::octave(5);
      Music::note("E_", 4); Music::rest(4); Music::rest(4);
      Music::note("E_", 4);

      Music::note("C_", 4); Music::rest(4); Music::rest(4);
      Music::note("C_", 4);

      Music::note("D_", 4); Music::rest(4); Music::rest(4);
      Music::note("D_", 4);

      Music::octave(3);
      Music::note("A#", 4); Music::rest(4); Music::rest(4);
      Music::note("A#", 4);

      for (int i = 0; i < 3; i++) {
        Music::octave(5);
        Music::note("E_", 2); Music::rest(2);
        Music::note("B_", 2); Music::rest(2);
        Music::note("A#", 2); Music::rest(2);
        Music::octave(6);
        Music::note("D_", 2); Music::rest(2);
        Music::note("C#", 2); Music::rest(2);
        Music::octave(5);
        Music::note("G#", 2); Music::rest(2);
        Music::note("G_", 2); Music::rest(2);
        Music::note("B_", 2); Music::rest(2);
        Music::note("A#", 2); Music::rest(2);
        Music::note("E_", 2); Music::rest(2);
        Music::note("D#", 2); Music::rest(2);
        Music::note("A_", 2); Music::rest(2);
        Music::note("G#", 2); Music::rest(2);
        Music::note("E_", 2); Music::rest(2);
        Music::note("F#", 2); Music::rest(2);
        Music::note("D#", 2); Music::rest(2);
      }

      Music::octave(4);
      Music::note("E_", 4); Music::note("B_", 4); Music::note("A#", 4);
      Music::octave(5); Music::note("D_", 4); Music::note("C#", 4);
      Music::octave(4); Music::note("G#", 4); Music::note("G_", 4);
      Music::note("B_", 4); Music::note("A#", 4);
      Music::note("E_", 4); Music::note("D#", 4);
      Music::note("A_", 4); Music::note("G#", 4);
      Music::note("E_", 4); Music::note("F#", 4); Music::note("D#", 4);

      Music::octave(3);
      Music::note("E_", 16); Music::note("C_", 16); Music::note("D_", 16);
      Music::octave(2); Music::note("A#", 16);

      Music::octave(3);
      Music::note("E_", 16); Music::note("F_", 16);
      Music::note("G_", 16); Music::octave(3); Music::note("B_", 16);

      for (int i = 0; i < 4; i++) Music::rest(16);
    }
  }

  // ---------------- CH4 ----------------
  void Music_Dungeon2_Ch4() {
    Music::rest(0);

  mainloop:
    while (true) {
      Music::rest(0);
    }
  }

} // namespace