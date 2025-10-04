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
		return BlockCount(m_desc.map_header.width_in_blocks);
	}
	BlockCount Map::height() const
	{
		return BlockCount(m_desc.map_header.height_in_blocks);
	}
}