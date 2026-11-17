#pragma once

namespace pokemon
{
	namespace registers
	{
		struct LCDC
		{
			// LCDC.0 has different meanings depending on Game Boy type and Mode:

			// Non-CGB Mode (DMG, SGB and CGB in compatibility mode): BG and Window display
			// When Bit 0 is cleared, both background and window become blank (white), and the Window Display Bit is ignored in that case. Only objects may still be displayed (if enabled in Bit 1).
			// 
			// CGB Mode: BG and Window master priority
			// When Bit 0 is cleared, the background and window lose their priority - the objects will be always displayed on top of background and window, independently of the priority flags in OAM and BG Map attributes.
			bool bg_window_enable : 1;

			// This bit toggles whether objects are displayed or not.
			bool obj_enable : 1;

			// This bit controls the size of all objects (1 tile or 2 stacked vertically).
			bool obj_size : 1;

			// This bit works similarly to LCDC bit 6: it determines which map the window uses for rendering
			bool bg_tile_map : 1;

			// This bit controls which addressing mode the BG and Window use to pick tiles.
			bool bg_window_tiles : 1;

			// This bit controls whether the window shall be displayed or not
			bool window_enable : 1;
			// This bit controls which background map the Window uses for rendering.
			bool window_tilemap : 1;

			// This bit controls whether the LCD is on and the PPU is active. Setting it to 0 turns both off, which grants immediate and full access to VRAM, OAM, etc.
			bool lcd_ppu_enable : 1;
		} g_lcdc;
	}
}