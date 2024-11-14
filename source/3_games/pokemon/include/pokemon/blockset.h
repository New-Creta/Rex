#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"

#include "pokemon/block.h"

namespace pokemon
{
	class BlockSet
	{
	public:
		const Block& block(s8 idx) const;

	private:
		rsl::vector<Block> m_blocks;
	};

	rsl::shared_ptr<BlockSet> load_blockset(rsl::string_view filepath);
}