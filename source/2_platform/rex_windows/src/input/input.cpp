#include "rex_windows/input/input.h"

#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/assert.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace input
  {
    namespace internal
    {
      // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
      card32 g_current_input_idx               = 1;
      rsl::array<bool, 256> g_keys_even_frames = {};
      rsl::array<bool, 256> g_keys_odd_frames  = {};
      rsl::array<bool, 256>* g_prev_keys       = &g_keys_even_frames;
      rsl::array<bool, 256>* g_current_keys    = &g_keys_odd_frames;
      // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

      uint8 poll()
      {
        constexpr card32 max_input_keys = (rsl::numeric_limits<uint8>::max)();
        for(card32 i = g_current_input_idx; i < max_input_keys; ++i, ++g_current_input_idx)
        {
          const uint8 key = static_cast<uint8>(i);

          if(WIN_CALL(GetAsyncKeyState(key)) & 0x8000u) // NOLINT(hicpp-signed-bitwise)
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

        while(const uint8 key = input::internal::poll())
        {
          if(key == '\0')
            break;

          g_current_keys->at(key) = true;
        }
      }
    } // namespace internal

    bool is_key_pressed(char8 key)
    {
      return internal::g_prev_keys->at(key) == false && internal::g_current_keys->at(key) == true; // NOLINT(readability-simplify-boolean-expr)
    }
    bool is_key_down(char8 key)
    {
      return internal::g_prev_keys->at(key) == true && internal::g_current_keys->at(key) == true; // NOLINT(readability-simplify-boolean-expr)
    }
    bool is_key_released(char8 key)
    {
      return internal::g_prev_keys->at(key) == true && internal::g_current_keys->at(key) == false; // NOLINT(readability-simplify-boolean-expr)
    }
    bool is_key_up(char8 key)
    {
      return internal::g_prev_keys->at(key) == false && internal::g_current_keys->at(key) == false; // NOLINT(readability-simplify-boolean-expr)
    }
  } // namespace input
} // namespace rex