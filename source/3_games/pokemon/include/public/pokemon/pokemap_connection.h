#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	class MapConnection
	{
	public:

	private:
		Blocks* m_blocks;				// The blocks used by the map we're connecting with
		s8 m_map_id;					  // The map id of the map we're connecting with
		s8 m_width_in_blocks;		// The width of the map we're connecting with, measured in blocks

		// the offset gets added to the x axis for north and south connections
		// it gets added to the y axis for the east and west connections
		// as the axis you're connecting on is always aligned, you only need 1 value
		// to move the connected map in 1 direction
		s8 m_offset;					  // The offset of this map from the map we're coming from.

	};
}