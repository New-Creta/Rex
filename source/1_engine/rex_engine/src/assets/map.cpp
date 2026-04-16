#include "rex_engine/assets/map.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/memory/blob_reader.h"

namespace rex
{
	Map::Map(MapDesc&& desc)
		: m_desc(rsl::move(desc))
	{
	}

	const MapDesc& Map::desc() const
	{
		return m_desc;
	}
	const u8* Map::blocks(s32 offset) const
	{
		return m_desc.blocks.get() + offset;
	}

	const Blockset* Map::blockset() const
	{
		return m_desc.blockset;
	}

	BlockCount Map::width() const
	{
		return BlockCount(m_desc.map_header.size.x);
	}
	BlockCount Map::height() const
	{
		return BlockCount(m_desc.map_header.size.y);
	}

	WorldCoordConverter Map::create_world_coord_converter()
	{
		WorldCoordConverter converter{};
		converter.num_pixels_per_tile = m_desc.blockset->tileset()->tile_size().x;
		converter.num_tiles_per_square = 2;
		converter.num_tiles_per_block = Block::num_tiles_per_row();

		return converter;
	}

}