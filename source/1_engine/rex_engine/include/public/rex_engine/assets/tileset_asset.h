#pragma once

#include "rex_engine/assets/asset.h"
#include "rex_engine/engine/types_2d.h"

#include "rex_std/bonus/math.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
	}

	// MERGE TILESET AND TILESET ASSET
	// one represent the metadata + binary data
	// the other represents just the binary data

	// To render a tileset, you need a few things
	// 1) the tileset texture, obviously
	// 2) the blockset
	//		  an abstraction on top. This allows for more modular design. A block is a 4x4 matrix holding tile indices into the tileset texture
	// 3) tile dimensions, holding the width and height of a single tile, in pixels
	class TilesetAsset : public Asset
	{
	public:
		TilesetAsset(rsl::pointi8 tileSize, rsl::unique_ptr<gfx::Texture2D> texture);

		const gfx::Texture2D* tileset_texture() const;

		rsl::pointi8 tile_size() const;

		u32 num_tiles_per_row() const;

	private:
		rsl::unique_ptr<gfx::Texture2D> m_tileset_texture;

		// Store how big the tiles are in pixels
		rsl::pointi8 m_tile_size;
	};

}