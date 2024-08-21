#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

namespace pokemon
{
  struct SaveFile
  {
  public:
    SaveFile(rsl::string_view filepath);

  public:
    rsl::string current_map;
    rsl::string current_chunk;
  };
}