#pragma once

#include "events/input/keytyped.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
    class Window;

    namespace win32
    {
        class KeyTyped : public events::KeyTyped, public NativeEvent
        {
        public:
            KeyTyped(rex::Window* window, rex::win32::MessageParameters params);
            ~KeyTyped() final;

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }

        private:
            KeyState m_key_state;
        };
    }
}