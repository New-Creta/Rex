#include "pokemon/old_code/tileset.h"

#include "pokemon/constants/render_constants.h"

namespace pokemon
{
	Tileset::Tileset(u8* data, s32 width, s32 height, rex::gfx::TextureFormat textureFormat)
		: rex::TextureAsset(data, width, height, textureFormat)
	{
	}

	s16 Tileset::num_tiles_per_row() const
	{
		return width() / constants::g_tile_width_px;
	}
	s16 Tileset::num_tiles() const
	{
		return num_tiles_per_row() * (height() / constants::g_tile_height_px);
	}

}