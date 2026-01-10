#pragma once

#include "rex_engine/assets/asset.h"

#include "rex_engine/assets/blockset.h"
#include "rex_engine/assets/map_objects/map_character.h"
#include "rex_engine/assets/map_objects/map_pickup.h"
#include "rex_engine/assets/map_objects/map_pokemon.h"
#include "rex_engine/assets/map_objects/map_textboard.h"
#include "rex_engine/assets/map_objects/map_trainer.h"
#include "rex_engine/assets/map_objects/map_warp.h"

#include "rex_engine/serialization/asset_loader.h"

#include "rex_engine/math/coords.h"

#include "rex_engine/engine/types.h"
#include "rex_std/string.h"
#include "rex_std/memory.h"

namespace rex
{
	// The game only supports directions in 90 degree angles
	// These directions are used for facings of characters 
	// as well as indicate map connections
	enum class Direction : s8
	{
		North,
		East,
		South,
		West
	};

	struct MapHeader
	{
		rsl::string name;											// name of the map
		rsl::point<BlockCount> size;
		s8 border_block_idx;									// Index of the block used for the border if no connection blocks are present
	};

	class Map;
	struct MapConnection
	{
		Map* map;
		Direction direction;

		// The offset is the how many tiles in a perpendicular angle of the direction a connection is connected
		// Example:
		// A connection to the north with an offset of 2
		// The northern map's center will be offsetted 2 pixels to the right of the current map's center
		s8 offset;
	};

	// This is the data that gets saved to disk
	struct MapDesc
	{
		// The map header, holding high level info of the map
		// This holds all the information to identify the map without needing to load all the objects of the map
		MapHeader map_header;

		// The connections of a map. This means where a player can walk to from this map without going through a warp
		rsl::unique_array<MapConnection> connections;

		// The objects within a map (characters, items, pokemon, ...)
		// An object is "something" in a map the player can interact with
		rsl::unique_array<rsl::unique_ptr<MapObject>> objects;

		// A warp is a space that teleports the player into another map
		// A good example of this is a door
		rsl::unique_array<WarpEvent> warps;

		// Scripts that run within a map (currently not implemented yet)
		rsl::unique_array<rsl::string> scripts;

		// The block indices of the map. This is required for rendering
		rsl::unique_array<u8> blocks;

		// The blockset used by the map. This is required for rendering
		Blockset* blockset;
	};

	class Map : public Asset
	{
	public:
		explicit Map(MapDesc&& desc);

		const MapDesc& desc() const;
		const u8* blocks(s32 offset = 0) const;
		const Blockset* blockset() const;

		BlockCount width() const;
		BlockCount height() const;

		WorldCoordConverter create_world_coord_converter();

	private:
		MapDesc m_desc;
	};
}