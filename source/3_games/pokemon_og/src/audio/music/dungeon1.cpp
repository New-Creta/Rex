// Music_Dungeon1 C++ conversion (direct translation)

namespace Music_Dungeon1 {

  struct MusicEngine {
    static void tempo(int) {}
    static void volume(int, int) {}
    static void duty_cycle(int) {}
    static void vibrato(int, int, int) {}
    static void toggle_perfect_pitch() {}
    static void note_type(int, int, int) {}
    static void octave(int) {}
    static void note(const char*, int = 0) {}
    static void rest(int) {}
    static void stereo_panning(int, int) {}
    static void execute_music() {}
  };

  using M = MusicEngine;

  // ---------------- CH1 ----------------
  void ch1_sub1();
  void ch1_sub2();
  void ch1_sub3();
  void ch1_sub4();

  void Music_Dungeon1_Ch1() {
    M::tempo(144);
    M::volume(7, 7);
    M::duty_cycle(3);
    M::toggle_perfect_pitch();
    M::vibrato(10, 1, 4);
    M::note_type(12, 4, -5);
    M::rest(8);
    M::stereo_panning(0b1110, 0b1101);
    M::octave(4);
    M::note("F#", 8);
    M::stereo_panning(0b1111, 0b1111);

  mainloop:
    while (true) {
      M::note_type(12, 11, 2);
      M::octave(3);

      for (int i = 0; i < 3; i++) ch1_sub1();

      M::note("E_", 1); M::note("D#", 1); M::note("B_", 1); M::note("A#", 1);
      M::note("G_", 1); M::note("G#", 1); M::rest(1); M::note("A#", 1);
      M::note("E_", 1); M::note("D#", 1); M::note("B_", 1); M::note("A#", 1);
      M::note("G_", 1); M::note("G#", 1); M::note("A_", 1); M::note("A#", 1);

      for (int i = 0; i < 4; i++) ch1_sub1();

      ch1_sub4();
      M::octave(3);
      M::note("G_", 1); M::note("F#", 1); M::note("E_", 1); M::note("G_", 1);

      ch1_sub4();
      M::octave(3);
      M::note("A_", 1); M::note("G_", 1); M::note("E_", 1); M::note("A_", 1);

      ch1_sub4();
      M::octave(3);
      M::note("B_", 1); M::note("A_", 1); M::note("G_", 1); M::note("F#", 1);

      M::note_type(12, 12, 3);
      M::note("G_", 4); M::note("F#", 4); M::note("E_", 4); M::note("D#", 4);

      M::note_type(12, 12, 2);
      ch1_sub3(); M::octave(3); M::note("B_", 3);
      ch1_sub3(); M::note("C_", 3);
      ch1_sub3(); M::note("C#", 3);

      M::note_type(12, 12, 3);
      M::octave(3);
      M::note("G_", 4); M::note("F#", 4); M::note("E_", 4); M::note("D#", 4);
      M::note("E_", 4); M::note("D#", 4); M::note("C_", 4);
      M::octave(2); M::note("B_", 4);

      M::note_type(12, 12, 2);
      for (int i = 0; i < 4; i++) {
        M::note("B_", 1); M::note("A#", 1); M::note("G_", 1); M::note("A#", 1);
      }

      M::octave(3); M::note("C_", 1);
      M::octave(2); M::note("B_", 1);
      M::note("G#", 1); M::note("B_", 1);

      M::octave(3); M::note("C_", 1);
      M::octave(2); M::note("B_", 1);
      M::note("G#", 1); M::note("B_", 1);

      M::octave(3); M::note("C#", 1); M::note("C_", 1);
      M::octave(2); M::note("A_", 1);
      M::octave(3); M::note("C_", 1); M::note("C#", 1); M::note("C_", 1);

      M::octave(2); M::note("A_", 1);
      M::octave(3); M::note("C_", 1); M::note("D_", 1); M::note("C#", 1);

      M::octave(2); M::note("A#", 1);
      M::octave(3); M::note("C#", 1); M::note("D_", 1); M::note("C#", 1);

      M::octave(2); M::note("A#", 1);
      M::octave(3); M::note("C#", 1);

      for (int i = 0; i < 8; i++) { M::rest(16); }

      M::note_type(12, 12, 3);
      ch1_sub2(); M::note("B_", 2); M::note("G_", 2);
      M::octave(3); M::note("C#", 4);
      M::octave(2); M::note("G_", 2); M::note("A_", 4); M::note("F#", 2);

      ch1_sub2();
      M::note("A#", 2); M::note("G_", 2); M::note("B_", 4);
      M::note("G_", 2); M::note("F#", 2); M::note("G_", 2); M::note("D#", 2);

      M::note_type(12, 12, 2);
      ch1_sub3(); M::octave(3); M::note("B_", 3);
      ch1_sub3(); M::note("C_", 3);
      ch1_sub3(); M::note("C#", 3);

      M::note_type(12, 12, 7);
      M::octave(3);
      M::note("F#", 8); M::note("D#", 8);

      M::note_type(12, 12, 2);
      ch1_sub1();
      ch1_sub1();
    }
  }

