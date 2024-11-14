#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	enum class Direction
	{
		North,
		East,
		South,
		West
	};

	struct MapHeader;
	struct MapData;

	struct MapConnection
	{
		MapData* map;
		Direction direction;
		s8 offset;
	};

	struct WrapEvent
	{
		s8 x;
		s8 y;
		s8 dst_map_id;
		s8 dst_warp_id;
	};

	struct TextEvent // BgEvent in pokemon code
	{
		s8 x;
		s8 y;
		s8 sign_id;
	};

	struct ObjectEvent
	{
		s8 x;
		s8 y;
		s8 sprite_id;
		s8 movement; // walk/stay
		s8 range_or_direction;
		s8 text_id;
	};
	struct ItemObjectEvent : ObjectEvent
	{
		s8 item;
	};
	struct TrainerObjectEvent : ObjectEvent
	{
		s8 trainer_class;
		s8 trainer_number;
	};
	struct PokemonObjectEvent : ObjectEvent
	{
		s8 pokemon_id;
		s8 pokemon_level;
	};

	struct Trainer
	{
		s8 event_flag;
		s8 view_range;
		s8 text_before_battle;
		s8 text_after_battle;
		s8 text_end_battle;
	};

	struct MapHeader
	{
		rsl::string name;					// name of the map
		rsl::string map_blocks;		// path to the map blocks
		rsl::string blockset;			// path to the blockset metadata
		s8 width;		// represented in block count
		s8 height;	// represented in block count
		s8 border_block_idx; // Index of the block used for the border if no connection blocks are present
	};

	struct MapData
	{
		MapHeader map_header;
		rsl::vector<MapConnection> connections;
		rsl::vector<WrapEvent> wrap_events;
		bool fully_loaded;
	};
}