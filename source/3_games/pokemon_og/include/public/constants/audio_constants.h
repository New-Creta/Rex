#pragma once

#include "rex_engine/engine/types.h"

#include "pokemon/original_code/constants/hardware_constants.h"

namespace pokemon
{
	// pitch
	// Audio[1 | 2 | 3]_Pitches indexes(see audio / notes.asm)
	enum Note
	{
		C_,
		CSharp,
		D_,
		DSharp,
		E_,
		F_,
		FSharp,
		G_,
		GSharp,
		A_,
		ASharp,
		B_,
		NUM_NOTES
	};

	// channel
	// Audio[1|2|3]_HWChannelBaseAddresses, Audio[1|2|3]_HWChannelDisableMasks,
	// and Audio[1|2|3]_HWChannelEnableMasks indexes (see audio/engine_[1|2|3].asm)
	enum Channel
	{
		CHAN1,
		CHAN2,
		CHAN3,
		CHAN4,
		NUM_MUSIC_CHANS,
		CHAN5 = 4,
		CHAN6,
		CHAN7,
		CHAN8,
		NUM_NOISE_CHANS = CHAN8 - NUM_MUSIC_CHANS,
		NUM_CHANNELS
	};

	// HW sound channel register base addresses
	const s8 HW_CH1_BASE = LOW(*rNR10);
	const s8 HW_CH2_BASE = LOW(*rNR21) - 1;
	const s8 HW_CH3_BASE = LOW(*rNR30);
	const s8 HW_CH4_BASE = LOW(*rNR41) - 1;

	// HW sound channel enable bit masks
	const s8 HW_CH1_ENABLE_MASK = LOW(0b00010001);
	const s8 HW_CH2_ENABLE_MASK = LOW(0b00100010);
	const s8 HW_CH3_ENABLE_MASK = LOW(0b01000100);
	const s8 HW_CH4_ENABLE_MASK = LOW(0b10001000);

	// HW sound channel disable bit masks
	const s8 HW_CH1_DISABLE_MASK = (~HW_CH1_ENABLE_MASK & 0xff);
	const s8 HW_CH2_DISABLE_MASK = (~HW_CH2_ENABLE_MASK & 0xff);
	const s8 HW_CH3_DISABLE_MASK = (~HW_CH3_ENABLE_MASK & 0xff);
	const s8 HW_CH4_DISABLE_MASK = (~HW_CH4_ENABLE_MASK & 0xff);

	const s8 REG_DUTY_SOUND_LEN = 1;
	const s8 REG_VOLUME_ENVELOPE = 2;
	const s8 REG_FREQUENCY_LO = 3;

	const s8 BIT_PERFECT_PITCH					= 0; // 0 ; controlled by toggle_perfect_pitch command
	const s8 BIT_SOUND_CALL							= 1; // 1 ; if in sound call
	const s8 BIT_NOISE_OR_SFX						= 2; // 2 ; if channel is the music noise channel or an SFX channel
	const s8 BIT_VIBRATO_DIRECTION			= 3; // 3 ; if the pitch is above or below normal (cycles)
	const s8 BIT_PITCH_SLIDE_ON					= 4; // 4 ; if pitch slide is active
	const s8 BIT_PITCH_SLIDE_DECREASING = 5; // 5 ; if the pitch slide frequency is decreasing (instead of increasing)
	const s8 BIT_ROTATE_DUTY_CYCLE			= 6; // 6 ; if rotating duty cycle

	// wChannelFlags2 constant (only has one flag)
	const s8 BIT_EXECUTE_MUSIC = 0; // if in execute music
}