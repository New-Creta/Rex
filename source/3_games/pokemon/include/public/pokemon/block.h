#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/array.h"
#include "pokemon/tileset.h"
#include "rex_std/algorithm.h"

#include "rex_std/bonus/math/point.h"

namespace pokemon
{
	class Block
	{
	private:
		constexpr static s8 s_num_tiles_per_row = 4;
		constexpr static s8 s_num_tiles_per_column = 4;
		constexpr static s8 s_num_indices = s_num_tiles_per_row * s_num_tiles_per_column;
		constexpr static u8 s_invalid_tile_index = 255;

	public:
		using indices_array = rsl::array<u8, s_num_indices>;

		Block();
		Block(const indices_array& tileIndices);

		void set_all(s8 idx);

		u8 index_at(s8 idx) const;
		u8 index_at(rsl::pointi8 coord) const;

		rsl::random_access_iterator<u8> begin();
		rsl::const_random_access_iterator<u8> begin() const;
		rsl::const_random_access_iterator<u8> cbegin() const;

		rsl::random_access_iterator<u8> end();
		rsl::const_random_access_iterator<u8> end() const;
		rsl::const_random_access_iterator<u8> cend() const;

		constexpr static s8 num_tiles()
		{
			return s_num_indices;
		}
		constexpr static s8 num_tiles_per_row()
		{
			return s_num_tiles_per_row;
		}
		constexpr static s8 num_tiles_per_column()
		{
			return s_num_tiles_per_column;
		}
		constexpr static s8 width_px()
		{
			return static_cast<s8>(s_num_tiles_per_row * Tile::width_px());
		}
		constexpr static s8 height_px()
		{
			return static_cast<s8>(s_num_tiles_per_row * Tile::height_px());
		}
		constexpr static s8 major_dimension()
		{
			return num_tiles_per_row();
		}
	private:
		indices_array m_tile_indices;
	};
}