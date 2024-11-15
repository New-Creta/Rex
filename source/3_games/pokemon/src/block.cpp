#include "pokemon/block.h"
#include "pokemon/tileset.h"

namespace pokemon
{
	void Block::set(s8 idx)
	{
		m_tile_indices.fill(idx);
	}

	s8 Block::index_at(s8 idx) const
	{
		return m_tile_indices[idx];
	}
	s8 Block::index_at(s8 x, s8 y) const
	{
		s8 idx = y * s_num_tiles_per_row + x;
		return m_tile_indices[idx];
	}

	rsl::random_access_iterator<s8> Block::begin()
	{
		return m_tile_indices.begin();
	}
	rsl::const_random_access_iterator<s8> Block::begin() const
	{
		return m_tile_indices.begin();
	}
	rsl::const_random_access_iterator<s8> Block::cbegin() const
	{
		return m_tile_indices.cbegin();
	}

	rsl::random_access_iterator<s8> Block::end()
	{
		return m_tile_indices.end();
	}
	rsl::const_random_access_iterator<s8> Block::end() const
	{
		return m_tile_indices.end();
	}
	rsl::const_random_access_iterator<s8> Block::cend() const
	{
		return m_tile_indices.cend();
	}
}