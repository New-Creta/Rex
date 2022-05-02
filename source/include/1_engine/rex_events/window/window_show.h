#pragma once

#include "window/window_event.h"

namespace rex
{
    namespace events
    {
        class WindowShow : public WindowEvent
        {
        public:
            EVENT_CLASS_TYPE(WindowShow);

            WindowShow(uint32 windowID/* , const memory::Blob& nativeEvent */);
            ~WindowShow() override;

            bool can_consume() const override;

            std::string to_string() const override;
        };
    }
}