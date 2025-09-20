#include "rex_engine/assets/map.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/memory/blob_reader.h"

namespace rex
{
	Map::Map(MapDesc&& desc, LoadFlags loadFlags)
		: m_desc(rsl::move(desc))
	{
		if (!rsl::has_flag(loadFlags, LoadFlags::PartialLoad))
		{
			load_tiles();
		}
	}

	const MapDesc& Map::desc() const
	{
		return m_desc;
	}
	const u8* Map::blocks(s32 offset) const
	{
		return m_blocks.get() + offset;
	}

	const u8* Map::tiles(s32 offset) const
	{
		return m_tiles.get() + offset;
	}

	const Blockset* Map::blockset() const
	{
		return m_blockset;
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

	void Map::load_tiles()
	{
		const s32 num_tiles_per_block = 16;
		const s32 num_tiles_per_block_row = 4;
		m_tiles = rsl::make_unique<u8[]>(m_desc.map_header.width_in_blocks * num_tiles_per_block_row * m_desc.map_header.height_in_blocks * num_tiles_per_block_row);

		m_blockset = asset_db::instance()->load<Blockset>(m_desc.blockset);
		memory::Blob blockmap = vfs::instance()->read_file(m_desc.blockmap);

		memory::BlobReader reader(blockmap);
		
		while (reader.read_offset() < blockmap.size())
		{
			// Reading 1 block
			s32 num_blocks_read = reader.read_offset();
			s32 block_row = num_blocks_read / m_desc.map_header.width_in_blocks;
			s32 block_row_size = m_desc.map_header.width_in_blocks * num_tiles_per_block;
			s32 tile_row_size = m_desc.map_header.width_in_blocks * num_tiles_per_block_row;
			s32 block_idx_in_row = num_blocks_read % m_desc.map_header.width_in_blocks;
			u8 block_idx = reader.read<u8>();
			const Block& block = m_blockset->block(block_idx);

			rsl::array<u8, num_tiles_per_block_row> block_tiles_in_row;
			for (s32 tile_row = 0; tile_row < num_tiles_per_block_row; ++tile_row)
			{
				// Reading 4 tiles (1 block row)
				block.fill_tiles(tile_row, block_tiles_in_row.data(), block_tiles_in_row.size());
				s32 tile_idx_in_block;
				u8* start = m_tiles.get()
					+ block_row * block_row_size
					+ block_idx_in_row * num_tiles_per_block_row
					+ tile_row * tile_row_size;

				rsl::memcpy(start, block_tiles_in_row.data(), num_tiles_per_block_row);
			}
		}
	}

	//s32 Map::width_in_blocks() const
	//{
	//	return m_desc.map_header.width_in_blocks;
	//}
	//s32 Map::height_in_blocks() const
	//{
	//	return m_desc.map_header.height_in_blocks;
	//}
	//s32 Map::border_block_idx() const
	//{
	//	return m_desc.map_header.border_block_idx;
	//}

	//const rsl::unique_array<MapConnection>& Map::connections() const
	//{
	//	return m_desc.connections;
	//}
	//const Blockset* Map::blockset() const
	//{
	//	return m_desc.blocks;
	//}

	//const rsl::unique_array<s8>& Map::blocks() const
	//{
	//	return m_desc.blocks;
	//}
}