  void ch1_sub1() {
    M::note("E_", 1); M::note("D#", 1); M::note("B_", 1); M::note("A#", 1);
    M::note("G_", 1); M::note("G#", 1); M::rest(1); M::note("A#", 1);
    M::note("E_", 1); M::note("D#", 1); M::note("B_", 1); M::note("A#", 1);
    M::note("G_", 1); M::note("G#", 1); M::rest(1); M::note("A#", 1);
  }

  void ch1_sub2() {
    M::octave(2);
    M::note("G_", 2); M::note("A#", 4); M::note("G_", 2);
    M::octave(3); M::note("C#", 4);
    M::octave(2); M::note("G_", 2); M::note("A#", 2);
  }

  void ch1_sub3() {
    M::octave(3);
    M::note("E_", 1); M::note("E_", 1); M::note("F_", 1); M::note("E_", 1);
    M::note("G_", 1); M::note("E_", 1); M::note("A_", 1); M::note("E_", 1);
    M::note("A#", 1); M::note("E_", 1); M::note("B_", 1); M::note("E_", 1);
    M::octave(4); M::note("C_", 1);
  }

  void ch1_sub4() {
    M::note("E_", 1); M::note("E_", 1);
    M::rest(4);
    M::octave(1); M::note("E_", 1);
    M::rest(3);
    M::note("E_", 1);
    M::rest(1);
  }

  // ---------------- CH2 ----------------
  void ch2_sub1();
  void ch2_sub2();
  void ch2_sub3();
  void ch2_sub4();
  void ch2_sub5();
  void ch2_sub6();

