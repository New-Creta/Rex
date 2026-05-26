// 1:1 transcription-style C++ representation of Music_SSAnne_*
// Hypothetical audio engine API matching the original commands

#include <cstdint>

// ---------------- Mock audio engine ----------------
void tempo(int v) {}
void volume(int a, int b) {}
void duty_cycle(int v) {}
void vibrato(int a, int b, int c) {}
void toggle_perfect_pitch() {}
void note_type(int a, int b, int c) {}
void octave(int v) {}
void note(const char* n, int len) {}
void rest(int len) {}
void sound_loop(int count, void* label) {}

// ---------------- CH1 ----------------
struct Music_SSAnne_Ch1 {

  static void mainloop();

  static void run() {
    tempo(128);
    volume(7, 7);
    duty_cycle(1);
    vibrato(8, 3, 4);
    toggle_perfect_pitch();

    mainloop();
  }
};

void Music_SSAnne_Ch1::mainloop() {
  while (true) {

    note_type(12, 10, 4);
    rest(12);

    octave(3);
    note("E_", 2);
    note("F#", 2);
    note("E_", 4);
    note("D_", 4);
    note("C#", 2);
    note("D_", 2);
    note("E_", 2);
    rest(2);

    note("E_", 4);
    note("F#", 2);
    note("D_", 2);
    note("E_", 4);
    note("F#", 4);
    note("G#", 4);
    rest(4);

    note("E_", 4);
    note("G#", 4);
    rest(4);

    note("C#", 2);
    note("E_", 2);
    note("F#", 4);
    note("D_", 4);
    note("E_", 4);
    note("D_", 4);
    note("C#", 2);
    note("D_", 2);
    note("E_", 2);
    rest(2);

    note("E_", 4);
    note("F#", 2);
    note("D_", 2);
    note("E_", 4);
    note("F#", 4);
    note("G#", 4);
    note("F#", 4);
    note("E_", 4);
    note("F#", 4);
    rest(4);

    note("C#", 2);
    note("E_", 2);
    note("F#", 4);
    note("D_", 4);
    note("E_", 4);
    note("D_", 4);
    note("C#", 2);
    note("D_", 2);
    note("E_", 2);
    rest(2);

    note("E_", 4);
    note("F#", 2);
    note("D_", 2);
    note("E_", 4);
    note("F#", 4);
    note("G#", 4);
    note("F#", 8);
    note("G#", 4);
    note("E_", 4);
    rest(4);

    note("E_", 4);
    note("F#", 2);
    note("E_", 2);
    note("D_", 4);
    rest(4);

    note("D_", 4);
    note("E_", 4);
    note("C#", 4);
    rest(4);

    note("C#", 4);
    note("D_", 2);
    note("C#", 2);

    octave(2);
    note("B_", 4);
    note("E_", 4);
    note("B_", 4);

    octave(3);
    note("D_", 2);

    octave(2);
    note("B_", 2);
    note("A_", 4);
    rest(4);

    note("A_", 4);
    note("B_", 2);
    note("A_", 2);
    note("G#", 4);
    rest(4);

    note("C#", 4);
    note("G#", 4);
    note("A_", 4);
    rest(4);

    note("A_", 4);
    note("B_", 4);
    note("B_", 4);
    rest(4);

    octave(3);
    note("D_", 8);

    sound_loop(0, nullptr);
  }
}

// ---------------- CH2 ----------------
struct Music_SSAnne_Ch2 {

  static void mainloop();

  static void run() {
    duty_cycle(0);
    vibrato(12, 2, 4);

    mainloop();
  }
};

void Music_SSAnne_Ch2::mainloop() {
  while (true) {

    note_type(12, 12, 5);

    octave(3);
    note("A_", 2);
    note("E_", 2);
    note("A_", 4);
    note("B_", 4);

    octave(4);
    note("D_", 4);

    note_type(8, 12, 5);
    note("C#", 2);
    note("D_", 2);
    note("C#", 2);

    octave(3);
    note("B_", 3);
    note("A_", 3);
    note("B_", 6);
    note("G#", 6);

    note("A_", 3);
    note("B_", 3);

    octave(4);
    note("C#", 6);
    note("D_", 6);

    note("C#", 3);

    octave(3);
    note("B_", 3);

    octave(4);
    note("E_", 6);
    note("D_", 3);
    note("C#", 3);

    octave(3);
    note("B_", 6);

    octave(4);
    note("C#", 3);

    octave(3);
    note("B_", 3);
    note("A_", 3);
    note("E_", 3);
    note("A_", 6);
    note("B_", 6);

    octave(4);
    note("D_", 6);

    note("C#", 2);
    note("D_", 2);
    note("C#", 2);

    octave(3);
    note("B_", 3);
    note("A_", 3);
    note("B_", 6);
    note("G#", 6);

    note("A_", 3);
    note("B_", 3);

    octave(4);
    note("C#", 6);
    note("D_", 6);

    note("C#", 3);

    octave(3);
    note("B_", 3);

    octave(4);
    note("E_", 6);
    note("D_", 3);
    note("C#", 3);

    octave(3);
    note("B_", 12);

    sound_loop(0, nullptr);
  }
}

// ---------------- CH3 ----------------
struct Music_SSAnne_Ch3 {

  static void mainloop();

  static void run() {
    mainloop();
  }
};

void Music_SSAnne_Ch3::mainloop() {
  while (true) {

    note_type(12, 1, 3);

    rest(16);
    rest(12);

    octave(4);
    note("B_", 2);
    note("G#", 2);
    note("A_", 8);
    note("F#", 8);

    note("G#", 4);
    rest(4);

    note("B_", 8);
    note("A_", 8);
    note("B_", 8);
    note("A_", 8);
    note("G#", 8);
    note("A_", 8);
    note("F#", 8);
    note("G#", 8);

    note("B_", 8);
    note("A_", 4);
    note("E_", 4);
    note("B_", 4);
    note("E_", 4);

    note("A_", 4);
    note("E_", 4);
    note("G#", 4);
    note("E_", 4);

    note("A_", 4);
    note("E_", 4);
    note("F#", 4);
    note("G_", 4);
    note("G#", 4);
    note("A_", 4);

    octave(5);
    note("D_", 8);
    note("C#", 8);
    note("C#", 8);

    octave(4);
    note("B_", 8);
    note("B_", 8);
    note("A_", 8);
    note("A_", 8);
    note("G#", 8);
    note("G#", 8);
    note("F#", 8);
    note("F#", 8);
    note("E_", 8);
    note("E_", 8);
    note("F#", 8);
    note("F#", 8);
    note("G#", 8);
    note("B_", 8);

    sound_loop(0, nullptr);
  }
}