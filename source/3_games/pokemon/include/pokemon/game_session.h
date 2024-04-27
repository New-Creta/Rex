#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/level.h"

namespace pokemon
{
  // A game session is an object that holds the current state of the game
  class GameSession
  {
  public:
    GameSession();

  private:
    void initialise_start_level();

  private:
    rsl::vector<LevelInfo> m_levels_info;
    rsl::unique_ptr<Level> m_current_level;
  };
}