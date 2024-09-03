#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"

namespace pokemon
{
	class Block
	{
	public:
		static constexpr s8 s_num_indices = 16;
		using tile_indices = rsl::array<rsl::byte, s_num_indices>;

	public:
		Block(const tile_indices& tileIndices);

		const tile_indices& indices() const;

	private:
		tile_indices m_tile_indices;
	};

	class BlockSet
	{
	public:
		const Block& block(s8 idx) const;

	private:
		rsl::vector<Block> m_blocks;
	};

	rsl::shared_ptr<BlockSet> load_blockset(rsl::string_view filepath);
}