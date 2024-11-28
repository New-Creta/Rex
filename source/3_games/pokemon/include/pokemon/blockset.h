#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"

#include "pokemon/block.h"

namespace pokemon
{
	class BlockSet
	{
	public:
		BlockSet(rsl::unique_array<Block> blocks);

		const Block& block(s32 idx) const;

	private:
		rsl::unique_array<Block> m_blocks;
	};

	rsl::shared_ptr<BlockSet> load_blockset(rsl::string_view filepath);
}