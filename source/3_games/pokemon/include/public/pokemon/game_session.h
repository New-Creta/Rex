#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/old_code/save_file.h"
#include "pokemon/old_code/poke_structs.h"
#include "pokemon/old_code/map_coordinates.h"
#include "pokemon/old_code/game_blockmap.h"
#include "pokemon/old_code/player_character.h"

#include "rex_engine/assets/map.h"

#include "rex_engine/gfx/rendering/camera_2d.h"

#include "glm/glm.hpp"

#include "rex_std/bonus/math/point.h"
#include "rex_std/unordered_map.h"

#include "pokemon/state_task.h"
#include "pokemon/pkmn_renderer.h"

namespace pokemon
{
	class MapMatrix;
  class PokemonRenderer;
  struct MapRenderData;

  struct StateInfo
  {
    GameState state;
    s16 num_frames_active;
  };

  class GameSession
  {
  public:
    GameSession();
    ~GameSession();

    // Function called after all systems have been initialized.
    // Useful to call if you need to do some setup as part of the first frame
    // that you don't want to repeat in the update loop
    void post_init();
    void update();

  private:
    void init_globals();
    void init_state_tree();

    SaveFile load_startup_savefile() const;

  private:
    StateInfo m_state_info;
    rsl::unordered_map<GameState, StateTask> m_state_tree;
    Renderer m_renderer;
  };
}