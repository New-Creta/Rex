#include "pokemon/chunk_component.h"

#include "rex_std/bonus/utility.h"
namespace pokemon
{
	ChunkConnection::ChunkConnection(const rex::json::json& json)
	{
		m_direction = rsl::enum_refl::enum_cast<Direction>(json["direction"].get<rsl::string_view>()).value();
		m_dst_chunk = json["chunk"];
		m_offset = json["offset"];
	}

	rsl::string_view ChunkConnection::dst_chunk_name() const
	{
		return m_dst_chunk;
	}

	ChunkComponent::ChunkComponent(const rex::json::json& json)
	{
		m_name = json["name"];
		m_width = json["width"];
		m_height = json["height"];
		if (json.contains("connections"))
		{
			const auto& connections = json["connections"];
			for (const auto& connection : connections)
			{
				m_connections.emplace_back(connection);
			}
		}
		m_blk_path = json["blk"];
		m_block_and_tiles_name = json["bat"];
	}

	rsl::string_view ChunkComponent::name() const
	{
		return m_name;
	}
	const rsl::vector<ChunkConnection>& ChunkComponent::connections() const
	{
		return m_connections;
	}

}