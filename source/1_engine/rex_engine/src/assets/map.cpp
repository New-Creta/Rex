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

	s32 Map::width_in_tiles() const
	{
		const s32 num_tiles_per_block_row = 4;
		return m_desc.map_header.width_in_blocks * num_tiles_per_block_row;
	}
	s32 Map::height_in_tiles() const
	{
		const s32 num_tiles_per_block_row = 4;
		return m_desc.map_header.height_in_blocks * num_tiles_per_block_row;
	}
}