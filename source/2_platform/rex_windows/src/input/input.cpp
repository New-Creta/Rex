#include "rex_windows/input/input.h"

#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/assert.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

#include "rex_windows/input/internal/input.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace input
  {
    bool is_key_pressed(char8 key)
    {
      return internal::global_input_handler()->is_key_pressed(key);
    }
    bool is_key_down(char8 key)
    {
      return internal::global_input_handler()->is_key_down(key);
    }
    bool is_key_released(char8 key)
    {
      return internal::global_input_handler()->is_key_released(key);
    }
    bool is_key_up(char8 key)
    {
      return internal::global_input_handler()->is_key_up(key);
    }
  } // namespace input
} // namespace rex