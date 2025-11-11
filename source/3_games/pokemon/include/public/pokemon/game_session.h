#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/save_file.h"
#include "pokemon/poke_structs.h"
#include "pokemon/map_coordinates.h"
#include "pokemon/game_blockmap.h"
#include "pokemon/player_character.h"

#include "rex_engine/assets/map.h"

#include "rex_engine/gfx/rendering/render_passes/block_pass.h"
#include "rex_engine/gfx/rendering/render_passes/animated_sprites_pass.h"

#include "rex_engine/gfx/rendering/camera.h"
#include "rex_engine/gfx/rendering/scene_renderer.h"
#include "glm/glm.hpp"

#include "rex_std/bonus/math/point.h"

namespace pokemon
{
	class MapMatrix;
  class PokemonRenderer;
  struct MapRenderData;

  class GameSession
  {
  public:
    GameSession();

    void update();

  private:
    SaveFile load_startup_savefile() const;

    void init_map(const SaveFile& saveFile);
    void init_player(const SaveFile& saveFile);
    void init_renderer();

    void draw();

    void clamp_player_pos();

    void on_map_change();

  private:
    rex::Map* m_active_map;

    // The blockmap, representing the current map and surroundings
    rsl::unique_ptr<GameBlockMap> m_scene_blockmap;
    rsl::unique_ptr<PlayerCharacter> m_player_character;

    //rex::gfx::BlockRenderPass* m_block_render_pass;
    //rex::gfx::AnimatedSpritesPass* m_animated_sprites_pass;
    
    //PokemonRenderer* m_renderer;
  };
}