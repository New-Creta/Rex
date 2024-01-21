#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/array.h"

namespace rex
{
  namespace input
  {
    namespace internal
    {
      class Input
      {
      public:
        // Update the internal state of which keys are pressed and which ones aren't
        void update();

        // Returns true the frame the key is pressed
        bool is_key_pressed(char8 key) const;
        // Returns true as long as the key is pressed
        bool is_key_down(char8 key) const;
        // Returns true the frame the key is release
        bool is_key_released(char8 key) const;
        // Returns true as long as they is released
        bool is_key_up(char8 key) const;

      private:
        // Keeps polling input state of keys untill all have been processed
        uint8 poll();

      private:
        card32 m_current_input_idx = 1;
        rsl::array<bool, 256> m_keys_even_frames = {};
        rsl::array<bool, 256> m_keys_odd_frames = {};
        rsl::array<bool, 256>* m_prev_keys = &m_keys_even_frames;
        rsl::array<bool, 256>* m_current_keys = &m_keys_odd_frames;

      };

      // The global input handler. Allows user to query global input state (eg. input::is_key_down)
      void set_global_input_handler(const Input& input);
      const Input* global_input_handler();
    }
  }
}