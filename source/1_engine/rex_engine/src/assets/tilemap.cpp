#include "rex_engine/assets/tilemap.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	Tilemap::Tilemap(s32 width, s32 height)
		: m_width(width)
		, m_height(height)
		, m_tile_width_px(8)
		, m_tile_height_px(8)
	{
		m_block_width_in_tiles = 4;
		m_block_height_in_tiles = 4;

		m_tiles = rsl::make_unique<u8[]>(m_width * m_height);
		rsl::memset(m_tiles.get(), 0, m_width * m_height);
	}

	void Tilemap::set(const u8* data, s32 len, s32 offset)
	{
		REX_ASSERT_X(offset + len <= m_tiles.count(), "would write out of bounds of tiles array");

		rsl::memcpy(m_tiles.get() + offset, data, len);
	}

	s32 Tilemap::width_in_px() const
	{
		return m_width * m_tile_width_px;
	}
	s32 Tilemap::height_in_px() const
	{
		return m_height * m_tile_height_px;
	}

	s32 Tilemap::width_in_tiles() const
	{
		return m_width;
	}
	s32 Tilemap::height_in_tiles() const
	{
		return m_height;
	}

	s32 Tilemap::width_in_blocks() const
	{
		return m_width * m_tile_width_px * m_block_width_in_tiles;
	}
	s32 Tilemap::height_in_blocks() const
	{
		return m_height * m_tile_height_px * m_block_height_in_tiles;
	}

	s32 Tilemap::tile_width_px() const
	{
		return m_tile_width_px;
	}
	s32 Tilemap::tile_height_px() const
	{
		return m_tile_height_px;
	}
	s32 Tilemap::num_tiles() const
	{
		return m_tiles.count();
	}
	const u8* Tilemap::tiles() const
	{
		return m_tiles.get();
	}

}