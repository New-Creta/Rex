#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	const s8 TILE_WIDTH = 8; // pixels

	const s8 LEN_1BPP_TILE = 1 * TILE_WIDTH; // bytes
	const s8 LEN_2BPP_TILE = 2 * TILE_WIDTH; // bytes

	const s8 SCREEN_WIDTH = 20;
	const s8 SCREEN_HEIGHT = 18;
	const s16 SCREEN_WIDTH_PX = SCREEN_WIDTH * TILE_WIDTH;		// pixels
	const s16 SCREEN_HEIGHT_PX = SCREEN_HEIGHT * TILE_WIDTH;	// pixels

	const s8 BG_MAP_WIDTH = 32;	 // tiles
	const s8 BG_MAP_HEIGHT = 32; // tiles

	const s8 SPRITEBUFFERSIZE = 7 * 7 * LEN_1BPP_TILE;

	// HP bar
	const s8 HP_BAR_GREEN = 0;
	const s8 HP_BAR_YELLOW = 1;
	const s8 HP_BAR_RED = 2;

	// wShadowOAM
	const s8 NUM_SPRITE_OAM_STRUCTS = 40;

	// hAutoBGTransferEnabled
	const s8 TRANSFERTOP = 0;
	const s8 TRANSFERMIDDLE = 1;
	const s8 TRANSFERBOTTOM = 2;

	// hRedrawRowOrColumnMode
	const s8 REDRAW_COL = 1;
	const s8 REDRAW_ROW = 1;

	// tile list ids
	// TileIDListPointerTable indexes(see data / tilemaps.asm)
	enum
	{
		TILEMAP_MON_PIC,
		TILEMAP_SLIDE_DOWN_MON_PIC_7X5,
		TILEMAP_SLIDE_DOWN_MON_PIC_7X3,
		TILEMAP_GENGAR_INTRO_1,
		TILEMAP_GENGAR_INTRO_2,
		TILEMAP_GENGAR_INTRO_3,
		TILEMAP_GAME_BOY,
		TILEMAP_LINK_CABLE,
		NUM_TILEMAPS,
	};
}