  void Music_Dungeon1_Ch2() {
    M::vibrato(11, 1, 5);
    M::duty_cycle(3);
    M::note_type(12, 0, -7);
    M::octave(4); M::note("G#", 8);
    M::note_type(12, 4, -5);
    M::octave(5); M::note("D_", 8);

  mainloop:
    while (true) {
      M::note_type(12, 12, 2);

      for (int i = 0; i < 3; i++) ch2_sub1();

      M::octave(2);
      M::note("E_", 1); M::note("E_", 1); M::note("B_", 1);
      M::octave(3); M::note("C_", 1);
      M::rest(2); M::note("C_", 1);
      M::octave(2); M::note("B_", 1); M::note("E_", 1);
      M::note("E_", 1); M::note("A_", 1); M::note("G_", 1);
      M::note("F_", 1); M::note("F#", 1); M::note("G_", 1); M::note("D#", 1);

      for (int i = 0; i < 4; i++) ch2_sub1();

      M::rest(2);
      ch2_sub2();
      M::octave(3); M::note("C_", 4);
      ch2_sub2(); M::note("C#", 4);
      ch2_sub2(); M::note("D#", 4);

      M::note_type(12, 13, 3);
      M::octave(4);
      M::note("E_", 4); M::note("D#", 4); M::note("C_", 4); M::note("B_", 4);

      M::note_type(12, 13, 2);
      ch2_sub3(); M::note("F#", 4);
      ch2_sub3(); M::note("G_", 4);
      ch2_sub3(); M::note("A#", 4);

      M::note_type(12, 13, 3);
      M::octave(4);
      M::note("E_", 4); M::note("D#", 4); M::note("C_", 4); M::note("B_", 4);
      M::note("C_", 4); M::note("B_", 4); M::note("G_", 4); M::note("F#", 2);

      M::note_type(12, 13, 6);
      M::note("E_", 8); M::note("F_", 8); M::note("F#", 8); M::note("G_", 8);

      M::note_type(12, 13, 2);
      ch2_sub4(); M::rest(10); M::note("E_", 1); M::rest(1);
      ch2_sub4(); M::rest(12);
      ch2_sub4(); M::rest(12);
      ch2_sub4(); M::rest(14);

      M::note_type(12, 13, 3);
      ch2_sub6(); M::note("D#", 4);
      ch2_sub6(); M::note("D#", 2); M::rest(2);

      M::note_type(12, 13, 2);
      ch2_sub5(); M::rest(2);
      M::octave(3); M::note("F#", 4);

      M::octave(2); M::note("E_", 1); M::note("E_", 1);
      M::octave(3); M::note("B_", 1); M::rest(1);
      M::octave(4); M::note("C_", 1); M::rest(1);
      M::note("C#", 1); M::rest(1);
      M::note("D_", 1); M::note("C#", 1); M::rest(2);

      M::octave(3); M::note("G_", 4);

      ch2_sub5();
      M::octave(3); M::note("A#", 4);

      M::note_type(12, 13, 6);
      M::note("B_", 8); M::octave(4); M::note("D#", 8);

      M::note_type(12, 13, 2);
      ch2_sub1(); ch2_sub1();
    }
  }

  void ch2_sub1() { M::octave(2); M::note("E_", 1); M::note("E_", 1); M::note("B_", 1); M::octave(3); M::note("C_", 1); M::rest(2); M::note("C_", 1); M::octave(2); M::note("B_", 1); }
  void ch2_sub2() { M::octave(2); M::note("E_", 1); M::note("E_", 1); M::rest(2); M::octave(1); M::note("E_", 1); M::rest(3); M::note("E_", 1); M::rest(3); }
  void ch2_sub3() { M::octave(2); M::note("E_", 1); M::note("E_", 1); M::rest(10); }
  void ch2_sub4() { M::octave(1); M::note("E_", 1); M::note("G_", 1); M::note("E_", 1); M::note("D#", 1); }
  void ch2_sub5() { M::octave(2); M::note("E_", 1); M::note("E_", 1); M::octave(4); M::note("E_", 1); M::rest(1); M::note("D#", 1); M::rest(1); M::note("D_", 1); M::rest(1); M::note("C#", 1); M::note("C_", 1); }
  void ch2_sub6() { M::octave(3); M::note("E_", 6); M::note("G_", 6); M::note("E_", 4); M::note("A_", 6); M::octave(4); M::note("C_", 6); M::octave(3); M::note("B_", 4); M::note("G_", 6); M::note("A#", 6); M::note("F#", 4); M::note("E_", 6); M::note("F#", 6); }

  // ---------------- CH3 ----------------
  void ch3_sub1();
  void ch3_sub2();
  void ch3_sub3();
  void ch3_sub4();
  void ch3_sub5();

