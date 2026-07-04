#include <cstdint>

namespace AudioData
{
	const char* SFX_Table = R"(
SFX_Headers_3::
	db $ff, $ff, $ff ; padding

SFX_Noise_Instrument01_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument01_3_Ch8

SFX_Noise_Instrument02_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument02_3_Ch8

SFX_Noise_Instrument03_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument03_3_Ch8

SFX_Noise_Instrument04_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument04_3_Ch8

SFX_Noise_Instrument05_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument05_3_Ch8

SFX_Noise_Instrument06_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument06_3_Ch8

SFX_Noise_Instrument07_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument07_3_Ch8

SFX_Noise_Instrument08_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument08_3_Ch8

SFX_Noise_Instrument09_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument09_3_Ch8

SFX_Noise_Instrument10_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument10_3_Ch8

SFX_Noise_Instrument11_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument11_3_Ch8

SFX_Noise_Instrument12_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument12_3_Ch8

SFX_Noise_Instrument13_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument13_3_Ch8

SFX_Noise_Instrument14_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument14_3_Ch8

SFX_Noise_Instrument15_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument15_3_Ch8

SFX_Noise_Instrument16_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument16_3_Ch8

SFX_Noise_Instrument17_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument17_3_Ch8

SFX_Noise_Instrument18_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument18_3_Ch8

SFX_Noise_Instrument19_3::
	channel_count 1
	channel 8, SFX_Noise_Instrument19_3_Ch8
)";

	const char* Music_BikeRiding = R"(
Music_BikeRiding_Ch1::
	tempo 144
	volume 7, 7
	duty_cycle 3
	vibrato 8, 1, 4
	note_type 12, 11, 5
	octave 3
	note G_, 2
.mainloop:
	octave 4
	note C_, 4
	note D_, 4
	note E_, 2
	note C_, 2
	note E_, 2
	note G_, 2
	note G_, 2
	note F_, 2
	note E_, 2
	note F_, 4
	note E_, 2
	note D_, 2
	note F_, 4
	note D_, 4
	octave 3
	note B_, 2
	octave 4
	note F_, 4
	note D_, 4
	note E_, 2
	note F_, 2
	note G_, 2
	note C_, 2
	note E_, 2
	note C_, 2
	note D_, 2
	note E_, 2
	note_type 12, 11, 6
	note F_, 10
	note_type 12, 10, 6
	note F_, 2
	note E_, 2
	note F_, 2
	note G_, 10
	note E_, 2
	note D_, 2
	note E_, 2
	note F_, 6
	toggle_perfect_pitch
	note_type 12, 11, 3
	note E_, 2
	note D_, 2
	note D_, 1
	note E_, 1
	note F_, 2
	note E_, 1
	note F_, 1
	toggle_perfect_pitch
	note_type 12, 11, 5
	note G_, 6
	note G_, 6
	note A_, 2
	note F_, 2
	note G_, 6
)";

	const char* Music_Celadon = R"(
Music_Celadon_Ch1::
	tempo 144
	volume 7, 7
	duty_cycle 3
	toggle_perfect_pitch
	note_type 12, 2, -7
	rest 8
	octave 3
	note D_, 8
.mainloop:
	duty_cycle 2
	note_type 12, 11, 2
	note G_, 4
	note B_, 4
	note G_, 2
	note B_, 4
	note G_, 2
	note E_, 1
	note E_, 1
	note G_, 1
	note E_, 1
	note B_, 2
	octave 4
	note C_, 2
	octave 3
	note A_, 8
)";

	const char* Music_Cinnabar = R"(
Music_Cinnabar_Ch1::
	tempo 144
	volume 7, 7
	duty_cycle 3
	vibrato 12, 3, 4
	toggle_perfect_pitch
.mainloop:
	note_type 12, 11, 5
	rest 4
	octave 3
	note D_, 4
	note E_, 6
	note C#, 2
)";
}