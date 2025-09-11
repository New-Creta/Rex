#pragma once

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/tilemap.h"

namespace pokemon
{
	class GameTilemap : public rex::Tilemap
	{
	public:
		GameTilemap(const rex::Map* map);

	private:
		void init_border_blocks(u8* blocks, s32 numBlocks, const rex::Map* map);
		void init_connection_blocks(u8* blocks, s32 numBlocks, const rex::Map* map);
		void init_inner_map_blocks(u8* blocks, s32 numBlocks, const rex::Map* map);

		void convert_blocks_to_tiles(u8* blocks, s32 numBlocks);

	private:
		rsl::unique_array<u8> m_tiles;
	};
}
