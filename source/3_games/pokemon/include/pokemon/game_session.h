#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/poke_map.h"
#include "pokemon/save_file.h"
#include "pokemon/poke_structs.h"
#include "pokemon/tile_renderer.h"
#include "pokemon/map_coordinates.h"

#include "rex_engine/components/camera.h"
#include "rex_engine/gfx/rendering/scene_renderer.h"
#include "glm/glm.hpp"

#include "rex_std/bonus/math/point.h"

namespace pokemon
{
	class MapMatrix;
  struct MapRenderData;

  class GameSession
  {
  public:
    GameSession();

    void update();

  private:
    SaveFile load_startup_savefile() const;

    rsl::unique_ptr<Map> init_map(const SaveFile& startupSaveFile);
    TileRenderer* init_tile_renderer(const MapMatrix& mapMatrix, const MapRenderData& mapRenderData);
    void init_input();

  private:
    rsl::unique_ptr<Map> m_active_map;
    TileRenderer* m_tile_renderer;
    TileCoord m_player_position;
  };
}