#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/array.h"
#include "pokemon/tileset.h"
#include "rex_std/algorithm.h"

namespace pokemon
{
	class Block
	{
	private:
		constexpr static s8 s_num_indices = 16;
		constexpr static s8 s_num_tiles_per_row = 4;
		constexpr static s8 s_num_tiles_per_column = 4;

	public:
		Block()
		{
			rsl::fill_n(m_tile_indices.begin(), m_tile_indices.end(), -1);
		}

		void set(s8 idx);

		s8 index_at(s8 idx) const;
		s8 index_at(s8 x, s8 y) const;

		rsl::random_access_iterator<s8> begin();
		rsl::const_random_access_iterator<s8> begin() const;
		rsl::const_random_access_iterator<s8> cbegin() const;

		rsl::random_access_iterator<s8> end();
		rsl::const_random_access_iterator<s8> end() const;
		rsl::const_random_access_iterator<s8> cend() const;

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
			return s_num_tiles_per_row * Tile::width_px();
		}
		constexpr static s8 height_px()
		{
			return s_num_tiles_per_row * Tile::height_px();
		}

	private:
		rsl::array<s8, s_num_indices> m_tile_indices;
	};
}