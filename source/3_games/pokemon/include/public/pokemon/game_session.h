#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/game_loops/save_file.h"
#include "pokemon/poke_structs.h"
#include "pokemon/map_coordinates.h"
#include "pokemon/game_blockmap.h"
#include "pokemon/player_character.h"

#include "pokemon/game_loops/game_loop.h"

#include "rex_engine/assets/map.h"

#include "rex_engine/gfx/rendering/camera_2d.h"

#include "glm/glm.hpp"

#include "rex_std/bonus/math/point.h"
#include "rex_std/unordered_map.h"

#include "pokemon/state_task.h"
#include "pokemon/game_loops/game_loop_state.h"

namespace pokemon
{
	class MapMatrix;
  class PokemonRenderer;
  struct MapRenderData;

  struct StateInfo
  {
    GameLoopState state;
    s16 num_frames_active;
  };

  class GameSession
  {
  public:
    GameSession();
    ~GameSession();

    void update();

  private:
    void init_globals();
    void init_state_tree();

    SaveFile load_startup_savefile() const;

  private:
    StateInfo m_state_info;
    rsl::unordered_map<GameLoopState, StateTask> m_state_tree;
  };
}