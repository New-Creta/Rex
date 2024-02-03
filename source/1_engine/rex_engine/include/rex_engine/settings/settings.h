#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace settings
  {
    s32 get(rsl::string_view name);

    void load(rsl::string_view path);
  }
}