  void Music_Dungeon1_Ch3() {
    M::note_type(12, 1, 1);
    M::vibrato(8, 2, 6);
    M::rest(14);
    M::octave(4); M::note("D_", 1); M::note("D#", 1);

  mainloop:
    while (true) {
      ch3_sub1(); ch3_sub1();
      M::octave(5); M::note("E_", 2); M::rest(4);
      M::octave(3); M::note("E_", 1); M::rest(3);
      M::note("E_", 1); M::rest(1);
      M::octave(4); M::note("F#", 4);

      ch3_sub2();
      M::octave(4); M::note("G_", 4);
      ch3_sub2();
      M::octave(4); M::note("A_", 4);

      M::note("B_", 4); M::note("A#", 4); M::note("G_", 4); M::note("F#", 4);
      M::rest(6);

      ch3_sub5();
      M::note("B_", 4); M::note("A#", 4); M::note("G_", 4); M::note("F#", 4);
      M::note("G_", 4); M::note("F#", 4); M::note("E_", 4); M::note("D#", 4);

      ch3_sub3(); M::rest(12);
      ch3_sub3(); M::rest(12);
      ch3_sub3(); M::rest(10); M::note("E_", 1); M::rest(1);

      ch3_sub3(); M::rest(12);
      ch3_sub3(); M::rest(12);
      ch3_sub3(); M::rest(10); M::note("D_", 1); M::note("D#", 1);

      ch3_sub4(); ch3_sub4();
      M::note("E_", 2); M::rest(4);

      ch3_sub5(); M::note("B_", 8); M::note("F#", 6);
      M::note("D_", 1); M::note("D#", 1);

      M::rest(16); M::rest(16);
    }
  }

  void ch3_sub1() { M::octave(5); M::note("E_", 4); M::note("B_", 4); M::note("A#", 4); M::octave(6); M::note("D_", 4); M::note("C#", 4); M::octave(5); M::note("G#", 4); M::note("G_", 4); M::note("B_", 4); M::note("A#", 4); M::note("E_", 4); M::note("D#", 4); M::note("A_", 4); M::note("G#", 4); M::note("E_", 4); M::note("F#", 4); M::note("D#", 4); }
  void ch3_sub2() { M::octave(6); M::note("E_", 1); M::note("E_", 1); M::rest(4); M::octave(3); M::note("E_", 1); M::rest(3); M::note("E_", 1); M::rest(1); }
  void ch3_sub3() { M::octave(3); M::note("E_", 1); M::note("G_", 1); M::note("E_", 1); M::note("D#", 1); }
  void ch3_sub4() { M::note("E_", 4); M::note("B_", 4); M::note("A#", 4); M::octave(4); M::note("D_", 4); M::note("C#", 4); M::octave(3); M::note("G#", 4); M::note("G_", 4); M::note("B_", 4); M::note("A#", 4); M::note("E_", 4); M::note("D#", 4); M::note("A_", 4); M::note("G#", 4); M::note("E_", 4); M::note("F#", 4); M::note("D#", 4); }
  void ch3_sub5() { M::octave(4); M::note("E_", 1); M::rest(3); M::note("E_", 1); M::rest(1); M::note("F#", 4); M::rest(6); M::note("E_", 1); M::rest(3); M::note("E_", 1); M::rest(1); M::note("G_", 4); M::rest(6); M::note("E_", 1); M::rest(3); M::note("E_", 1); M::note("E_", 1); M::note("A#", 4); }

  // ---------------- CH4 ----------------
  void ch4_sub1();
  void ch4_sub2();
  void ch4_sub3();

  void Music_Dungeon1_Ch4() {
    M::rest(14);
    // drum init
    M::rest(0);

  mainloop:
    while (true) {
      for (int i = 0; i < 3; i++) ch4_sub1();

      M::rest(0);
      for (int i = 0; i < 3; i++) ch4_sub1();

      M::rest(0);
      for (int i = 0; i < 3; i++) ch4_sub2();

      ch4_sub3();

      for (int i = 0; i < 3; i++) ch4_sub2();

      ch4_sub3();

      M::rest(2);

      for (int i = 0; i < 13; i++) M::rest(16);

      M::rest(12);

      for (int i = 0; i < 3; i++) ch4_sub2();

      ch4_sub1(); ch4_sub1();
    }
  }

  void ch4_sub1() { /* drum 13,4 12,4 13,4 12,4 */ }
  void ch4_sub2() { /* drum 12,1 12,1 rest10 14 */ }
  void ch4_sub3() { /* drum 9,4 10,4 10,4 11,4 */ }

} // namespace