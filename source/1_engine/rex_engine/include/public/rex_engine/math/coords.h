#pragma once

#include "rex_engine/assets/map.h"

#include "rex_engine/engine/types_2d.h"

namespace rex
{
	rsl::pointi32 size_in_px(const Map* map);
	rsl::point<TileCount> size_in_tiles(const Map* map);

}