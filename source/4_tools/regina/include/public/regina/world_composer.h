#pragma once

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/tilemap.h"
#include "rex_engine/shapes/minmax.h"

#include "rex_std/unordered_map.h"

namespace regina
{
	// The name came from asking ChatGPT the following prompt
	// What would be a good name for an object that can parse 2D maps that are connected to each other
	// and build a giant tilemap based on those connections as well as maintain their metadata (AABB etc) after the fact,
	// so it can be queried if a new map is within the previously build tilemap that encapsulates the tilemaps of all maps
	class WorldComposer
	{
		struct MapMetaData
		{
			rex::MinMax aabb;
		};

	public:
		bool is_map_in_tilemap(const rex::Map* map) const;
		rsl::pointi32 map_pos(const rex::Map* map) const;
		const rex::Tilemap* tilemap() const;

		void build_world(rex::Map* startMap);


	private:
		void load_all_maps(rex::Map* startMap);
		void build_world_in_relative_coord(const rex::Map* startMap);
		void convert_world_to_abs_coord();

		rex::MinMax calc_map_aabb(const rex::MapHeader& mapHeader, rsl::pointi32 startPos);

	private:
		// Represents a lookup for every map to its metadata
		rsl::unordered_map<const rex::Map*, MapMetaData> m_map_to_metadata;

		// Represents the tilemap of the entire world.
		// Note: A tilemap is a 2D matrix, so if you don't have a dense populated world
		// There will be a lot of empty entries within the matrix which wastes memory
		rsl::unique_ptr<rex::Tilemap> m_tilemap;
	};
}