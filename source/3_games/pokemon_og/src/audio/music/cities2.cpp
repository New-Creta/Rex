#include <vector>
#include <array>

enum Note {
  C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, Rest
};

struct Event {
  int octave;
  Note note;
  int len;
};

struct Channel {
  std::vector<Event> events;
};

static Channel Music_Cities2_Ch1() {
  Channel ch;

  ch.events = {
      {3, E, 2}, {3, Ds, 2}, {3, Cs, 2}, {3, C, 2}, {2, B, 2},

      {3, B, 4}, {4, Cs, 2}, {4, Ds, 1}, {4, D, 1}, {4, Cs, 4},
      {3, B, 4}, {3, Gs, 8}, {3, E, 8},

      {3, A, 4}, {3, Gs, 2}, {3, Fs, 2}, {3, Gs, 2}, {3, A, 2},
      {3, B, 2}, {4, Cs, 2}
  };

  return ch;
}

static Channel Music_Cities2_Ch2() {
  Channel ch;

  ch.events = {
      {4, E, 2}, {4, Ds, 2}, {4, Cs, 2}, {3, B, 2},
      {3, A, 2}, {3, B, 2}, {4, Cs, 2}, {4, Ds, 2},

      {4, E, 6}, {4, E, 1}, {3, B, 1}, {4, Cs, 2},
      {4, Ds, 2}, {4, E, 1}, {4, Fs, 1}, {4, Gs, 1}, {4, A, 1}
  };

  return ch;
}

static Channel Music_Cities2_Ch3() {
  Channel ch;

  ch.events = {
      {4, E, 2}, {4, Gs, 2}, {4, E, 2}, {4, Gs, 2},
      {4, F, 2}, {4, A, 2}, {4, B, 2}, {4, A, 2}
  };

  return ch;
}

struct Song {
  int tempo;
  Channel ch1, ch2, ch3;
};

static Song Music_Cities2() {
  Song s;
  s.tempo = 148;
  s.ch1 = Music_Cities2_Ch1();
  s.ch2 = Music_Cities2_Ch2();
  s.ch3 = Music_Cities2_Ch3();
  return s;
}