// 1:1 transcription-style conversion of the provided music data into C++-like structure.
// This assumes a hypothetical audio engine API that mirrors the original assembly commands.

#include <cstdint>

// -------------------- Mock audio engine API --------------------
void tempo(int v) {}
void volume(int a, int b) {}
void duty_cycle(int v) {}
void vibrato(int a, int b, int c) {}
void toggle_perfect_pitch() {}
void note_type(int len, int vel, int bend) {}
void octave(int v) {}
void note(const char* n, int len) {}
void rest(int len) {}
void pitch_slide(int a, int b, const char* n) {}
void sound_ret() {}
void sound_loop(int count, void* label) {}

// Forward declarations (subroutines)
struct Music_SilphCo_Ch1_t;
void SilphCo_Ch1_sub1();

// -------------------- CH1 --------------------
struct Music_SilphCo_Ch1_t {

  static void mainloop();

  static void run() {
    tempo(160);
    volume(7, 7);
    duty_cycle(3);
    toggle_perfect_pitch();
    vibrato(8, 2, 2);

    note_type(6, 11, 3);
    octave(2);

    note("E_", 1);
    note("F#", 1);
    note("G#", 1);
    note("A#", 1);
    octave(3);
    note("C_", 1);

    note_type(6, 10, 0);
    note("C_", 12);

    note_type(6, 11, 3);
    note("C_", 2);

    octave(2);
    note("B_", 1);
    note("A#", 1);
    note("G#", 1);
    note("F#", 1);
    note("E_", 1);
    note("E_", 2);

    rest(2);
    note("B_", 2);
    rest(6);
    note("B_", 2);
    rest(6);
    note("B_", 2);
    rest(6);
    note("B_", 2);
    rest(2);

    duty_cycle(3);

    mainloop();
  }

};

void SilphCo_Ch1_sub1() {
  note_type(6, 11, 3);
  octave(2);
  note("B_", 2);
  note("A#", 2);
  note("B_", 2);
  octave(3);
  note("C_", 2);
  octave(2);
  note("B_", 2);
  note("A#", 2);
  note("B_", 2);
  octave(3);
  note("C_", 2);
  sound_ret();
}

void Music_SilphCo_Ch1_t::mainloop() {

  while (true) {

    note("G_", 2);
    rest(2);
    note("B_", 2);
    rest(6);
    note("B_", 2);
    rest(6);
    note("A#", 2);
    rest(6);
    note("B_", 2);
    rest(6);

    note("B_", 2);
    rest(6);
    note("B_", 2);
    rest(2);

    octave(3);
    note("C_", 2);
    rest(2);

    note("C_", 2);
    rest(6);
    note("C_", 2);
    rest(6);
    note("C_", 2);
    rest(6);
    note("C_", 2);
    rest(6);

    octave(2);
    note("A#", 2);
    rest(6);
    note("B_", 2);
    rest(2);
    note("G_", 2);
    rest(10);

    note("E_", 1);
    note("F#", 1);
    note("G#", 1);
    note("A_", 1);

    note_type(6, 10, 0);
    note("B_", 12);

    note_type(6, 11, 3);
    note("B_", 1);
    note("A_", 1);
    note("G_", 1);
    note("F_", 1);
    note("E_", 1);
    rest(11);

    note("F_", 1);
    note("G#", 1);
    note("A#", 1);
    note("B_", 1);

    note_type(6, 10, 0);
    octave(3);
    note("C_", 12);

    note_type(6, 11, 3);
    note("C_", 1);

    octave(2);
    note("B_", 1);
    note("A_", 1);
    note("G_", 1);
    note("F_", 1);
    rest(11);

    note("F#", 1);
    note("B_", 1);

    octave(3);
    note("C_", 1);
    note("C#", 1);

    note_type(6, 10, 0);
    note("D_", 12);

    note_type(6, 11, 3);
    note("D_", 1);
    note("C_", 1);

    octave(2);
    note("A#", 1);
    note("G_", 1);
    note("F_", 1);
    rest(11);

    note("F_", 1);
    note("G#", 1);
    note("A#", 1);
    note("B_", 1);

    note_type(6, 10, 0);
    octave(3);
    note("C_", 12);

    note_type(6, 11, 3);
    note("C_", 1);

    octave(2);
    note("B_", 1);
    note("G#", 1);
    note("F_", 1);
    note("E_", 1);
    rest(7);

    note_type(6, 8, 0);
    note("B_", 16);

    note_type(6, 11, 0);
    octave(3);
    note("E_", 8);

    rest(4);
    rest(4);

    return;
  }
}

// -------------------- CH2 --------------------
struct Music_SilphCo_Ch2_t {

  static void mainloop();

  static void run() {
    vibrato(10, 3, 2);
    duty_cycle(3);

    note_type(6, 12, 3);
    octave(3);
    note("E_", 1);
    note("F#", 1);
    note("G#", 1);
    note("A#", 1);
    octave(4);
    note("C_", 1);

    note_type(6, 11, 0);
    note("C_", 12);

    note_type(6, 12, 3);
    note("C_", 2);

    octave(3);
    note("B_", 1);
    note("A#", 1);
    note("G#", 1);
    note("F#", 1);
    note("E_", 1);

    note("E_", 2);
    rest(2);

    octave(2);
    note("G_", 2);
    rest(6);
    note("F#", 2);
    rest(6);
    note("G_", 2);
    rest(6);
    note("G#", 2);
    rest(2);

    mainloop();
  }
};

void Music_SilphCo_Ch2_t::mainloop() {

  while (true) {

    note("E_", 2);
    rest(2);
    note("G_", 2);
    rest(6);
    note("G_", 2);
    rest(6);

    note("G_", 2);
    rest(6);
    note("F#", 2);
    rest(6);
    note("G_", 2);
    rest(6);
    note("G#", 2);
    rest(2);

    note("E_", 2);
    rest(2);
    note("G#", 2);
    rest(6);

    note("G#", 2);
    rest(6);
    note("G#", 2);
    rest(6);
    note("G#", 2);
    rest(6);
    note("G#", 2);
    rest(6);

    note("G#", 2);
    rest(2);
  }
}

// -------------------- CH3 --------------------
struct Music_SilphCo_Ch3_t {

  static void mainloop();

  static void run() {
    vibrato(8, 1, 1);
    note_type(12, 1, 1);

    rest(12);

    octave(5);
    note("E_", 4);
    note("D#", 4);
    note("E_", 4);
    note("F_", 4);

    mainloop();
  }
};

void Music_SilphCo_Ch3_t::mainloop() {

  while (true) {

    note("E_", 4);
    note("D#", 4);
    note("D_", 4);
    note("D#", 4);
    note("E_", 4);
    note("F_", 4);
    note("F#", 4);
    note("G_", 4);
    note("G#", 4);
    note("D#", 4);
    note("D_", 4);
    note("D#", 4);

    note("E_", 1);
    rest(1);
    note("G_", 1);
    rest(3);
    note("F#", 1);
    rest(3);
    note("G_", 1);
    rest(3);
    note("G#", 1);
    rest(1);
  }
}