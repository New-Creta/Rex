#include "rex_windows/input/internal/input.h"

#include "rex_engine/platform/win/diagnostics/win_call.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace input
  {
    namespace internal
    {
      uint8 Input::poll()
      {
        // Pull all the possible input keys and check which one is set
        constexpr card32 max_input_keys = (rsl::numeric_limits<uint8>::max)();
        for (card32 i = m_current_input_idx; i < max_input_keys; ++i, ++m_current_input_idx)
        {
          const uint8 key = static_cast<uint8>(i);

          if (WIN_CALL(GetAsyncKeyState(key)) & 0x8000u) // NOLINT(hicpp-signed-bitwise)
          {
            // If a key is set, make sure we set the last index we processed so we don't have to restart
            // then return the key that's pressed
            ++m_current_input_idx;
            return key;
          }
        }

        // if no other keys are pressed, reset the idx so we loop over all keys again the next time
        m_current_input_idx = 1;
        return '\0';

      }
      void Input::update()
      {
        // This is similar to presenting with backbuffer and front buffer swapping
        rsl::swap(m_prev_keys, m_current_keys);
        m_current_keys->fill(false);

        // Keep polling untill all keys are processed
        while (const uint8 key = poll())
        {
          // Break out of the loop if all keys are processed
          if (key == '\0')
            break;

          // If a pressed key is found, flag it as such in our internal array
          m_current_keys->at(key) = true;
        }
      }

      // Returns true the frame the key is pressed
      bool Input::is_key_pressed(char8 key) const
      {
        return m_prev_keys->at(key) == false && m_current_keys->at(key) == true; // NOLINT(readability-simplify-boolean-expr)
      }
      // Returns true as long as the key is pressed
      bool Input::is_key_down(char8 key) const
      {
        return m_prev_keys->at(key) == true && m_current_keys->at(key) == true; // NOLINT(readability-simplify-boolean-expr)
      }
      // Returns true the frame the key is release
      bool Input::is_key_released(char8 key) const
      {
        return m_prev_keys->at(key) == true && m_current_keys->at(key) == false; // NOLINT(readability-simplify-boolean-expr)
      }
      // Returns true as long as they is released
      bool Input::is_key_up(char8 key) const
      {
        return m_prev_keys->at(key) == false && m_current_keys->at(key) == false; // NOLINT(readability-simplify-boolean-expr)
      }

      const Input* g_global_input_handler = nullptr;
      void set_global_input_handler(const Input& input)
      {
        g_global_input_handler = &input;
      }
      const Input* global_input_handler()
      {
        return g_global_input_handler;
      }
    }
  }
}