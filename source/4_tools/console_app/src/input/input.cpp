#include "console_app/input/input.h"

#include "rex_std/vector.h"
#include "rex_std/assert.h"
#include "rex_std/algorithm.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace input
  {
    card32 g_current_input_idx = 0;

    char8 poll()
    {
      constexpr card32 max_input_keys = (rsl::numeric_limits<char8>::max)();
      // process all subscribed keys, return as soon as one is find that's pressed
      // keep track which one we just processed so we can continue in the next call
      for (card32 i = g_current_input_idx; i < max_input_keys; ++i, ++g_current_input_idx)
      {
        char8 key = static_cast<char8>(i);

        if (GetKeyState(key) & 0x8000)
        {
          return key;
        }
      }

      g_current_input_idx = 0;
      return '\0';
    }
  }
}