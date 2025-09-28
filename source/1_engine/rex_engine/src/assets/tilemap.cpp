#include "rex_engine/assets/tilemap.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	Tilemap::Tilemap(rsl::point<TileCount> size)
		: m_size(size)
	{
		m_tiles = rsl::make_unique<u8[]>(width().get() * height().get());
		rsl::memset(m_tiles.get(), 0, m_tiles.count());
	}

	void Tilemap::set(const u8* data, s32 len, s32 offset)
	{
		REX_ASSERT_X(offset + len <= m_tiles.count(), "would write out of bounds of tiles array");

		rsl::memcpy(m_tiles.get() + offset, data, len);
	}

	TileCount Tilemap::width() const
	{
		return m_size.x;
	}
	TileCount Tilemap::height() const
	{
		return m_size.y;
	}

	s32 Tilemap::num_tiles() const
	{
		return m_tiles.count();
	}
	const u8* Tilemap::tiles() const
	{
		return m_tiles.get();
	}

	void Tilemap::init_tiles(rsl::unique_array<u8>&& tiles)
	{
		m_tiles = rsl::move(tiles);
	}

}