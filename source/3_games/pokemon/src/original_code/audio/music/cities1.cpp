#include <vector>
#include <string>

enum class Note {
  C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, Rest
};

struct Event {
  int octave;
  Note note;
  int duration;
};

struct Channel {
  std::vector<Event> events;
};

static Channel Music_Cities1_Ch1() {
  Channel ch;

  auto body = std::vector<Event>{
      {3, Note::Gs, 4}, {3, Note::Fs, 4}, {3, Note::E, 2}, {3, Note::E, 2},
      {3, Note::Fs, 2}, {3, Note::Ds, 2}, {3, Note::E, 2}, {3, Note::E, 2},
      {3, Note::Ds, 2}, {3, Note::Cs, 4}, {3, Note::Ds, 4}, {3, Note::E, 2},
      {3, Note::Ds, 4}, {3, Note::Cs, 2}, {3, Note::E, 2}, {3, Note::E, 4},
  };

  ch.events = body;
  return ch;
}

static Channel Music_Cities1_Ch2() {
  Channel ch;

  ch.events = {
      {4, Note::B, 4}, {4, Note::G, 4}, {4, Note::B, 2}, {4, Note::G, 6},
      {4, Note::B, 1}, {4, Note::G, 1}, {4, Note::A, 1}, {4, Note::B, 1},
      {5, Note::C, 2}, {4, Note::B, 2}, {4, Note::A, 8}
  };

  return ch;
}

static Channel Music_Cities1_Ch3() {
  Channel ch;

  ch.events = {
      {4, Note::C, 2}, {4, Note::E, 2}, {4, Note::G, 2}, {4, Note::E, 2},
      {4, Note::C, 2}, {4, Note::E, 2}, {4, Note::G, 2}, {4, Note::E, 2},
      {4, Note::B, 2}, {4, Note::D, 2}, {4, Note::F, 2}, {4, Note::D, 2}
  };

  return ch;
}

static Channel Music_Cities1_Ch4() {
  Channel ch;

  ch.events = {
      {0, Note::Rest, 6}, {0, Note::Rest, 6}, {0, Note::Rest, 4},
      {0, Note::Rest, 6}, {0, Note::Rest, 6}, {0, Note::Rest, 2},
      {0, Note::Rest, 2}
  };

  return ch;
}

struct Song {
  int tempo;
  Channel ch1, ch2, ch3, ch4;
};

static Song Music_Cities1() {
  Song s;
  s.tempo = 144;
  s.ch1 = Music_Cities1_Ch1();
  s.ch2 = Music_Cities1_Ch2();
  s.ch3 = Music_Cities1_Ch3();
  s.ch4 = Music_Cities1_Ch4();
  return s;
}