#pragma once

#include "rex_engine/assets/texture_asset.h"

namespace pokemon
{
	class Tileset : public rex::TextureAsset
	{
	public:
		Tileset(u8* data, s32 width, s32 height, rex::gfx::TextureFormat textureFormat);

		s16 num_tiles_per_row() const;
		s16 num_tiles() const;
	};
}