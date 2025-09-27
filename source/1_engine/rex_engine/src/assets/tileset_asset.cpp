#include "rex_engine/assets/tileset_asset.h"

#include "rex_engine/assets/tileset.h"
#include "rex_engine/gfx/resources/texture_2d.h"

namespace rex
{
	TilesetAsset::TilesetAsset(rsl::pointi8 tileSize, const Tileset* texture)
		: m_tile_size(tileSize)
		, m_tileset_texture(texture)
	{
		
	}

	const Tileset* TilesetAsset::tileset_texture() const
	{
		return m_tileset_texture;
	}
	//const rex::Blockset* TilesetAsset::blockset() const
	//{
	//	return m_blockset;
	//}

	rsl::pointi8 TilesetAsset::tile_size() const
	{
		return m_tile_size;
	}

	u32 TilesetAsset::num_tiles_per_row() const
	{
		s32 tileset_width = tileset_texture()->texture_resource()->width();
		return tileset_width / tile_size().x;
	}
}