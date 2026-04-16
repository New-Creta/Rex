#pragma once

#include "rex_engine/assets/tileset_asset.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/rendering/camera_2d.h"
#include "rex_engine/math/coords.h"

namespace rex
{
	namespace gfx
	{
		// structure holding all the required data to render a single frame
		// different render passes may use some of the data in here for their internal rendering logic
		struct SceneRenderParams
		{
			const TilesetAsset* tileset;					// tileset to be used for the active scene/world
			const u8* tiles_source;								// pointer to start of the buffer listing all tiles of the current scene, starting from the top left
			const Camera2D* camera;								// camera used by the current scene
			WorldCoordConverter coord_converter;  // converter used to calcualte coordinates of a different scale for the current scene
			TileCount world_width_in_tiles;				// the total width of the world, in tiles
		};
	}
}