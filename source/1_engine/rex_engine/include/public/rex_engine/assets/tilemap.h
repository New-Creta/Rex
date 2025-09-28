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
		Tilemap(BlockCount width, BlockCount height);
		Tilemap(const Tilemap&) = delete;
		Tilemap(Tilemap&&) = default;

		Tilemap& operator=(const Tilemap&) = delete;
		Tilemap& operator=(Tilemap&&) = default;

		void set(const u8* data, s32 len, s32 offset);

		s32 width_in_px() const;
		s32 height_in_px() const;

		s32 width_in_tiles() const;
		s32 height_in_tiles() const;

		s32 width_in_blocks() const;
		s32 height_in_blocks() const;

		s32 tile_width_px() const;
		s32 tile_height_px() const;

		s32 num_tiles() const;
		const u8* tiles() const;

	protected:
		void init_tiles(rsl::unique_array<u8>&& tiles);

	private:
		BlockCount m_width;
		BlockCount m_height;
		s32 m_tile_width_px;
		s32 m_tile_height_px;

		s32 m_block_width_in_tiles;
		s32 m_block_height_in_tiles;

		rsl::unique_array<u8> m_tiles;
	};
}