#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"

namespace rex
{
	RSL_CREATE_STRONG_TYPE(BlockCount, s32);
	RSL_CREATE_STRONG_TYPE(SquareCount, s32);
	RSL_CREATE_STRONG_TYPE(TileCount, s32);

	class Tilemap
	{
	public:
		Tilemap(rsl::point<TileCount> size);
		Tilemap(const Tilemap&) = delete;
		Tilemap(Tilemap&&) = default;
		
		Tilemap& operator=(const Tilemap&) = delete;
		Tilemap& operator=(Tilemap&&) = default;

		void set(const u8* data, s32 len, s32 offset);

		TileCount width() const;
		TileCount height() const;

		s32 num_tiles() const;
		const u8* tiles() const;

	protected:
		void init_tiles(rsl::unique_array<u8>&& tiles);

	private:
		rsl::point<TileCount> m_size;
		rsl::unique_array<u8> m_tiles;
	};
}