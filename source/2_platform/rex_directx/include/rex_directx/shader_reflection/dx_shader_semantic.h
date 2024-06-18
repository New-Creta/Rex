#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  namespace gfx
  {
    // Extend this enum when required
    // If there's a way to get a semantic of a constant buffer variable
    // Please use that instead and delete this file
    enum class Semantic
    {
      Invalid,
      Model,
      View,
      Proj,
      MVP
    };

    Semantic var_name_to_semantic(rsl::string_view name);
  }
}