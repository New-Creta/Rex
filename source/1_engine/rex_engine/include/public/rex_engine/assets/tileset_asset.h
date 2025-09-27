#pragma once

#include "rex_engine/assets/asset.h"
#include "rex_engine/assets/tileset.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	// To render a tileset, you need a few things
	// 1) the tileset texture, obviously
	// 2) the blockset
	//		  an abstraction on top. This allows for more modular. A block is a 4x4 matrix holding tile indices into the tileset texture
	// 3) tile dimensions, holding the width and height of a single tile, in pixels
	class TilesetAsset : public Asset
	{
	public:
		TilesetAsset(rsl::pointi8 tileSize, const Tileset* texture);

		const Tileset* tileset_texture() const;
		//const rex::Blockset* blockset() const;

		rsl::pointi8 tile_size() const;

		u32 num_tiles_per_row() const;

	private:
		const Tileset* m_tileset_texture;
		//const rex::Blockset* m_blockset;

		// Store how big the tiles are in pixels
		rsl::pointi8 m_tile_size;
	};

}