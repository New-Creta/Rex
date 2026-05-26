// 1:1 style C++ translation of the assembly music data (Routes3)
// Assumed audio engine API: Note(), Rest(), Octave(), DutyCycle(),
// Vibrato(), NoteType(), DrumNote(), SoundCall(), SoundRet(), SoundLoop()

#include "AudioEngine.h"

void Music_Routes3_Ch1()
{
  Tempo(148);
  Volume(7, 7);
  DutyCycle(3);
  Vibrato(6, 3, 4);
  TogglePerfectPitch();

  NoteType(12, 11, 5);
  Octave(3);
  Note(E_, 1);
  Note(Fs, 1);
  Note(G_, 6);
  Note(F_, 1);
  Note(G_, 1);
  Note(E_, 1);

  Rest(16);
  Rest(15);

mainloop:
  NoteType(12, 11, 5);
  Note(E_, 6);
  Note(D_, 1);
  Note(E_, 1);
  Note(C_, 4);
  Note(E_, 4);
  Note(C_, 6);
  Note(D_, 1);
  Note(E_, 1);
  Note(F_, 2);
  Note(G_, 2);
  Note(G_, 2);
  Note(A_, 2);

  NoteType(12, 10, 7);
  Note(As, 8);
  Note(F_, 8);
  Note(D_, 8);
  Note(F_, 8);

  NoteType(12, 11, 5);
  Note(E_, 6);
  Note(D_, 1);
  Note(E_, 1);
  Note(C_, 4);
  Note(E_, 4);
  Note(C_, 6);
  Note(D_, 1);
  Note(E_, 1);
  Note(F_, 2);
  Note(G_, 2);
  Note(G_, 2);
  Note(A_, 2);

  NoteType(12, 10, 7);
  Note(As, 8);
  Note(As, 8);
  Note(D_, 8);
  Note(F_, 8);

  NoteType(12, 11, 5);
  Note(E_, 4);
  Note(E_, 2);
  Note(F_, 2);
  Note(G_, 4);
  Note(F_, 2);
  Note(E_, 2);
  Note(B_, 2);

  Octave(2);
  Note(G_, 4);

  Octave(3);
  Note(B_, 8);
  Note(A_, 2);

  NoteType(8, 12, 3);
  Note(A_, 4);
  Note(F_, 4);
  Note(A_, 4);

  NoteType(8, 4, -7);
  Note(A_, 12);

  NoteType(8, 9, 0);
  Note(F_, 12);
  Note(G_, 12);

  goto mainloop;
}

void Music_Routes3_Ch2()
{
  Vibrato(8, 2, 3);
  DutyCycle(2);

  NoteType(12, 12, 7);
  Octave(3);
  Note(G_, 1);
  Note(As, 1);
  Note(B_, 6);
  Note(A_, 1);
  Note(B_, 1);

  Octave(4);
  Note(C_, 1);

  Rest(15);
  Rest(16);

mainloop:
  NoteType(12, 12, 7);
  DutyCycle(2);

  Octave(4);
  Note(C_, 6);
  Octave(3);
  Note(G_, 1);
  Octave(4);
  Note(C_, 1);
  Note(E_, 10);

  Octave(3);
  Note(G_, 2);
  Octave(4);
  Note(C_, 2);
  Note(G_, 2);
  Note(F_, 2);
  Note(E_, 2);
  Note(D_, 2);
  Note(C_, 2);

  Note(D_, 8);
  Note(F_, 8);

  NoteType(12, 12, 5);
  DutyCycle(3);
  Octave(3);
  Note(As, 8);
  Note(A_, 8);

  NoteType(12, 12, 7);
  DutyCycle(2);
  Octave(4);
  Note(C_, 6);
  Octave(3);
  Note(G_, 1);
  Octave(4);
  Note(C_, 1);
  Note(E_, 10);

  Octave(3);
  Note(G_, 2);
  Octave(4);
  Note(C_, 2);
  Note(G_, 2);
  Note(F_, 2);
  Note(E_, 2);
  Note(D_, 2);
  Note(C_, 2);

  Note(D_, 8);
  Note(F_, 8);

  NoteType(12, 12, 5);
  DutyCycle(3);
  Octave(3);
  Note(As, 8);

  Octave(4);
  Note(D_, 6);

  NoteType(12, 12, 7);
  DutyCycle(2);

  Note(C_, 1);
  Note(D_, 1);
  Note(E_, 2);
  Note(D_, 2);
  Note(E_, 2);
  Note(C_, 8);

  Octave(3);
  Note(B_, 1);
  Octave(4);
  Note(C_, 1);
  Note(D_, 2);

  Octave(3);
  Note(G_, 4);

  Octave(4);
  Note(G_, 8);
  Note(F_, 1);
  Note(E_, 1);

  NoteType(8, 13, 3);
  Note(F_, 4);
  Note(E_, 4);

  NoteType(8, 12, 4);
  Note(C_, 4);

  NoteType(8, 12, 5);
  Note(C_, 12);

  NoteType(12, 10, 0);
  DutyCycle(3);
  Octave(3);
  Note(A_, 8);
  Note(B_, 8);

  goto mainloop;
}

