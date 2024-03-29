#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace input
  {
    bool is_key_pressed(char8 key);
    bool is_key_down(char8 key);
    bool is_key_released(char8 key);
    bool is_key_up(char8 key);
  } // namespace input
} // namespace rex