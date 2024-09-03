#pragma once

#include "rex_std/vector.h"

#include "rex_engine/engine/types.h"

namespace pokemon
{
	class MapBlocks
	{
	public:
		MapBlocks(rsl::vector<s8>&& blockIndices);

	private:
		rsl::vector<s8> m_block_indices;
	};

	rsl::shared_ptr<MapBlocks> load_blocks(rsl::string_view filepath);
}