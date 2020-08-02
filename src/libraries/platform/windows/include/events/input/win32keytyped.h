#pragma once

#include "events/input/keytyped.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"

namespace rex
{
    class Window;

    namespace win32
    {
        class KeyTyped : public events::KeyTyped
        {
        public:
            KeyTyped(rex::Window* window, rex::win32::MessageParameters params);
            ~KeyTyped() final;

        private:
            KeyState m_key_state;
        };
    }
}