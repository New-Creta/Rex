#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"

namespace rex
{
	class Tilemap
	{
	public:
		Tilemap(s32 width, s32 height);
		Tilemap(const Tilemap&) = delete;
		Tilemap(Tilemap&&) = default;

		Tilemap& operator=(const Tilemap&) = delete;
		Tilemap& operator=(Tilemap&&) = default;

		void set(const u8* data, s32 len, s32 offset);

		s32 width_in_px() const;
		s32 height_in_px() const;

		s32 width_in_tiles() const;
		s32 height_in_tiles() const;
		s32 tile_width_px() const;
		s32 tile_height_px() const;

		s32 num_tiles() const;
		const u8* tiles() const;

	private:
		s32 m_width;
		s32 m_height;
		s32 m_tile_width_px;
		s32 m_tile_height_px;
		rsl::unique_array<u8> m_tiles;
	};
}