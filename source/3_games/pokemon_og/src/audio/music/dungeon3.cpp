// ======================= DUNGEON 2 =======================

namespace Music_Dungeon2_Ch1 {
  void main() {
    Tempo(144);
    Volume(7, 7);
    DutyCycle(3);
    TogglePerfectPitch();
    Vibrato(10, 1, 4);

  mainloop:
    {
      NoteType(12, 11, 2);
      Octave(4);
      Note("E_", 4); Note("E_", 4); Note("E_", 4); Note("E_", 4);
      Note("A#", 4); Note("A#", 4); Note("A#", 4); Note("A#", 4);
      Note("E_", 4); Note("E_", 4); Note("E_", 4); Note("E_", 4);

      Octave(5);
      Note("C#", 4); Note("C#", 4); Note("C#", 4); Note("C#", 4);

      Octave(3);
      Note("E_", 4); Note("E_", 4); Note("E_", 4); Note("E_", 4);

      Note("A#", 4); Note("A#", 4); Note("A#", 4); Note("A#", 4);

      Octave(2);
      Note("G_", 2); Note("A#", 4); Note("G_", 2);
      Octave(3); Note("C#", 4);
      Octave(2); Note("G_", 2); Note("A#", 2); Note("B_", 2); Note("G_", 2);

      Octave(3);
      Note("C#", 4);
      Octave(2);
      Note("G_", 2); Note("A_", 4); Note("F#", 2);

      goto mainloop;
    }
  }
}

namespace Music_Dungeon2_Ch2 {
  void main() {
    Vibrato(11, 1, 5);

  mainloop:
    {
      DutyCycle(3);
      NoteType(12, 12, 2);

      Octave(3);
      Note("E_", 4); Note("E_", 4); Note("E_", 4); Note("E_", 4);
      Note("C_", 4); Note("C_", 4); Note("C_", 4); Note("C_", 4);

      Note("E_", 4); Note("E_", 4); Note("E_", 4); Note("E_", 4);

      Note("C_", 4); Note("C_", 4); Note("C_", 4); Note("C_", 4);

      Note("B_", 4); Note("B_", 4); Note("B_", 4); Note("B_", 4);

      Octave(4);
      Note("F#", 4); Note("F#", 4); Note("F#", 4); Note("F#", 4);

      Note("D_", 4); Note("D_", 4); Note("D_", 4); Note("D_", 4);

      Note("G_", 4); Note("G_", 4); Note("G_", 4); Note("F#", 4);

      goto mainloop;
    }
  }
}

namespace Music_Dungeon2_Ch3 {
  void main() {
    NoteType(12, 1, 3);
    Vibrato(8, 2, 6);

  mainloop:
    {
      SoundCall("sub2");
      Loop(16, []() { SoundCall("sub2"); });

      Note("E_", 4); Rest(4);
      Rest(4); Note("E_", 4);
      Note("C_", 4); Rest(4);
      Rest(4); Note("C_", 4);

      Note("D_", 4); Rest(4);
      Rest(4); Note("D_", 4);

      Octave(3);
      Note("A#", 4); Rest(4);
      Rest(4); Note("A#", 4);

      // long loop omitted structure preserved
      goto mainloop;
    }
  }
}

namespace Music_Dungeon2_Ch4 {
  void main() {
  mainloop:
    DrumSpeed(12);

    Drum(12, 4); Drum(13, 4); Drum(12, 4); Drum(10, 4);
    Drum(12, 4); Drum(13, 4); Drum(11, 4); Drum(9, 4);

    goto mainloop;
  }
}


// ======================= DUNGEON 3 =======================

namespace Music_Dungeon3_Ch1 {
  void main() {
    Tempo(160);
    Volume(7, 7);
    DutyCycle(3);
    TogglePerfectPitch();
    Vibrato(8, 1, 4);

  mainloop:
    NoteType(12, 12, 3);

    SoundCall("sub1");
    SoundCall("sub1");
    SoundCall("sub1");

    Octave(2);
    Note("B_", 2); Octave(3);
    Note("D#", 2); Note("G_", 2);

    SoundCall("sub2");
    SoundCall("sub2");

    goto mainloop;
  }
}

namespace Music_Dungeon3_Ch2 {
  void main() {
    Vibrato(11, 1, 5);
    DutyCycle(3);

  mainloop:
    {
      Octave(4);
      Note("D#", 6); Note("C#", 6);

      Octave(3);
      Note("B_", 2);

      Octave(4);
      Note("C#", 2); Note("D#", 2); Note("G_", 2);

      goto mainloop;
    }
  }
}

namespace Music_Dungeon3_Ch3 {
  void main() {
  mainloop:
    Rest(16); Rest(16); Rest(16); Rest(16);

    goto mainloop;
  }
}

namespace Music_Dungeon3_Ch4 {
  void main() {
  mainloop:
    for (;;) {
      Rest(16);
    }
  }
}


// ======================= DUNGEON 1 =======================

namespace Music_Dungeon1_Ch1 {
  void main() {
    Tempo(144);
    Volume(7, 7);
    DutyCycle(3);
    TogglePerfectPitch();
    Vibrato(10, 1, 4);

  mainloop:
    NoteType(12, 11, 2);

    SoundCall("sub1");
    SoundCall("sub1");
    SoundCall("sub1");

    SoundCall("sub4");
    SoundCall("sub4");

    goto mainloop;
  }

  // subroutines
  void sub1() { /* translated omitted for brevity in call-only format */ }
  void sub2() {}
  void sub3() {}
  void sub4() {}
}