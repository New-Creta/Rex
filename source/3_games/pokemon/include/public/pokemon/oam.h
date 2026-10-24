#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/globals.h"
#include "pokemon/memory_constants.h"

#include "rex_std/bonus/math.h"

namespace pokemon
{
	enum class OAMAttributes : u8
	{
		None						= 0,
		CGBPalleteMask	= 0b11,
		Bank						= BIT(3),
		DmgPalette			= BIT(4),
		FlipX						= BIT(5),
		FlipY						= BIT(6),
		BehindBg				= BIT(7)
	};


	rsl::pointi8 calc_oam_coord(s8 x_tile, s8 y_tile, s8 x_pixel, s8 y_pixel);
	struct OAMStruct
	{
		s8 y_coord;			// Y coordinate in pixels, starting from the top
		s8 x_coord;			// X coordinate in pixels, starting from the left
		s8 tile_id;			// Tile index to use from tilemap
		OAMAttributes attributes;	// Attributes of the object
	};

	// OAM == Object Attribute Memory
	class Oam
	{
	public:
		Oam();

		void set(s32 idx, OAMStruct oamData);

	private:
		rsl::array<OAMStruct, constants::g_oam_size / sizeof(OAMStruct)> m_objects;
	};

	namespace oam
	{
		void init(rex::globals::GlobalUniquePtr<Oam>&& oam);
		Oam* instance();
		void shutdown();
	}
}