#pragma once

#include "pokemon/map_coordinates.h"

namespace pokemon
{
	// player sprite is 16 x 16 pixels
	class PlayerCharacter
	{
	public:
		TileCoord pos() const;

	private:
		TileCoord m_pos;
	};
}