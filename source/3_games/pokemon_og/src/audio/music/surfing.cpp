// 1:1 C++ style transcription of Music_Surfing (no structural merging)

void Music_Surfing_Ch1();

void Music_Surfing_Ch1() {
  tempo(160);
  volume(7, 7);
  duty_cycle(3);
  vibrato(12, 3, 4);
  toggle_perfect_pitch();

  note_type(12, 11, 5);
  rest(6);

  octave(2);
  note("A_"); note_length(2);
  note("G#"); note_length(3);
  note("F#"); note_length(1);
  note("E_"); note_length(6);

  octave(3);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(4);

mainloop:

  note_type(12, 11, 2);
  note("E_"); note_length(2);
  note("E_"); note_length(4);
  note("E_"); note_length(2);
  note("E_"); note_length(4);

  note("D_"); note_length(2);
  note("D_"); note_length(4);
  note("D_"); note_length(2);
  note("D_"); note_length(4);
  note("D_"); note_length(2);
  note("D_"); note_length(4);

  note_type(12, 11, 1);
  note("F#"); note_length(3);

  note_type(12, 11, 2);
  note("D_"); note_length(3);

  note("E_"); note_length(2);
  note("E_"); note_length(4);
  note("E_"); note_length(2);
  note("E_"); note_length(4);

  note("A_"); note_length(2);
  note("A_"); note_length(4);

  note("G#"); note_length(2);
  note("G#"); note_length(4);

  note("F#"); note_length(2);
  note("F#"); note_length(4);

  note("E_"); note_length(2);
  note("E_"); note_length(4);

  note("G#"); note_length(2);
  note("G#"); note_length(4);

  note_type(12, 11, 1);
  note("F#"); note_length(3);

  note_type(12, 11, 2);
  note("G#"); note_length(1);

  note_type(12, 11, 4);
  note("A_"); note_length(2);
  note("B_"); note_length(2);

  octave(4);
  note("C_"); note_length(2);

  note_type(12, 11, 7);
  note("C#"); note_length(6);

  note_type(12, 11, 3);
  note("D_"); note_length(2);
  note("C#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("E_"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("E_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);
  note("A_"); note_length(2);
  note("B_"); note_length(2);

  octave(4);
  note("D_"); note_length(2);
  note("C#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("E_"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("A_"); note_length(2);
  note("E_"); note_length(2);
  note("G#"); note_length(2);
  note("B_"); note_length(2);

  octave(4);
  note("D_"); note_length(2);
  note("C#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("C#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);
  note("A_"); note_length(2);
  note("G#"); note_length(2);
  note("F#"); note_length(2);
  note("E_"); note_length(4);

  goto mainloop;
}

//////////////////////////////////////////////////////////////

void Music_Surfing_Ch2();

void Music_Surfing_Ch2() {
  duty_cycle(3);
  vibrato(16, 2, 5);

  note_type(12, 12, 6);
  octave(3);

  note("A_"); note_length(2);
  note("G#"); note_length(3);
  note("F#"); note_length(1);
  note("E_"); note_length(6);
  note("G#"); note_length(6);

  octave(4);
  note("E_"); note_length(2);
  note("D_"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

mainloop:

  note_type(12, 12, 6);
  octave(4);
  note("E_"); note_length(6);
  note("C#"); note_length(5);
  note("E_"); note_length(1);

  note("D_"); note_length(6);

  octave(3);
  note("B_"); note_length(6);

  octave(4);
  note("D_"); note_length(6);

  octave(3);
  note("B_"); note_length(2);

  note_type(12, 12, 1);
  octave(4);
  note("F#"); note_length(3);

  note_type(12, 12, 6);
  note("D_"); note_length(1);
  note("E_"); note_length(6);
  note("C#"); note_length(6);
  note("E_"); note_length(6);
  note("C#"); note_length(5);
  note("E_"); note_length(1);

  note("F#"); note_length(6);
  note("D_"); note_length(6);
  note("F#"); note_length(6);
  note("E_"); note_length(2);

  note_type(12, 12, 1);
  note("F#"); note_length(3);

  note_type(12, 12, 6);
  note("E_"); note_length(1);
  note("A_"); note_length(6);
  note("A_"); note_length(6);
  note("B_"); note_length(2);
  note("A_"); note_length(3);
  note("D_"); note_length(1);
  note("F#"); note_length(6);
  note("E_"); note_length(2);
  note("F#"); note_length(3);
  note("E_"); note_length(1);
  note("C#"); note_length(6);
  note("D_"); note_length(2);
  note("C#"); note_length(3);

  octave(3);
  note("B_"); note_length(1);

  octave(4);
  note("F#"); note_length(6);
  note("E_"); note_length(2);
  note("D#"); note_length(3);
  note("E_"); note_length(1);

  note("A_"); note_length(6);
  note("B_"); note_length(2);
  note("A_"); note_length(3);
  note("D_"); note_length(1);
  note("F#"); note_length(6);
  note("E_"); note_length(2);
  note("F#"); note_length(3);
  note("G#"); note_length(1);

  note("A_"); note_length(6);
  note("G#"); note_length(2);
  note("F#"); note_length(3);
  note("E_"); note_length(1);

  note("G#"); note_length(6);
  note("A_"); note_length(2);
  note("G#"); note_length(3);
  note("A_"); note_length(1);

  octave(5);
  note("C#"); note_length(6);

  goto mainloop;
}

//////////////////////////////////////////////////////////////

void Music_Surfing_Ch3();

void Music_Surfing_Ch3() {
  note_type(12, 1, 0);

  rest(12);

  octave(5);
  note("E_"); note_length(6);

  octave(4);
  note("E_"); note_length(2);
  note("F#"); note_length(2);
  note("G#"); note_length(2);

mainloop:

  octave(3);
  note("A_"); note_length(2);
  octave(4);
  note("A_"); note_length(2);
  note("A_"); note_length(2);
  note("E_"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);
  note("E_"); note_length(2);
  note("G#"); note_length(2);
  note("G#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);
  note("E_"); note_length(2);

  note("G#"); note_length(1);
  rest(2);
  note("E_"); note_length(1);

  note("C#"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);
  note("E_"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("C#"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);
  note("E_"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);
  note("E_"); note_length(2);
  note("G#"); note_length(2);
  note("G#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);
  note("E_"); note_length(2);

  note("G#"); note_length(1);
  rest(2);
  note("E_"); note_length(1);

  note("C#"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);
  note("E_"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("D_"); note_length(2);
  note("F#"); note_length(2);
  note("F#"); note_length(2);

  octave(3);
  note("A_"); note_length(2);

  octave(4);
  note("F#"); note_length(2);
  note("F#"); note_length(2);
  note("E_"); note_length(2);

  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("C#"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("E_"); note_length(2);

  note("G#"); note_length(2);
  note("G#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);

  note("E_"); note_length(2);

  note("A_"); note_length(2);
  note("A_"); note_length(2);

  octave(3);
  note("A_"); note_length(2);

  octave(4);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("D_"); note_length(2);
  note("F#"); note_length(2);
  note("F#"); note_length(2);

  octave(3);
  note("A_"); note_length(2);

  octave(4);
  note("F#"); note_length(2);
  note("F#"); note_length(2);

  note("E_"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("C#"); note_length(2);
  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("E_"); note_length(2);

  note("G#"); note_length(2);
  note("G#"); note_length(2);

  octave(3);
  note("B_"); note_length(2);

  octave(4);
  note("G#"); note_length(2);
  note("G#"); note_length(2);

  note("E_"); note_length(2);

  note("A_"); note_length(2);
  note("A_"); note_length(2);

  note("C#"); note_length(2);
  note("D_"); note_length(2);
  note("E_");

  goto mainloop;
}