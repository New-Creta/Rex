#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

#include "rex_std/bonus/math/point.h"

#include "rex_engine/math/coords.h"

#include "pokemon/map_coordinates.h"
#include "pokemon/game_loops/game_loop_state.h"

namespace pokemon
{
  struct SaveFile
  {
  public:
    SaveFile(rsl::string_view filepath);

  public:
    rsl::string current_map_filepath;
    rex::TileCoord position;
  };
}