void Music_Routes3_Ch3()
{
  Vibrato(4, 1, 0);
  NoteType(6, 1, 2);

  Octave(4);
  Note(G_, 2);
  Note(As, 2);
  Note(B_, 8);
  Note(A_, 8);
  Note(G_, 2);

  Rest(2);
  Note(G_, 7);
  Rest(1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 2);
  Rest(2);
  Note(G_, 2);
  Rest(2);
  Note(G_, 8);

  Note(G_, 2);
  Rest(2);
  Note(G_, 7);
  Rest(1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 2);
  Rest(2);
  Note(G_, 2);
  Rest(2);
  Note(G_, 8);

mainloop:
  NoteType(12, 1, 2);

  Note(E_, 1);
  Rest(1);
  Note(G_, 4);
  Note(E_, 1);
  Note(E_, 1);
  Note(E_, 1);
  Rest(1);
  Note(E_, 1);
  Rest(1);
  Note(G_, 4);

  Note(E_, 1);
  Rest(1);
  Note(G_, 4);

  Note(E_, 1);
  Note(E_, 1);
  Note(E_, 1);
  Rest(1);
  Note(E_, 1);
  Rest(1);
  Note(G_, 2);
  Note(A_, 2);

  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);
  Note(As, 2);
  Note(F_, 2);

  Note(E_, 1);
  Rest(1);
  Note(G_, 4);

  Note(E_, 1);
  Note(E_, 1);
  Note(E_, 1);
  Rest(1);
  Note(E_, 1);
  Rest(1);
  Note(G_, 4);

  Note(E_, 1);
  Rest(1);
  Note(G_, 4);

  Note(E_, 1);
  Note(E_, 1);
  Note(E_, 1);
  Rest(1);
  Note(E_, 1);
  Rest(1);
  Note(G_, 2);
  Note(A_, 2);

  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Rest(1);
  Note(As, 4);

  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);
  Note(As, 2);
  Note(A_, 2);

  Note(G_, 1);
  Rest(1);

  Octave(5);
  Note(C_, 4);

  Octave(4);
  Note(G_, 1);
  Note(G_, 1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 1);
  Rest(1);

  Octave(5);
  Note(C_, 4);

  Octave(4);
  Note(G_, 1);
  Rest(1);

  Octave(5);
  Note(D_, 4);

  Octave(4);
  Note(G_, 1);
  Note(G_, 1);
  Note(G_, 1);
  Rest(1);
  Note(G_, 1);
  Rest(1);

  Octave(5);
  Note(D_, 4);

  Octave(4);
  Note(F_, 1);
  Rest(1);

  Octave(5);
  Note(C_, 4);

  Octave(4);
  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);

  Octave(5);
  Note(C_, 4);

  Octave(4);
  Note(F_, 1);
  Rest(1);

  Note(A_, 4);

  Note(F_, 1);
  Note(F_, 1);
  Note(F_, 1);
  Rest(1);
  Note(F_, 1);
  Rest(1);

  Note(A_, 4);

  goto mainloop;
}

void Music_Routes3_Ch4()
{
  DrumSpeed(6);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(17, 8);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(17, 12);

  DrumNote(17, 2);
  DrumNote(18, 2);
  DrumNote(17, 4);
  DrumNote(17, 4);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(17, 12);

  DrumNote(17, 2);
  DrumNote(18, 2);
  DrumNote(17, 4);
  DrumNote(18, 4);
  DrumNote(17, 4);

mainloop:
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(18, 2);
  DrumNote(17, 4);
  DrumNote(17, 4);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 8);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(18, 2);
  DrumNote(17, 4);
  DrumNote(17, 10);

  DrumNote(19, 1);
  DrumNote(19, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(19, 2);
  DrumNote(17, 4);
  DrumNote(17, 4);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 8);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 10);

  DrumNote(19, 1);
  DrumNote(19, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 8);

  DrumNote(18, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(19, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 8);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  DrumNote(17, 12);
  DrumNote(17, 2);
  DrumNote(17, 2);
  DrumNote(17, 4);
  DrumNote(17, 4);

  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(19, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);
  DrumNote(18, 1);

  goto mainloop;
}