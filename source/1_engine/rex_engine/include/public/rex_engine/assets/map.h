#pragma once

#include "rex_engine/assets/asset.h"

#include "rex_engine/assets/blockset.h"
#include "rex_engine/serialization/serializer_base.h"

#include "rex_engine/engine/types.h"
#include "rex_std/string.h"
#include "rex_std/memory.h"

namespace rex
{
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
		Blockset* blockset;										// the blockset to be used by the map
		s8 width_in_blocks;										// represented in block count
		s8 height_in_blocks;									// represented in block count
		s8 border_block_idx;									// Index of the block used for the border if no connection blocks are present
	};

	class Map;
	struct MapConnection
	{
		Map* map;
		Direction direction;
		s8 offset;
	};

	struct WarpEvent
	{
		rsl::pointi8 pos;
		s8 dst_map_id;
		s8 dst_warp_id;
	};

	struct TextEvent // BgEvent in pokemon code
	{
		rsl::pointi8 pos;
		rsl::string text;
		s8 sign_id;
	};

	//struct MapObject // Characters
	//{
	//	rsl::string name;
	//};

	enum class ObjectEventType
	{
		Character, // By default
		Item,
		Trainer,
		Pokemon
	};

	struct ObjectEvent
	{
		rsl::pointi8 pos;
		rsl::string text_id;
		rsl::string direction;
		rsl::string movement; // walk/stay
		rsl::string sprite_id;
	};
	struct CharacterObjectEvent : ObjectEvent
	{
		// Has no extra members
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

	struct MapScript
	{
		rsl::string name;
	};

	struct MapObject
	{
		rsl::string name;
		rsl::string text_id;
		rsl::string sprite_id;

		rsl::pointi8 pos;
	};

	enum class MapCharacterMovement
	{
		Walk,
		Stay
	};
	enum class MapCharacterMovementDirection
	{
		None,
		UpDown,
		LeftRight,
		AnyDir
	};
	class MapCharacter : public MapObject
	{
	private:
		MapCharacterMovementDirection m_direction;
		MapCharacterMovement m_movement;
	};
	class MapTrainer : public MapCharacter
	{
	private:
		s8 m_trainer_class;
		s8 m_trainer_number;
	};
	class MapPickup : public MapObject
	{
		s8 m_pickup_id;
	};
	class MapPokemon : public MapObject
	{
	private:
		s8 m_pokemon_id;
		s8 m_pokemon_level;
	};
	class MapTextBoard : public MapObject
	{

	};

	// This is the data that gets saved to disk
	struct MapDesc
	{
		// The map header, holding high level info of the map
		// This holds all the information to identify the map without needing to load all the objects of the map
		MapHeader map_header;

		// The connections of a map. This means where a player can walk to from this map without going through a warp
		rsl::unique_array<MapConnection> connections;

		// Holds all the pickups, text signs, trainers, ... that are present in the current map
		rsl::unique_array<rsl::unique_ptr<MapObject>> objects;

		// The objects within a map (characters, items, pokemon, ...)
		// This should be merged with object and text events
		// An object is "something" in a map the player can interact with
		//rsl::unique_array<MapObject> objects;
		rsl::unique_array<rsl::unique_ptr<ObjectEvent>> object_events;
		rsl::unique_array<TextEvent> text_events;

		// A warp is a space that teleports to the player into another map
		// A good example of this is a door
		rsl::unique_array<WarpEvent> warps;

		// Scripts that run within a map (currently not implemented yet)
		rsl::unique_array<rsl::string> scripts;

		// The block indices of the map. This is required for rendering
		rsl::unique_array<u8> blocks;

		rsl::string blockset; // path to blockset
		rsl::string blockmap; // path to block map
	};

	class Map : public Asset
	{
	public:
		Map(MapDesc&& desc, LoadFlags loadFlags);

		const MapDesc& desc() const;
		const u8* blocks(s32 offset = 0) const;
		const u8* tiles(s32 offset = 0) const;
		const Blockset* blockset() const;

		s32 width_in_tiles() const;
		s32 height_in_tiles() const;

	private:
		void load_blocks();
		void load_tiles();

		//s32 width_in_blocks() const;
		//s32 height_in_blocks() const;
		//s32 border_block_idx() const;

		//const rsl::unique_array<MapConnection>& connections() const;
		//const rsl::unique_array<u8>& blocks() const;

	private:
		MapDesc m_desc;
		Blockset* m_blockset;
		rsl::unique_array<u8> m_tiles;
		rsl::unique_array<u8> m_blocks;
	};
}