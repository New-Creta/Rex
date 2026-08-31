#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

namespace pokemon
{
	namespace constants
	{
		// 0000	3FFF	  16,384 bytes     16 KiB ROM bank 00	From cartridge, usually a fixed bank
		// 4000	7FFF	  16,384 bytes     16 KiB ROM Bank 01–NN	From cartridge, switchable bank via mapper(if any)
		// 8000	9FFF	   8,192 bytes     8 KiB Video RAM(VRAM)	In CGB mode, switchable bank 0 / 1
		//		The first 6144 bytes are used for 384 tiles, each being 16 bytes
		//		The remaining 2048 bytes are to be used for tile indices
		//
		// A000	BFFF	   8,192 bytes     8 KiB External RAM	From cartridge, switchable bank if any
		// C000	CFFF	   4,096 bytes     4 KiB Work RAM(WRAM)
		// D000	DFFF	   4,096 bytes     4 KiB Work RAM(WRAM)	In CGB mode, switchable bank 1–7
		// E000	FDFF	   7,680 bytes     Echo RAM(mirror of C000–DDFF)	Nintendo says use of this area is prohibited.
		// FE00	FE9F	     160 bytes     Object attribute memory(OAM)
		// FEA0	FEFF	      95 bytes     Not Usable	Nintendo says use of this area is prohibited.
		// FF00	FF7F	     128 bytes     I / O Registers
		// FF80	FFFE	     127 bytes     High RAM(HRAM)
		// FFFF	FFFF	       1 byte      Interrupt Enable register (IE)
		//
		// Total Pokemon Red Size: 1,048,576 bytes
		// Blank Data: 464,522 bytes
		// Reserved Data & Registers: 7904
		// VRAM and OAM: 8352
		// Total Asset & Code size: 567,798

		constexpr s32 g_rom0_size = 16_kib;
		constexpr s32 g_romn_size = 16_kib;
		constexpr s32 g_vram_size = 8_kib;
		constexpr s32 g_tilemap_size = 6144_bytes;
		constexpr s32 g_tile_indices_size = 2048_bytes;
		constexpr s32 g_xram_size = 8_kib;
		constexpr s32 g_wram_size = 8_kib;
		constexpr s32 g_oam_size  = 160_bytes;
		constexpr s32 g_hram_size = 127_bytes;
	}
}