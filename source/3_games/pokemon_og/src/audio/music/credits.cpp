#include <string>

static const char* Music_CinnabarMansion_Ch1 = R"(Music_CinnabarMansion_Ch1::
	tempo 144
	volume 7, 7
	vibrato 11, 2, 5
	duty_cycle 2
.mainloop:
.loop1:
	note_type 12, 6, 2
	octave 5
	note E_, 1
	note E_, 1
	octave 4
	note B_, 1
	note B_, 1
	note C_, 1
	rest 2
	octave 5
	note B_, 2
	note E_, 2
	octave 4
	note C_, 2
	note B_, 2
	note E_, 2
	note C_, 1
	octave 5
	note B_, 1
	rest 2
	sound_loop 14, .loop1
	note_type 12, 10, 5
	rest 16
	rest 16
	rest 15
	octave 4
	note C_, 1
	octave 5
	note B_, 1
	note B_, 2
	sound_loop 0, .mainloop
)";

static const char* Music_Cities1_Ch1 = R"(Music_Cities1_Ch1::
	tempo 144
.body:
	volume 7, 7
	vibrato 8, 2, 4
	duty_cycle 3
.mainloop:
	note_type 12, 12, 5
	octave 3
	note G#, 4
	note F#, 4
	note E_, 2
	note E_, 2
	note F#, 2
	note D#, 2
	note E_, 2
	note E_, 2
	note D#, 2
	note C#, 4
	note D#, 4
	note E_, 2
	note D#, 4
	note C#, 2
	note E_, 2
	note E_, 4
	note_type 12, 10, 5
	note C#, 4
	octave 2
	note B_, 6
	octave 3
	note C#, 2
	note C#, 4
	octave 2
	note B_, 4
	note_type 12, 12, 5
	sound_call .sub1
	octave 3
	note D#, 6
	note E_, 2
	octave 2
	note B_, 4
	note_type 12, 10, 5
	octave 3
	note C#, 2
	octave 2
	note B_, 2
	note A_, 4
	note B_, 4
	note B_, 2
	octave 3
	note C#, 2
	note D#, 2
	note E_, 2
	note D#, 2
	note C#, 2
	note D#, 2
	note_type 12, 12, 5
	note G#, 2
	note E_, 2
	note F#, 2
	note E_, 2
	note E_, 4
	note F#, 2
	note D#, 2
	note E_, 4
	note D#, 2
	note C#, 4
	note D#, 4
	note E_, 2
	note D#, 2
	note C#, 2
	note C#, 2
	note E_, 2
	note E_, 4
	note_type 12, 10, 5
	note C#, 2
	octave 2
	note A_, 2
	note B_, 6
	octave 3
	note C#, 2
	note C#, 2
	octave 2
	note B_, 2
	note B_, 4
	sound_loop 0, .mainloop
.sub1:
	octave 3
	note F#, 2
	note D#, 4
	note E_, 2
	note D#, 4
	note C#, 4
	octave 2
	note B_, 4
	octave 3
	note C#, 2
	note D#, 2
	note C#, 2
	sound_ret
)";

static const char* Music_Cities2_Ch1 = R"(Music_Cities2_Ch1::
	tempo 148
	volume 7, 7
	duty_cycle 3
	vibrato 8, 3, 2
	toggle_perfect_pitch
	note_type 12, 11, 6
	rest 8
	octave 3
	note E_, 2
	note D#, 2
	note C#, 2
	note C_, 2
	octave 2
	note B_, 2
	rest 14
.mainloop:
	octave 3
	note B_, 4
	octave 4
	note C#, 2
	note D#, 1
	note D_, 1
	note C#, 4
	octave 3
	note B_, 4
	note G#, 8
	note E_, 8
	note A_, 4
	note G#, 2
	note F#, 2
	note G#, 2
	note A_, 2
	note B_, 2
	octave 4
	note C#, 2
	rest 16
	sound_loop 0, .mainloop
)";

static const char* Music_Credits_Ch1 = R"(Music_Credits_Ch1::
	tempo 140
	volume 7, 7
	duty_cycle 3
	vibrato 8, 3, 4
	toggle_perfect_pitch
	note_type 12, 11, 5
	octave 4
	note E_, 6
	octave 3
	note A_, 1
	octave 4
	note E_, 1
	note D_, 6
	octave 3
	note G_, 1
	octave 4
	note D_, 1
	note C#, 6
	octave 3
	note F#, 1
	octave 4
	note C#, 1
	note D_, 4
	note E_, 2
	note C#, 1
	note E_, 1
	note C#, 1
	rest 1
	octave 3
	note E_, 1
	rest 3
	note E_, 1
	note E_, 1
	note E_, 1
	rest 1
	note E_, 1
	rest 1
	note E_, 1
	rest 3
	note E_, 1
	rest 1
	note E_, 1
	rest 3
	note E_, 1
	note E_, 1
	note E_, 1
	rest 1
	note E_, 1
	rest 1
	note E_, 1
	note E_, 1
	note F#, 1
	note G_, 1
	note_type 12, 11, 6
	note A_, 4
	note E_, 2
	note A_, 2
	note G_, 4
	note A_, 2
	note G_, 2
	note B_, 4
	note A_, 4
	note G_, 2
	note F#, 2
	note E_, 2
	note D_, 2
	note C#, 6
	note E_, 2
	note A_, 4
	note C#, 4
	note E_, 4
	note D_, 2
	note C#, 2
	note E_, 2
	note F#, 2
	note G_, 2
	note F#, 2
	note A_, 4
	note E_, 2
	note A_, 2
	note G_, 4
	note A_, 2
	note G_, 2
	note B_, 4
	note A_, 4
	note G_, 2
	note A_, 2
	note F#, 2
	note D_, 2
	note E_, 6
	note C#, 2
	note A_, 4
	note C#, 4
	note E_, 4
	note D_, 2
	note C#, 2
	note E_, 2
	note F#, 2
	note G_, 2
	note F#, 2
	note G_, 4
	note D_, 2
	note G_, 2
	note B_, 2
	note A_, 2
	note G_, 2
	note A_, 2
	note D_, 4
	note E_, 2
	note F#, 2
	note G_, 2
	note F#, 2
	note E_, 2
	note D_, 2
	sound_ret
)";

static const char* Music_Credits_Ch2 = R"(Music_Credits_Ch2::
	duty_cycle 3
	vibrato 10, 2, 5
	note_type 12, 12, 5
	octave 4
	note A_, 6
	note E_, 1
	note A_, 1
	note G_, 6
	note D_, 1
	note G_, 1
	sound_ret
)";

static const char* Music_Credits_Ch3 = R"(Music_Credits_Ch3::
	note_type 12, 1, 0
	octave 5
	note C#, 6
	octave 4
	note A_, 1
	octave 5
	note C#, 1
	note D_, 6
	octave 4
	note B_, 1
	octave 5
	note D_, 1
	note F#, 6
	note D_, 1
	note F#, 1
	note A_, 4
	note G#, 2
	note E_, 1
	note G#, 1
	note A_, 1
	rest 15
	sound_ret
)";

int main() {
	return 0;
}