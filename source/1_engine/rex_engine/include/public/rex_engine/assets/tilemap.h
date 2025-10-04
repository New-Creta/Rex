#pragma once

#include "rex_engine/assets/asset.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/engine/types_2d.h"
#include "rex_std/memory.h"

namespace rex
{
	class Tilemap : public Asset
	{
	public:
		Tilemap(rsl::point<TileCount> size);
		Tilemap(const Tilemap&) = delete;
		Tilemap(Tilemap&&) = default;
		
		Tilemap& operator=(const Tilemap&) = delete;
		Tilemap& operator=(Tilemap&&) = default;

		void set(const u8* data, s32 len, s32 offset);
		void set(s32 offset, u8 value);

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