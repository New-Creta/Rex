#pragma once

#include "window/window_event.h"

namespace rex
{
    namespace events
    {
        class WindowHide : public WindowEvent
        {
        public:
            EVENT_CLASS_TYPE(WindowHide);

            WindowHide(uint32 windowID/* , const memory::Blob& nativeEvent */);
            ~WindowHide() override;

            bool can_consume() const override;

            std::string to_string() const override;
        };
    }
}