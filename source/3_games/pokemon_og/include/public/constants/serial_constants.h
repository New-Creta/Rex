#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	const s8 ESTABLISH_CONNECTION_WITH_INTERNAL_CLOCK = 0x01;
	const s8 ESTABLISH_CONNECTION_WITH_EXTERNAL_CLOCK = 0x02;

	const s8 USING_EXTERNAL_CLOCK = 0x01;
	const s8 USING_INTERNAL_CLOCK = 0x02;
	const s8 CONNECTION_NOT_ESTABLISHED = 0xff;
	
	// signals the start of an array of bytes transferred over the link cable
	const s8 SERIAL_PREAMBLE_BYTE = 0xfd;

	// this byte is used when there is no data to send
	const s8 SERIAL_NO_DATA_BYTE = 0xfe;

	// signals the end of one part of a patch list (there are two parts) for player/enemy party data
	const s8 SERIAL_PATCH_LIST_PART_TERMINATOR = 0xfe;

	const s8 SERIAL_PREAMBLE_LENGTH = 6;
	const s8 SERIAL_RN_PREAMBLE_LENGTH = 7;
	const s8 SERIAL_RNS_LENGTH = 10;

	const s8 LINK_STATE_NONE					= 0x00; // not using link
	const s8 LINK_STATE_IN_CABLE_CLUB = 0x01; // in a cable club room (Colosseum or Trade Centre)
	const s8 LINK_STATE_START_TRADE		= 0x02; // pre-trade selection screen initialisation
	const s8 LINK_STATE_START_BATTLE	= 0x03; // pre-battle initialisation
	const s8 LINK_STATE_BATTLING			= 0x04; // in a link battle
	const s8 LINK_STATE_RESET					= 0x05; // reset game (unused)
	const s8 LINK_STATE_TRADING				= 0x32; // in a link trade
	
	const s8 LINKBATTLE_RUN = 0xf;
	const s8 LINKBATTLE_STRUGGLE = 0xe;
	const s8 LINKBATTLE_NO_ACTION = 0xd;
}