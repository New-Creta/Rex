#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

#include "rex_std/bonus/math/point.h"

#include "pokemon/map_coordinates.h"

namespace pokemon
{
  struct SaveFile
  {
  public:
    SaveFile(rsl::string_view filepath);

  public:
    rsl::string current_map_filepath;
    TileCoord position;
  };
}