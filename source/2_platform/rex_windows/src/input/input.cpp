#include "rex_windows/input/input.h"

#include "rex_std/vector.h"
#include "rex_std/assert.h"
#include "rex_std/algorithm.h"

#include "rex_engine/diagnostics/win/win_call.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace input
  {
    card32 g_current_input_idx = 1;
    rsl::array<bool, 256> g_keys_even_frames = {};
    rsl::array<bool, 256> g_keys_odd_frames = {};
    rsl::array<bool, 256>* g_prev_keys = &g_keys_even_frames;
    rsl::array<bool, 256>* g_current_keys = &g_keys_odd_frames;

    uint8 poll()
    {
      constexpr card32 max_input_keys = (rsl::numeric_limits<uint8>::max)();
      for (card32 i = g_current_input_idx; i < max_input_keys; ++i, ++g_current_input_idx)
      {
        uint8 key = static_cast<uint8>(i);

        if (WIN_CALL(GetAsyncKeyState(key)) & 0x8000)
        {
          ++g_current_input_idx;
          return key;
        }
      }

      g_current_input_idx = 1;
      return '\0';
    }

    void update()
    {
      rsl::swap(g_prev_keys, g_current_keys);
      g_current_keys->fill(false);

      while (char8 key = input::poll())
      {
        if (key == '\0')
          break;

        g_current_keys->at(key) = true;
      }
    }

    bool is_key_pressed(char8 key)
    {
      return g_prev_keys->at(key) == false && g_current_keys->at(key) == true;
    }
    bool is_key_down(char8 key)
    {
      return g_prev_keys->at(key) == true && g_current_keys->at(key) == true;
    }
    bool is_key_released(char8 key)
    {
      return g_prev_keys->at(key) == true && g_current_keys->at(key) == false;
    }
    bool is_key_up(char8 key)
    {
      return g_current_keys->at(key) == false;
    }
  }
}