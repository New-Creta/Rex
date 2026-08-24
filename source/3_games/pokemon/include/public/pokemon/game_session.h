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

    void update();

  private:
    SaveFile load_startup_savefile() const;

    void init_map(const SaveFile& saveFile);
    void init_player(const SaveFile& saveFile);
    void init_camera();

    void update_render_info();


    void begin_copyright();
    GameLoopState tick_copyright();
    void end_copyright();

    void begin_black_borders();
    GameLoopState tick_black_borders();
    void end_black_borders();

    void begin_gamefreak();
    GameLoopState tick_gamefreak();
    void end_gamefreak();

    void begin_fight();
    GameLoopState tick_fight();
    void end_fight();

    void begin_startmenu();
    GameLoopState tick_startmenu();
    void end_startmenu();

    void begin_oak_intro();
    GameLoopState tick_oak_intro();
    void end_oak_intro();

    void begin_overworld();
    GameLoopState tick_overworld();
    void end_overworld();

    void begin_battle();
    GameLoopState tick_battle();
    void end_battle();



  private:
    StateInfo m_state_info;
    rsl::unordered_map<GameLoopState, StateTask> m_state_tree;


    rex::Map* m_active_map;

    // The blockmap, representing the current map and surroundings
    rsl::unique_ptr<GameBlockMap> m_scene_blockmap;
    rsl::unique_ptr<PlayerCharacter> m_player_character;

    rsl::unique_ptr<rex::gfx::Camera2D> m_camera;

    rsl::unique_ptr<GameLoop> m_game_loop;
  };
}