#pragma once

#include "rex_engine/text_processing/json.h"

#include "pokemon/poke_structs.h"

namespace pokemon
{
	class ChunkConnection
	{
	public:
		ChunkConnection(const rex::json::json& json);

		rsl::string_view dst_chunk_name() const;

	private:
		Direction m_direction;
		rsl::string m_dst_chunk;
		// Move along X or Y axis based on the direction
		// if north or south move "offset" right
		// if east or west move "offset" down
		s8 m_offset;
	};

	class ChunkComponent
	{
	public:
		ChunkComponent(const rex::json::json& json);

		rsl::string_view name() const;
		const rsl::vector<ChunkConnection>& connections() const;

	private:
		rsl::string m_name;
		s8 m_width; // Width in blocks
		s8 m_height; // Height in blocks
		rsl::vector<ChunkConnection> m_connections;
		rsl::string m_blk_path;
		rsl::string m_block_and_tiles_name;
	};
}